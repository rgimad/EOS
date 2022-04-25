/*
 * EOS - Experimental Operating System
 * Physical memory manager header
 */

#ifndef _PHYS_MEMORY_H_
#define _PHYS_MEMORY_H_

#include <kernel/multiboot.h>
#include <kernel/mm/memlayout.h>

#include <stdint.h>
#include <stdbool.h>

void pmm_parse_memory_map(multiboot_memory_map_entry *mmap_addr, uint32_t length);

int pmm_find_free_block();
int pmm_find_free_blocks(uint32_t count);

physical_addr pmm_alloc_block();
void pmm_free_block(physical_addr addr);

bool pmm_is_block_alloced(physical_addr addr);
physical_addr pmm_alloc_blocks(uint32_t count);
void pmm_free_blocks(physical_addr addr, uint32_t count);

void pmm_alloc_chunk(uint64_t base_addr, uint64_t length);
void pmm_free_chunk(uint64_t base_addr, uint64_t length);

void pmm_free_available_memory(struct multiboot_info *mb);
void update_phys_memory_bitmap_addr(physical_addr addr);

void pmm_init(struct multiboot_info *mboot_info);

#endif /* _PHYS_MEMORY_H_ */
