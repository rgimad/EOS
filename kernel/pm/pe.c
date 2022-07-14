#include <kernel/fs/vfs.h>
#include <kernel/io/qemu_log.h>
#include <kernel/libk/string.h>
#include <kernel/mm/kheap.h>
#include <kernel/mm/virt_memory.h>
#include <kernel/pm/pe.h>
#include <kernel/tty.h>

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define PE_DEBUG(...) qemu_printf("PE_DEBUG: "__VA_ARGS__)
static const char* pe_libdir = "/apps/";

static uintptr_t pe_load_dll(const char* name, dll_list_t* dll_list, pe_status_t* status);

static inline bool is_powerof2(uint32_t val) {
    if (val == 0)
        return false;
    return (val & (val - 1)) == 0;
}

static bool dll_list_init(dll_list_t* list, uint32_t size) {
    list->_max_size = size;
    list->_pos = -1;
    list->image_bases = kmalloc(list->_max_size);
    if (!list->image_bases) {
        return false;
    }
    return true;
}

static bool dll_list_add(dll_list_t* list, uintptr_t image_base) {
    list->_pos++;
    if (list->_pos > list->_max_size) {
        return false;
    }
    list->image_bases[list->_pos] = image_base;
    PE_DEBUG("Adeed DLL image_base = %x\n", image_base);
    return true;
}

static void dll_list_free(dll_list_t* list) {
    while (list->_pos >= 0) {
        kfree((void*)list->image_bases[list->_pos]);
        PE_DEBUG("Free DLL image_base = %x\n", list->image_bases[list->_pos]);
        list->_pos--;
    }
    kfree(list->image_bases);
    list->image_bases = NULL;
    list->_max_size = 0;
}

static bool pe_validate(uintptr_t base, size_t size) {
    pe_pimage_dos_header_t dos = pe_get_dos_header(base);

    if (!base || size < sizeof(pe_image_dos_header_t))
        return false;

    if (dos->e_magic != PE_IMAGE_DOS_SIGNATURE || dos->e_lfanew <= 0)
        return false;

    pe_pimage_nt_headers32_t nt = pe_get_nt_headers(dos);

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
    } else if (nt->optional_header.section_alignment < nt->optional_header.file_alignment)
        return false;

    if (!is_powerof2(nt->optional_header.section_alignment) || !is_powerof2(nt->optional_header.file_alignment))
        return false;

    if (nt->file_header.number_of_sections > PE_MAX_SECTIONS)
        return false;
    return true;
}

static uintptr_t pe_create_image(uintptr_t image_base, uintptr_t file_ptr) {
    pe_pimage_dos_header_t dos = pe_get_dos_header(file_ptr);
    pe_pimage_nt_headers32_t nt = pe_get_nt_headers(dos);

    memcpy((void*)image_base, (void*)file_ptr,
           nt->optional_header.size_of_headers);

    pe_pimage_section_header_t section_h = pe_get_section_header(nt);
    uint32_t section_align = nt->optional_header.section_alignment;

    for (uint16_t i = 0; i < nt->file_header.number_of_sections;
         i++, section_h++) {
        if (section_h->virtual_address > nt->optional_header.size_of_image) {
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
            memset(PE_MAKE_PTR(void*, virt, section_h->size_of_raw_data), 0,
                   section_size - section_h->size_of_raw_data);
        }
    }

    pe_image_data_directory_t reloc_dir = pe_get_data_directory(nt, PE_IMAGE_RELOC_DIRECTORY);

    if (image_base != nt->optional_header.image_base) {
        if (reloc_dir.size) {
            uint32_t delta = image_base - nt->optional_header.image_base;
            pe_pimage_base_relocation_t reloc = pe_get_base_relocation(image_base, &reloc_dir);

            while (reloc->size_of_block) {
                uint32_t count = (reloc->size_of_block - sizeof(*reloc)) / sizeof(uint16_t);
                uint16_t* entry = PE_MAKE_PTR(uint16_t*, reloc, sizeof(*reloc));

                for (uint32_t i = 0; i < count; i++) {
                    uint16_t* p16;
                    uint32_t* p32;

                    uint16_t reloc_type = (*entry & 0xF000) >> 12;
                    uint32_t offs = (*entry & 0x0FFF) + reloc->virtual_address;

                    switch (reloc_type) {
                    case PE_IMAGE_REL_BASED_HIGH:
                        p16 = PE_MAKE_PTR(uint16_t*, image_base, offs);
                        *p16 += (uint16_t)(delta >> 16);
                        break;
                    case PE_IMAGE_REL_BASED_LOW:
                        p16 = PE_MAKE_PTR(uint16_t*, image_base, offs);
                        *p16 += (uint16_t)delta;
                        break;
                    case PE_IMAGE_REL_BASED_HIGHLOW:
                        p32 = PE_MAKE_PTR(uint32_t*, image_base, offs);
                        *p32 += delta;
                    }
                    entry++;
                }
                reloc = PE_MAKE_PTR(pe_pimage_base_relocation_t, reloc,
                                    reloc->size_of_block);
            }
        }
    }

    PE_DEBUG("Create PE base %x (original PE base %x), size %x, %d sections\n",
             image_base, nt->optional_header.image_base,
             nt->optional_header.size_of_image,
             nt->file_header.number_of_sections);
    return image_base;
}

