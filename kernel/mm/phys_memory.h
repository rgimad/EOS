/*
 * EOS - Experimental Operating System
 * Physical memory manager header
 */

#ifndef _PHYS_MEMORY_H_
#define _PHYS_MEMORY_H_

#include <kernel/multiboot.h>
#include <kernel/mm/memlayout.h>

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define PHYS_BLOCK_USED 1
#define PHYS_BLOCK_FREE 0

void pmm_parse_memory_map(multiboot_memory_map_entry_t *mmap_addr, size_t length);
bool pmm_find_free_block(size_t *block_idx);
size_t pmm_find_free_blocks(size_t count, size_t *block_idx);
void *pmm_alloc_block();
void pmm_free_block(void *addr);
bool pmm_is_block_alloced(void *addr);
void *pmm_alloc_blocks(size_t count);
void pmm_free_blocks(void *addr, size_t count);
void pmm_free_available_memory(multiboot_info_t *mb);
void pmm_relocate_initrd_to_high_mem(multiboot_info_t *mb);
void update_phys_memory_bitmap_addr(void *addr);
void pmm_init(multiboot_info_t *mboot_info);

#endif /* _PHYS_MEMORY_H_ */
