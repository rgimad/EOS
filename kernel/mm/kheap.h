/*
 * EOS - Experimental Operating System
 * Kernel heap manager header
 */

#ifndef _KHEAP_H_
#define _KHEAP_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <kernel/mm/memlayout.h>
#include <kernel/libk/string.h>
#include <kernel/libk/assert.h>

#include <kernel/mm/virt_memory.h>
#include <kernel/tty.h>
#include <kernel/io/qemu_log.h>

typedef struct kheap_item_t {
    struct kheap_item_t *next;
    size_t size;
    uint8_t used; // 0 - free, 1 - not free
} __attribute__((packed)) kheap_item_t;

extern kheap_item_t *kheap_begin, *kheap_end; // pointers to current heap bounds
extern uint8_t *kheap_limit;                  // maximal address of heap_end
// how much memory was used and how many allocations were
extern size_t kheap_memory_used, kheap_allocs_num;

void kheap_init(void);
size_t kheap_increase(size_t size);
void kheap_merge_free_adjacent(void);
void kfree(void *address);
void *kmalloc(size_t size);
void *krealloc(void *p, size_t newsz);
void kheap_print_stat(void);
void kheap_test(void);

#endif /* _HEAP_H_ */