static pe_error_t pe_open(const char* fname, void** addr) {
    size_t fsize = vfs_get_size(fname);
    if (!fsize) {
        return PE_ERR_FILE_NOT_FOUND;
    }
    *addr = kmalloc(fsize);
    if (!*addr) {
        return PE_ERR_ALLOC;
    }
    vfs_read(fname, 0, fsize, *addr);
    if (!pe_validate((uintptr_t)*addr, fsize)) {
        kfree(addr);
        return PE_ERR_INVALID_FILE;
    }
    return 0;
}

static uintptr_t pe_get_export_fn_by_name(uintptr_t image_base, char* imp_name) {
    pe_pimage_dos_header_t dos = pe_get_dos_header(image_base);
    pe_pimage_nt_headers32_t nt = pe_get_nt_headers(dos);
    pe_pimage_export_directory_t export_dir = pe_get_export_directory(image_base, nt);

    uintptr_t* fn_ptrs = PE_MAKE_PTR(uintptr_t*, image_base, export_dir->address_of_functions);
    uint16_t* ordinals = PE_MAKE_PTR(uint16_t*, image_base, export_dir->address_of_name_ordinals);

    char** fn_names = PE_MAKE_PTR(char**, image_base, export_dir->address_of_names);

    for (uint32_t i = 0; i < export_dir->number_of_functions; i++) {
        if (!ordinals[i]) {
            continue;
        }
        char* exp_name = PE_MAKE_PTR(char*, image_base, fn_names[ordinals[i] - 1]);
        if (!strcmp(imp_name, exp_name)) {
            return image_base + fn_ptrs[ordinals[i]];
        }
    }
    return 0;
}

static pe_error_t pe_resolve_import(uintptr_t image_base, const char* mom_name, dll_list_t* dll_list, const pe_status_t* status) {
    pe_pimage_nt_headers32_t nt = pe_get_nt_headers(pe_get_dos_header(image_base));
    pe_pimage_import_descriptor_t import_libs = pe_get_import_descriptor(image_base, nt);

    while (import_libs->name && !import_libs->time_date_stamp) {
        pe_pimage_thunk_data32_t iat = pe_get_iat(image_base, import_libs);
        pe_pimage_thunk_data32_t ilt = pe_get_ilt(image_base, import_libs);

        char fullpath[PE_FILENAME_MAX];
        strcpy(fullpath, pe_libdir);
        strcat(fullpath, PE_MAKE_PTR(char*, image_base, import_libs->name));

        uintptr_t dll_image_base = pe_load_dll(fullpath, dll_list, (pe_status_t*)status);
        if (dll_image_base == (uintptr_t)PE_BAD_IMAGE_BASE) {
            return status->err_code;
        }

        while (true) {
            if (ilt->address_of_data == 0 || ilt->ordinal & PE_IMAGE_ORDINAL_FLAG) {
                break;
            }
            pe_pimage_import_by_name_t record = pe_get_import_by_name(image_base, ilt);
            uintptr_t fn_ptr = pe_get_export_fn_by_name(dll_image_base, record->name);

            PE_DEBUG("Resolve '%s' import in '%s' from '%s' : %s\n", record->name,
                     mom_name, fullpath, fn_ptr ? "OK" : "ERR");

            if (fn_ptr) {
                iat->address_of_data = fn_ptr;
            } else {
                return PE_ERR_EXP_SYM_NOT_FOUND;
            }

            iat++;
            ilt++;
        }
        import_libs++;
    }
    return 0;
}

