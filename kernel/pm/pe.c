#include <kernel/pm/pe.h>
#include <kernel/fs/vfs.h>
#include <kernel/mm/virt_memory.h>
#include <kernel/mm/kheap.h>
#include <kernel/tty.h>

#include <kernel/libk/string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define PE_DEBUG(...) qemu_printf("PE_DEBUG: "__VA_ARGS__)

static char last_pe_name[512] = "none"; 
static const char* pe_libdir = "/apps/";

static inline bool is_powerof2(uint32_t val) {
    if (val == 0)
        return false;
    return (val & (val - 1)) == 0;
}

static uintptr_t pe_get_export_fn_by_name(uintptr_t image_base, char *name) {
    pe_pimage_dos_header_t dos = pe_get_dos_header(image_base);
    pe_pimage_nt_headers32_t nt = pe_get_nt_header(dos);
    pe_pimage_export_directory_t export_dir = pe_get_export_directory(image_base, nt);

    uintptr_t* fn_ptrs = PE_MAKE_PTR(uintptr_t*, image_base, export_dir->address_of_functions);
    uint16_t* ordinals = PE_MAKE_PTR(uint16_t*, image_base, export_dir->address_of_name_ordinals);

    char** fn_names = PE_MAKE_PTR(char**, image_base, export_dir->address_of_names);
    
    for (int i=0; i < export_dir->number_of_functions; i++) {
        if (!ordinals[i]) {
            continue;
        }
        if(!strcmp(name, PE_MAKE_PTR(char*, image_base, fn_names[ordinals[i]-1]))) {
            return image_base + fn_ptrs[ordinals[i]];
        }
    }
    return 0;
}

static bool pe_validate(uintptr_t base, size_t size) {
    pe_pimage_dos_header_t dos = pe_get_dos_header(base);

    if (!base || size < sizeof(pe_image_dos_header_t))
        return false;

    if (dos->e_magic != PE_IMAGE_DOS_SIGNATURE || dos->e_lfanew <= 0)
        return false;

    pe_pimage_nt_headers32_t nt = pe_get_nt_header(dos);

    if ((uintptr_t)nt < (uintptr_t)base)
        return false;

    if (nt->signature != PE_IMAGE_NT_SIGNATURE)
        return false;

    if (nt->file_header.machine != PE_IMAGE_FILE_MACHINE_I386)
        return false;

    if (nt->optional_header.magic != PE_IMAGE_NT_OPTIONAL_HDR32_MAGIC)
        return false;

    if (nt->optional_header.section_alignment < PAGE_SIZE) {
        if (nt->optional_header.file_alignment != nt->optional_header.section_alignment) {
            return false;
        }
    }
    else if (nt->optional_header.section_alignment < nt->optional_header.file_alignment)
        return false;

    if (!is_powerof2(nt->optional_header.section_alignment) ||
        !is_powerof2(nt->optional_header.file_alignment))
        return false;

    if (nt->file_header.number_of_sections > PE_MAX_SECTIONS)
        return false;
    return true;
}

static uintptr_t pe_create_image(uintptr_t image_base, uintptr_t file_ptr) {
    pe_pimage_dos_header_t dos = pe_get_dos_header(file_ptr);
    pe_pimage_nt_headers32_t nt = pe_get_nt_header(dos);

    memcpy((void*)image_base, (void*)file_ptr, nt->optional_header.size_of_headers);

    pe_pimage_section_header_t section_h = pe_get_section_header(nt); 
    uint32_t section_align = nt->optional_header.section_alignment;

    for (uint16_t i = 0; i < nt->file_header.number_of_sections; i++, section_h++) {
        if(section_h->virtual_address > nt->optional_header.size_of_image) {
            PE_DEBUG("Bad VA section '%s'. Skip.\n", section_h->name);
            continue;
        }

        void* data = PE_MAKE_PTR(void*, file_ptr, section_h->pointer_to_raw_data);
        void* virt = PE_MAKE_PTR(void*, image_base, section_h->virtual_address);
        if (section_h->size_of_raw_data) {
            memcpy(virt, data, section_h->size_of_raw_data);   
        }

        size_t section_size = (section_h->misc.virtual_size + section_align - 1) & (-section_align);

        if (section_size > section_h->size_of_raw_data) {
            memset(PE_MAKE_PTR(void*, virt, section_h->size_of_raw_data), 0, section_size - section_h->size_of_raw_data);
        }
    }

    pe_image_data_directory_t reloc_dir = pe_get_data_directory(nt, PE_IMAGE_RELOC_DIRECTORY);

    if (image_base != nt->optional_header.image_base) {
        if (reloc_dir.size) {
            uint32_t delta = image_base - nt->optional_header.image_base;
            pe_pimage_base_relocation_t reloc = pe_get_base_relocation(image_base, &reloc_dir);

            while (reloc->size_of_block) {
                uint32_t count = (reloc->size_of_block - sizeof(*reloc)) / sizeof(uint16_t);
                uint16_t *entry = PE_MAKE_PTR(uint16_t*, reloc, sizeof(*reloc));

                for (int i = 0; i < count; i++) {
                    uint16_t *p16;
                    uint32_t *p32;

                    uint16_t reloc_type = (*entry & 0xF000) >> 12;
                    uint32_t offs = (*entry & 0x0FFF) + reloc->virtual_address;

                    switch(reloc_type) {
                        case PE_IMAGE_REL_BASED_HIGH:
                            p16 = PE_MAKE_PTR(uint16_t*, image_base, offs);
                            *p16+= (uint16_t)(delta >> 16);
                            break;
                        case PE_IMAGE_REL_BASED_LOW:
                            p16 = PE_MAKE_PTR(uint16_t*, image_base, offs);
                            *p16+= (uint16_t)delta;
                            break;
                        case PE_IMAGE_REL_BASED_HIGHLOW:
                            p32 = PE_MAKE_PTR(uint32_t*, image_base, offs);
                            *p32+= delta;
                    }
                    entry++;
                }
                reloc = PE_MAKE_PTR(pe_pimage_base_relocation_t, reloc, reloc->size_of_block);
            }
        }
    }

    PE_DEBUG("Create PE base %x (original PE base %x), size %x, %d sections\n",
                image_base, nt->optional_header.image_base, nt->optional_header.size_of_image, nt->file_header.number_of_sections);
    return image_base;
}


