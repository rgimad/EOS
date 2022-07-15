/*
 * EOS - Experimental Operating System
 * KEX executable files support module
 */

#include <kernel/pm/kex.h>

bool kex_hdr_is_valid(const kex_hdr_t *hdr)
{
    return hdr->magic[0] == 'M' && hdr->magic[1] == 'E'
        && hdr->magic[2] == 'N' && hdr->magic[3] == 'U'
        && hdr->magic[4] == 'E' && hdr->magic[5] == 'T'
        && hdr->magic[6] == '0' && hdr->magic[7] == '1';
}

void kex_hdr_info(const kex_hdr_t *hdr)
{
    if (!kex_hdr_is_valid(hdr)) {
        tty_printf("incorrect kex header\n");
        return;
    }
    tty_printf("\n    magic = %c%c%c%c%c%c%c%c\n    version = %x\n"
               "    entry_point = %x\n    end_image = %x\n"
               "    memory = %u bytes\n    stack_top = %x\n"
               "    cmdline = %x\n    file_path = %x\n",
               hdr->magic[0], hdr->magic[1], hdr->magic[2], hdr->magic[3],
               hdr->magic[4], hdr->magic[5], hdr->magic[6], hdr->magic[7],
               hdr->version, hdr->entry_point, hdr->end_image, hdr->memory,
               hdr->stack_top, hdr->cmdline, hdr->file_path);
}

void kex_info(const char *fname)
{
    if (!vfs_exists(fname)) {
        tty_printf("kex doesnt exist\n");
        return;
    }
    size_t fsize = vfs_get_size(fname);
    tty_printf("file size = %d\n", fsize);
    void *buf = kmalloc(fsize);
    int res = vfs_read(fname, 0, fsize, buf);
    (void)res;
    kex_hdr_info((kex_hdr_t *)buf);
    kfree(buf);
}

void kex_run(const char *fname)
{
    if (!vfs_exists(fname)) {
        tty_printf("kex doesnt exist\n");
        return;
    }
    size_t fsize = vfs_get_size(fname);
    tty_printf("file size = %d\n", fsize);
    kex_hdr_t *kexfile = kmalloc(fsize);
    int res = vfs_read(fname, 0, fsize, kexfile);
    (void)res;
    if (!kex_hdr_is_valid(kexfile)) {
        tty_printf("incorrect kex header\n");
        goto exit;
    }
    for (uint8_t *alloc_addr = NULL;
         alloc_addr < (uint8_t *)kexfile->memory; alloc_addr += PAGE_SIZE) {
        vmm_alloc_page(alloc_addr);
    }
    memcpy(NULL, kexfile, kexfile->memory);
    void (*entry_point)() = (void (*)())kexfile->entry_point;
    entry_point();
    for (uint8_t *alloc_addr = NULL;
         alloc_addr < (uint8_t *)kexfile->memory; alloc_addr += PAGE_SIZE) {
        vmm_free_page(alloc_addr);
    }
exit:
    kfree(kexfile);
}
