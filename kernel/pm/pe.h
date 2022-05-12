#ifndef PE_H
#define PE_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <kernel/pm/elf.h>

#define PE_IMAGE_DOS_SIGNATURE              0x5A4D      /* MZ in ASCII */
#define PE_IMAGE_NT_SIGNATURE               0x00004550  /* PE in ASCII */
#define PE_IMAGE_NT_OPTIONAL_HDR32_MAGIC    0x10b
#define PE_IMAGE_FILE_MACHINE_I386          0x014c      /* Intel 386 or later processors and compatible processors */
#define PE_IMAGE_SIZEOF_SHORT_NAME          8
#define PE_IMAGE_NUMBEROF_DIRECTORY_ENTRIES 16

#define PE_MAX_SECTIONS 96

#define PE_MAKE_PTR(cast, ptr, add_value) (cast)((uintptr_t)(ptr) + (uint32_t)(add_value))

#define PE_IMAGE_RELOC_DIRECTORY 5

enum PE_IMAGE_REL_BASED_TYPES {
    PE_IMAGE_REL_BASED_ABSOLUTE = 0,
    PE_IMAGE_REL_BASED_HIGH = 1,
    PE_IMAGE_REL_BASED_LOW = 2,
    PE_IMAGE_REL_BASED_HIGHLOW = 3,
};

#pragma pack(push,2)
typedef struct pe_image_dos_header_s {
    uint16_t    e_magic;
    uint16_t    e_cblp;
    uint16_t    e_cp;
    uint16_t    e_crlc;
    uint16_t    e_cparhdr;
    uint16_t    e_minalloc;
    uint16_t    e_maxalloc;
    uint16_t    e_ss;
    uint16_t    e_sp;
    uint16_t    e_csum;
    uint16_t    e_ip;
    uint16_t    e_cs;
    uint16_t    e_lfarlc;
    uint16_t    e_ovno;
    uint16_t    e_res[4];
    uint16_t    e_oemid;
    uint16_t    e_oeminfo;
    uint16_t    e_res2[10];
    uint32_t    e_lfanew;
} pe_image_dos_header_t, *pe_pimage_dos_header_t;
#pragma pack(pop)


#pragma pack(push,4)
typedef struct pe_image_file_header_s {
    uint16_t   machine;
    uint16_t   number_of_sections;
    uint32_t   time_date_stamp;
    uint32_t   pointer_to_symbol_table;
    uint32_t   number_of_symbols;
    uint16_t   size_of_optional_header;
    uint16_t   characteristics;
} pe_image_file_header_t, *pe_pimage_file_header_t;


typedef struct pe_image_data_directory_s {
    uint32_t   virtual_address;
    uint32_t   size;
} pe_image_data_directory_t, *pe_pimage_data_directory_t;

typedef struct pe_image_optional_header_s{
    uint16_t    magic;
    uint8_t     major_linker_version;
    uint8_t     minor_linker_version;
    uint32_t    size_of_code;
    uint32_t    size_of_initialized_data;
    uint32_t    size_of_uninitialized_data;
    uint32_t    address_of_entry_point;
    uint32_t    base_of_code;
    uint32_t    base_of_data;
    uint32_t    image_base;
    uint32_t    section_alignment;
    uint32_t    file_alignment;
    uint16_t    major_os_version;
    uint16_t    minor_os_version;
    uint16_t    major_image_version;
    uint16_t    minor_image_version;
    uint16_t    major_subsystem_version;
    uint16_t    minor_subsystem_version;
    uint32_t    win32_version_value;
    uint32_t    size_of_image;
    uint32_t    size_of_headers;
    uint32_t    checksum;
    uint16_t    subsystem;
    uint16_t    dll_characteristics;
    uint32_t    size_of_stack_reserve;
    uint32_t    size_of_stack_commit;
    uint32_t    size_of_heap_reserve;
    uint32_t    size_of_heap_commit;
    uint32_t    loader_flags;
    uint32_t    number_of_rva_and_sizes;
    pe_image_data_directory_t data_directory[PE_IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
} pe_image_optional_header_t, *pe_pimage_optional_header_t;
#pragma pack(pop)

#pragma pack(push,4)
typedef struct pe_image_nt_headers32_s {
    uint32_t                    signature;
    pe_image_file_header_t      file_header;
    pe_image_optional_header_t  optional_header;
} pe_image_nt_headers32_t, *pe_pimage_nt_headers32_t;

typedef struct pe_image_section_header_s {
    uint8_t name[PE_IMAGE_SIZEOF_SHORT_NAME];
    union {
        uint32_t physical_address;
        uint32_t virtual_size;
    } misc;
    uint32_t    virtual_address;
    uint32_t    size_of_raw_data;
    uint32_t    pointer_to_raw_data;
    uint32_t    pointer_to_relocations;
    uint32_t    pointer_to_linenumbers;
    uint16_t    number_of_relocations;
    uint16_t    number_of_linenumbers;
    uint32_t    characteristics;
} pe_image_section_header_t, *pe_pimage_section_header_t;
#pragma pack(pop)


typedef struct pe_image_base_relocation_s {
    uint32_t    virtual_address;
    uint32_t    size_of_block;
} pe_image_base_relocation_t, *pe_pimage_base_relocation_t;

int run_pe_file(const char *name);

#endif