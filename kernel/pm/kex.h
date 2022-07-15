/*
 * EOS - Experimental Operating System
 * KEX executable files support module
 */
#ifndef KEX_H
#define KEX_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include <kernel/tty.h>
#include <kernel/io/qemu_log.h>
#include <kernel/fs/vfs.h>
#include <kernel/mm/kheap.h>

typedef struct {
    uint8_t magic[8];
    uint32_t version;
    uint32_t entry_point;
    uint32_t end_image;
    uint32_t memory;
    uint32_t stack_top;
    uint32_t cmdline;
    uint32_t file_path;
} __attribute__((packed)) kex_hdr_t;

bool kex_hdr_is_valid(const kex_hdr_t *hdr);
void kex_hdr_info(const kex_hdr_t *hdr);
void kex_info(const char *fname);
void kex_run(const char *fname);

#endif // KEX_H