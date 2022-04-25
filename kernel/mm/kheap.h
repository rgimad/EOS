/*
 * EOS - Experimental Operating System
 * Kernel heap manager header
 */

#ifndef _KHEAP_H_
#define _KHEAP_H_

#include <kernel/mm/memlayout.h>

#include <libk/types.h>

#include <stdbool.h>
#include <stdint.h>

typedef struct __attribute__((packed)) kheap_item { //9 bytes yes?
    struct kheap_item *next;
    uint32_t size;
    unsigned char used;
} kheap_item;

void kheap_init();

void *kheap_morecore(uint32_t size);
void kheap_free(void *address);
void *kheap_malloc(uint32_t size);

void kheap_print_stat();
void kheap_test();

#endif /* _HEAP_H_ */
