/*
 * EOS - Experimental Operating System
 * InitRD (initial ramdisk) driver
*/

#include <kernel/fs/initrd.h>
#include <kernel/mm/virt_memory.h>
#include <kernel/mm/kheap.h>
#include <kernel/tty.h>

#include <kernel/libk/string.h>

uint8_t* initrd_begin = 0;
uint8_t* initrd_end = 0;
size_t initrd_size;

int oct2bin(char *str, int size) {
    int n = 0;
    char *c = str;
    while (size-- > 0) {
        n *= 8;
        n += *c - '0';
        c++;
    }
    return n;
}

unsigned int tar_getsize(const char *in) {
    unsigned int size = 0;
    unsigned int j;
    unsigned int count = 1;

    for (j = 11; j > 0; j--, count *= 8) {
        size += ((in[j - 1] - '0') * count);
    }

    return size;
}

// Returns pointer to file data
uint8_t* tar_lookup(uint8_t *archive, const char *filename) {
    uint8_t *ptr = archive;
 
    while (!memcmp(ptr + 257, "ustar", 5)) 
    {
        int filesize = oct2bin((char*)ptr + 0x7c, 11);
        if (!memcmp(ptr, filename, strlen(filename) + 1)) {
            return ptr + 512;
        }

        ptr += (((filesize + 511) / 512) + 1) * 512;
        if (ptr == initrd_end) {
            return NULL;
        }
        if (ptr > initrd_end) {
            return NULL;
        }
    }
    return NULL;
}

uint32_t initrd_read(const char *filename, int offset, int size, vfs_filesystem_t *fs, void *buffer) {
    int read_size = 0;
    (void)fs;
    if (!filename) {
        return 0;
    }
    if (!size) {
        return 0;
    }

    uint8_t *file_addr = tar_lookup(initrd_begin, filename);
    if (file_addr == NULL) { // File not found
        return 0;
    }

    ustar_file_t *file = (ustar_file_t*)(file_addr - 512);

    if (size > oct2bin(file->size, 11)) {
        read_size = oct2bin(file->size, 11);
    } else {
        read_size = size;
    }

    memcpy(buffer, file_addr + offset, size);

    return read_size;
}

uint32_t initrd_file_exists(const char *filename, vfs_filesystem_t *fs) {
    (void)fs;
    if (!filename) {
        return 0;
    }

    uint8_t *file_addr = tar_lookup(initrd_begin, filename);
    if (!file_addr) { // file not found
        return 0;
    } else {
        return 1; // file_addr;
    }
}

uint32_t initrd_get_filesize(const char *filename, vfs_filesystem_t* fs) {
    (void)fs;
    if (!filename) {
        return 0;
    }

    uint8_t *file_addr = tar_lookup(initrd_begin, filename);
    if (!file_addr) { // file not found
        return 0;
    } else {
        file_addr -= 512;
        ustar_file_t *file = (ustar_file_t*)file_addr;
        return oct2bin(file->size, 11);
    }
}

uint32_t initrd_is_dir(const char *filename, vfs_filesystem_t* fs) {
    (void)fs;
    if (!filename) {
        return 0;
    }

    uint8_t *file_addr = tar_lookup(initrd_begin, filename);
    if (!file_addr) { // file not found
        return 0;
    } else {
        ustar_file_t *file = (ustar_file_t*)file_addr;
        return file->type; //(file->type == USTAR_DIRECTORY); // TODO why this comparison doenst work?? why for files file->type is 0 and for dirs id 48 aka '0' ????
    }
}

void initrd_list(int argc, char **arg) {
    (void)argc; (void)arg;
    uint8_t* addr = initrd_begin;

    while (!memcmp(addr + 257, "ustar", 5)) {
        int filesize = oct2bin((char *)addr + 0x7c, 11);
        ustar_file_t *file = (ustar_file_t*)addr;

        if (file->type == USTAR_DIRECTORY) {
            //printf("\n\e[36%s", file->fname);
            tty_printf("\n%s", file->fname);
        } else if (file->type == USTAR_NORMAL_FILE) {
            //printf("\n\e[37%s", file->fname);
            tty_printf("\n%s", file->fname);
        }
        addr += (((filesize + 511) / 512) + 1) * 512;
        
        if (addr == initrd_end) {
            break;
        }
        if (addr > initrd_end ) {
            break;
        }
    }
    tty_printf("\n");
}

void initrd_init(uint32_t phys_begin, uint32_t phys_end) {

    tty_printf("initrd phys begin = %x\ninitrd phys end = %x\n", phys_begin, phys_end);

    //int i;
    //uint32_t v1 = vmm_temp_map_page(phys_begin);
    //for (i = 0; i < 20; i++) tty_printf("%c", *(char*) (phys_begin + i));

    initrd_size = phys_end - phys_begin;
    initrd_begin = (uint8_t*)(kheap_malloc(initrd_size + 4 * PAGE_SIZE));

    uint8_t *frame, *virt;
    for (frame = PAGE_ALIGN_DOWN(phys_begin), virt = PAGE_ALIGN_DOWN((uint32_t)initrd_begin) + PAGE_SIZE;
         frame <= (PAGE_ALIGN_DOWN(phys_end));
         frame += PAGE_SIZE, virt += PAGE_SIZE) {
        vmm_map_page(frame, virt);
    }
    initrd_begin = (uint8_t *)(PAGE_ALIGN_DOWN((uint32_t)initrd_begin) + PAGE_SIZE + phys_begin % PAGE_SIZE);
    initrd_end = initrd_begin + initrd_size;

    //int i;
    //for (i = 0; i < 100; i++) tty_printf("%c", *(char*) (initrd_begin + i));

    //uint32_t v1 = vmm_temp_map_page(phys_begin);
    //for (i = 0; i < 4096; i++) tty_printf("%c", *(char*) (v1 + i));

    vfs_storage_dev_t *dev = kheap_malloc(sizeof(vfs_storage_dev_t));
    dev->type = 4;
    strcpy(dev->name, "initrd");

    vfs_filesystem_handles_t *fs_handles = kheap_malloc(sizeof(vfs_filesystem_handles_t));
    fs_handles->read = &initrd_read;
    fs_handles->exists = &initrd_file_exists;
    fs_handles->get_size = &initrd_get_filesize;
    fs_handles->is_dir = &initrd_is_dir;
    fs_handles->write = 0;
    fs_handles->readdir = 0;
    fs_handles->mkfile = 0;
    fs_handles->mkdir = 0;
    fs_handles->rm = 0;

    vfs_mount(dev, fs_handles, 0, "/initrd/", 0);
}
