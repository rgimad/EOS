#include <kernel/pm/pe.h>
#include <kernel/fs/vfs.h>
#include <kernel/mm/virt_memory.h>
#include <kernel/mm/kheap.h>
#include <kernel/tty.h>

#include <kernel/libk/string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

static inline bool is_powerof2(uint32_t val) {
    if (val == 0)
        return false;
    return (val & (val - 1)) == 0;
}

bool pe_validate(uintptr_t addr, size_t size) {
    pe_pimage_dos_header_t     dos;
    pe_pimage_nt_headers32_t   nt;

    dos = (pe_pimage_dos_header_t)addr;

    if (!addr || size < sizeof(pe_image_dos_header_t))
        return false;

    if (dos->e_magic != PE_IMAGE_DOS_SIGNATURE || dos->e_lfanew <= 0)
        return false;
   
    nt = PE_MAKE_PTR(pe_pimage_nt_headers32_t, dos, dos->e_lfanew);

    if ((uintptr_t)nt < (uintptr_t)addr)
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

bool pe_create_image(uintptr_t img_base, uintptr_t raw) {
    pe_pimage_dos_header_t     dos;
    pe_pimage_nt_headers32_t   nt;
    pe_pimage_section_header_t img_sec;

    uint32_t  sec_align;

    dos = (pe_pimage_dos_header_t)raw;
    nt = PE_MAKE_PTR(pe_pimage_nt_headers32_t , dos, dos->e_lfanew);

    memcpy((void*)img_base, (void*)raw, nt->optional_header.size_of_headers);

    img_sec = PE_MAKE_PTR(pe_pimage_section_header_t, nt, sizeof(pe_image_nt_headers32_t));
    sec_align = nt->optional_header.section_alignment;

    for (uint16_t i = 0; i < nt->file_header.number_of_sections; i++) {
        uintptr_t src_ptr = PE_MAKE_PTR(uintptr_t, raw, img_sec->pointer_to_raw_data);
        uintptr_t dest_ptr = PE_MAKE_PTR(uintptr_t, img_base, img_sec->virtual_address);

        if (img_sec->size_of_raw_data) {
            memcpy((void*)dest_ptr, (void*)src_ptr, img_sec->size_of_raw_data);   
        }

        size_t sec_size = (img_sec->misc.virtual_size + sec_align - 1) & (-sec_align);

        if (sec_size > img_sec->size_of_raw_data) {
            memset((void*)(dest_ptr + img_sec->size_of_raw_data), 0, sec_size - img_sec->size_of_raw_data);
        }
        img_sec++;
    }

    pe_image_data_directory_t reloc_dir = nt->optional_header.data_directory[PE_IMAGE_RELOC_DIRECTORY];

    if (img_base != nt->optional_header.image_base) {
        if (reloc_dir.size) {
            uint32_t delta = img_base - nt->optional_header.image_base;
            pe_pimage_base_relocation_t reloc = PE_MAKE_PTR(pe_pimage_base_relocation_t, img_base, reloc_dir.virtual_address);

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
                            p16 = PE_MAKE_PTR(uint16_t*, img_base, offs);
                            *p16+= (uint16_t)(delta >> 16);
                            break;
                        case PE_IMAGE_REL_BASED_LOW:
                            p16 = PE_MAKE_PTR(uint16_t*, img_base, offs);
                            *p16+= (uint16_t)delta;
                            break;
                        case PE_IMAGE_REL_BASED_HIGHLOW:
                            p32 = PE_MAKE_PTR(uint32_t*, img_base, offs);
                            *p32+= delta;
                    }
                    entry++;
                }
                reloc = PE_MAKE_PTR(pe_pimage_base_relocation_t, reloc, reloc->size_of_block);
            }
        } else {
            tty_printf("PE image base relocation error! Section '.reloc' not found!\n");
            return false;
        }
    }

    tty_printf("Create PE base %x (preferred PE base %x), size %x, %d sections\n",
                img_base, nt->optional_header.image_base, nt->optional_header.size_of_image, nt->file_header.number_of_sections);
    return true;
}


void *pe_open(const char *fname) { // Returns pointer to PE file.
    size_t fsize = vfs_get_size(fname);
    void *addr = kheap_malloc(fsize);
    int res = vfs_read(fname, 0, fsize, addr);
    if (!pe_validate((uintptr_t)addr, fsize)){
        tty_printf("Bad PE file!\n");
        return NULL;
    }
    return addr;
}

#define TEST_PE_BASE_RELOC

int run_pe_file(const char *name) {
    void *pe_file = pe_open(name);
    if (!pe_file) {
        return -1;
    }
    pe_pimage_dos_header_t dos = (pe_pimage_dos_header_t)pe_file;
    pe_pimage_nt_headers32_t nt = PE_MAKE_PTR(pe_pimage_nt_headers32_t, dos, dos->e_lfanew);

#ifdef TEST_PE_BASE_RELOC
    uintptr_t image_base = 0;
#else
    uintptr_t image_base = nt->optional_header.image_base;
#endif

    uint32_t alloc_addr;
    for (alloc_addr = image_base;
         alloc_addr <= image_base + nt->optional_header.size_of_image;
         alloc_addr += PAGE_SIZE) {
        vmm_alloc_page(alloc_addr);
    }

    if (!pe_create_image(image_base, (uintptr_t)pe_file)) {
        return -1;
    }
    int (*entry_point)(void) = (int(*)(void))image_base + nt->optional_header.address_of_entry_point;
    tty_printf("PE entry point: %x\n", entry_point);

    int ret_code = entry_point();

    for (alloc_addr = image_base;
         alloc_addr <= image_base + nt->optional_header.size_of_image;
         alloc_addr += PAGE_SIZE) {
        vmm_free_page(alloc_addr);
    }
    return ret_code;
}