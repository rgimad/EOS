/*
*    EOS - Experimental Operating System
*    Physical memory manager header
*/
#ifndef _PHYS_MEMORY_H_
#define _PHYS_MEMORY_H_

#include <kernel/multiboot.h>
#include <kernel/mm/memlayout.h>
#include <stdint.h>
#include <stdbool.h>


void parse_memory_map(multiboot_memory_map_entry *mmap_addr, uint32_t length);
int phys_find_free_block();
int phys_find_free_blocks(uint32_t count);
physical_addr phys_alloc_block();
void phys_free_block(physical_addr addr);
bool phys_is_block_alloced(physical_addr addr);
physical_addr phys_alloc_blocks(uint32_t count);
void phys_free_blocks(physical_addr addr, uint32_t count);
void phys_allocate_chunk(uint64_t base_addr, uint64_t length);//int
void phys_free_chunk(uint64_t base_addr, uint64_t length);//int
void phys_free_available_memory(struct multiboot_info* mb);

void phys_memory_init(struct multiboot_info* mboot_info);


#endif /* _PHYS_MEMORY_H_ */