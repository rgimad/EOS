#ifndef _MEMLAYOUT_H_
#define _MEMLAYOUT_H_

#include <stdint.h>

// Constants to the boundaries of memory
#define KERNEL_START_PADDR ((uint32_t)&kernel_phys_start)
#define KERNEL_END_PADDR ((uint32_t)&kernel_phys_end)
#define KERNEL_SIZE (KERNEL_END_PADDR - KERNEL_START_PADDR)

//#define KERNEL_START_VADDR ((uint32_t)&kernel_virt_start)
//#define KERNEL_END_VADDR ((uint32_t)&kernel_virt_end)

#define KERNEL_PHYS_MAP_START kernel_phys_map_start
#define KERNEL_PHYS_MAP_END kernel_phys_map_end
#define KERNEL_PHYS_MAP_SIZE kernel_phys_map_end - kernel_phys_map_start;

// Constants to the Physical Memory Manager
#define PHYS_BLOCKS_PER_BYTE 8
#define PHYS_BLOCK_SIZE 4096

// Functions to
#define ALIGN_BLOCK(addr) (addr) - ((addr) % PHYS_BLOCK_SIZE);

// Addresses of the Kernel in the Physical and Virtual memory
extern uint32_t kernel_phys_start;
extern uint32_t kernel_phys_end;
//extern uint32_t kernel_virt_start;
//extern uint32_t kernel_virt_end;

// Addresses of the Physical Memory Map in the Physical memory
extern uint32_t kernel_phys_map_start;
extern uint32_t kernel_phys_map_end;

typedef uint32_t virtual_addr;
typedef uint32_t physical_addr;

#endif  // _MEMLAYOUT_H_