/*
 * EOS - Experimental Operating System
 * User heap manager module header
 */

#ifndef _UHEAP_H_
#define _UHEAP_H_

#include <kernel/mm/memlayout.h>
#include <kernel/pm/process.h>
#include <kernel/pm/thread.h>

#include <libk/types.h>

#include <stdbool.h>
#include <stdint.h>

// User heap item structure
typedef struct __attribute__((packed)) uheap_item { //9 bytes yes?
    struct uheap_item *next;
    uint32_t size;
    unsigned char used;
} uheap_item;

void uheap_init(process_t *proc); // Initialize heap for process
void *uheap_morecore(process_t *proc, uint32_t size);
void uheap_free(process_t *proc, void *address);
void *uheap_malloc(process_t *proc, uint32_t size);

#endif