static uintptr_t pe_load_dll(const char* name, dll_list_t* dll_list, pe_status_t* status) {
    strncpy(status->file_name, name, PE_FILENAME_MAX - 1);

    void* pe_file = NULL;
    status->err_code = (uintptr_t)pe_open(name, &pe_file);
    if (status->err_code) {
        return PE_BAD_IMAGE_BASE;
    }

    pe_pimage_nt_headers32_t nt = pe_get_nt_headers(pe_get_dos_header(pe_file));

    uintptr_t image_base = (uintptr_t)kmalloc(nt->optional_header.size_of_image);
    if (!image_base) {
        status->err_code = PE_ERR_ALLOC;
        return PE_BAD_IMAGE_BASE;
    }

    image_base = pe_create_image(image_base, (uintptr_t)pe_file);
    kfree((void*)pe_file);

    status->err_code = pe_resolve_import(image_base, name, dll_list, status);
    if (status->err_code) {
        kfree((void*)image_base);
        return PE_BAD_IMAGE_BASE;
    }
    if (!dll_list_add(dll_list, image_base)) {
        status->err_code = PE_ERR_DLL_LIMIT;
        return PE_BAD_IMAGE_BASE;
    }
    return image_base;
}

int run_pe(const char* name, pe_status_t* status) {
    strncpy(status->file_name, name, PE_FILENAME_MAX - 1);
    status->err_code = 0;

    dll_list_t dll_list;
    if (!dll_list_init(&dll_list, PAGE_SIZE)) {
        status->err_code = PE_ERR_ALLOC;
        return 0;
    }

    void* pe_file = NULL;
    status->err_code = pe_open(name, &pe_file);
    if (status->err_code) {
        return 0;
    }

    pe_pimage_nt_headers32_t nt = pe_get_nt_headers(pe_get_dos_header(pe_file));
    uintptr_t image_base = nt->optional_header.image_base;

    for (uintptr_t alloc_addr = image_base;
         alloc_addr <= image_base + nt->optional_header.size_of_image;
         alloc_addr += PAGE_SIZE) {
        vmm_alloc_page((void*)alloc_addr);
    }

    image_base = pe_create_image(image_base, (uintptr_t)pe_file);
    kfree((void*)pe_file);

    nt = pe_get_nt_headers(pe_get_dos_header(image_base));

    status->err_code = pe_resolve_import(image_base, name, &dll_list, status);
    if (status->err_code) {
        goto free;
    }

    int (*entry_point)(void) = pe_get_entry(image_base, nt);
    PE_DEBUG("PE entry point: %x\n", entry_point);
    int entry_retcode = entry_point();

free:
    dll_list_free(&dll_list);
    for (uintptr_t alloc_addr = image_base;
         alloc_addr <= image_base + nt->optional_header.size_of_image;
         alloc_addr += PAGE_SIZE) {
        vmm_free_page((void*)alloc_addr);
    }
    return entry_retcode;
}

bool pe_status_init(pe_status_t* status) {
    status->err_code = 0;
    status->file_name = kmalloc(PE_FILENAME_MAX);
    if (!status->file_name) {
        return false;
    }
    return true;
}

void pe_status_free(pe_status_t* status) {
    status->err_code = 0;
    kfree(status->file_name);
}