static void *pe_open(const char *fname) {
    size_t fsize = vfs_get_size(fname);
    if (!fsize) {
        return 0;
    }
    void *addr = kmalloc(fsize);
    int res = vfs_read(fname, 0, fsize, addr);
    if (!pe_validate((uintptr_t)addr, fsize)) {
        kfree(addr);
        return NULL;
    }
    return addr;
}

char* pe_get_last_loaded_file(void) {
    return last_pe_name; 
}

pe_load_t pe_load(const char* name) {
    pe_load_t pe;
    pe.image_base = PE_BAD_IMAGE_BASE;
    pe.entry_retcode = 0;
    strncpy(last_pe_name, name, 512-1);

    uintptr_t pe_file = (uintptr_t)pe_open(name);
    if (!pe_file) { 
       return pe;
    }

    pe_pimage_dos_header_t dos = pe_get_dos_header(pe_file);
    pe_pimage_nt_headers32_t nt = pe_get_nt_header(dos);

    pe.image_base = nt->optional_header.image_base;

    if (nt->file_header.characteristics & PE_IMAGE_FILE_DLL) {
        pe.image_base = (uintptr_t)kmalloc(nt->optional_header.size_of_image);
    } else {
        uint32_t alloc_addr;
        for (alloc_addr = pe.image_base;
            alloc_addr <= pe.image_base + nt->optional_header.size_of_image;
            alloc_addr += PAGE_SIZE) {
            vmm_alloc_page(alloc_addr);
        }
    }

    pe.image_base = pe_create_image(pe.image_base, (uintptr_t)pe_file);
    
    kfree((void*)pe_file);

    dos = pe_get_dos_header(pe.image_base);
    nt = pe_get_nt_header(dos);

    pe_pimage_import_descriptor_t import_libs = pe_get_import_descriptor(pe.image_base, nt);
    while (import_libs->name && !import_libs->time_date_stamp) {
        pe_pimage_thunk_data32_t iat = pe_get_iat(pe.image_base, import_libs);
        pe_pimage_thunk_data32_t ilt = pe_get_ilt(pe.image_base, import_libs);

        char fullpath[512];
        strcpy(fullpath, pe_libdir);
        strcat(fullpath, PE_MAKE_PTR(char*, pe.image_base, import_libs->name));

        pe_load_t pedll = pe_load(fullpath);

        if (pedll.image_base == PE_BAD_IMAGE_BASE) {
            memcpy(&pe, &pedll, sizeof(pe_load_t));
            goto free;
        }

        while (true) {
            if (ilt->address_of_data == 0 || ilt->ordinal & PE_IMAGE_ORDINAL_FLAG) {
                break;
            }
            pe_pimage_import_by_name_t record = pe_get_import_by_name(pe.image_base, ilt);
            uintptr_t fn_ptr = pe_get_export_fn_by_name(pedll.image_base, record->name);

            PE_DEBUG("Resolve '%s' import in '%s' from '%s' : %s\n", record->name, name, fullpath, fn_ptr ? "OK" : "ERR" );
            kfree(fullpath);
            if (fn_ptr) {
                iat->address_of_data = fn_ptr;
            } else {
                pe.image_base = PE_BAD_IMAGE_BASE;
                goto free;
            }

            iat++;
            ilt++;
        }
        import_libs++;
    }

    int (*entry_point)(void) = pe_get_entry(pe.image_base, nt);
    PE_DEBUG("PE entry point: %x\n", entry_point);
    pe.entry_retcode = entry_point();
     
free:
    if (!(nt->file_header.characteristics & PE_IMAGE_FILE_DLL)) {
        uint32_t alloc_addr;
        for (alloc_addr = pe.image_base;
            alloc_addr <= pe.image_base + nt->optional_header.size_of_image;
            alloc_addr += PAGE_SIZE) {
            vmm_free_page(alloc_addr);
        }
    }
    return pe;
}