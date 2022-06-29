/*
 * EOS - Experimental Operating System
 * Physical memory manager module
 */

#include <kernel/mm/phys_memory.h>
#include <kernel/multiboot.h>
#include <kernel/tty.h>
#include <kernel/libk/string.h>
#include <kernel/libk/assert.h>

uint32_t *phys_memory_bitmap = 0;
size_t phys_block_count = 0, phys_used_block_count = 0;

uint32_t kernel_phys_map_start;
uint32_t kernel_phys_map_end;

uint64_t phys_installed_memory_size = 0, phys_available_memory_size = 0;

// For initrd
uint64_t initrd_mmap_entry_addr = -1;
uint32_t mmap_avail_entries_count = 0;
multiboot_memory_map_entry_t mmap_avail_entries_array[100];

inline static void bitmap_set(size_t bit) {
    phys_memory_bitmap[bit / 32] |= (1 << (bit % 32));
}

inline static void bitmap_unset(size_t bit) {
    phys_memory_bitmap[bit / 32] &= ~(1 << (bit % 32));
}

inline static bool bitmap_test(size_t bit) {
    return phys_memory_bitmap[bit / 32] & (1 << (bit % 32));
}

void pmm_parse_memory_map(multiboot_memory_map_entry_t *mmap_addr, size_t length) {
    multiboot_memory_map_entry_t *mentry = mmap_addr;	
    // tty_printf("Physical memory map:\n");
    for (size_t i = 0; i < length / sizeof(multiboot_memory_map_entry_t); i++) {
        if ((mentry + i)->type == MULTIBOOT_MEMORY_AVAILABLE) {
            // tty_printf("Available: |");
            phys_available_memory_size += (mentry + i)->len;
        }
        // tty_printf(" addr: %x", (mentry + i)->addr);
        // tty_printf(" length: %x\n", (mentry + i)->len);
        phys_installed_memory_size += (mentry + i)->len;
    }
    /* tty_printf("Installed memory size: %d KB", phys_installed_memory_size / 1024);
    tty_printf(" = %d MB\n", phys_installed_memory_size / (1024 * 1024));

    tty_printf("Available memory size: %d KB", phys_available_memory_size / 1024);
    tty_printf(" = %d MB\n", phys_available_memory_size / (1024 * 1024)); */
}


bool pmm_find_free_block(size_t *block_idx) {
    for (size_t i = 0; i < phys_block_count / 32; i++) {
        uint32_t block = phys_memory_bitmap[i];
        if (block != 0xFFFFFFFF) {
            for (uint8_t j = 0; j < 32; j++) {
                int bit = 1 << j;
                if (!(bit & block)) {
                    *block_idx = (32 * i) + j;
                    return true;
                }
            }
        }
    }
    return false;
}


size_t pmm_find_free_blocks(size_t count, size_t *block_idx) {
    size_t consec_block_count = 0, starting_block = 0, starting_block_bit = 0;
    for (size_t i = 0; i < phys_block_count / 32; i++) {
        uint32_t cur_block = phys_memory_bitmap[i];
        if (cur_block == 0xFFFFFFFF) {
            consec_block_count = 0;
            continue;
        }
        for (uint8_t j = 0; j < 32; j++) {
            int bit = 1 << j;
            if (bit & cur_block) {
                consec_block_count = 0;
                starting_block = i;
                starting_block_bit = j;
                continue;
            }
            if (++consec_block_count == count) {
                *block_idx = (32 * starting_block) + starting_block_bit;
                return true;
            }
        }
    }
    return false;
}


void* pmm_alloc_block() {
    if (phys_block_count <= phys_used_block_count) {
        return NULL;
    }
    size_t free_block;
    if (!pmm_find_free_block(&free_block)) {
        return NULL;
    }
    bitmap_set(free_block);
    phys_used_block_count++;
    return (void*)(free_block * PHYS_BLOCK_SIZE);
}


void pmm_free_block(void *addr) {
    size_t block = (uintptr_t)addr / PHYS_BLOCK_SIZE;
    bitmap_unset(block);
    phys_used_block_count--;
}


bool pmm_is_block_alloced(void *addr) {
    size_t block = (uintptr_t)addr / PHYS_BLOCK_SIZE;
    return bitmap_test(block);
}


void* pmm_alloc_blocks(size_t count) {
    if (phys_block_count <= phys_used_block_count) {
        return NULL;
    }
    size_t free_block;
    if (!pmm_find_free_blocks(count, &free_block)) {
        return NULL;
    }
    for (size_t i = 0; i < count; i++) {
        bitmap_set(free_block + i);
    }
    phys_used_block_count += count;
    return (void*)(free_block * PHYS_BLOCK_SIZE);
}


void pmm_free_blocks(void *addr, size_t count) {
    size_t block = (uintptr_t)addr / PHYS_BLOCK_SIZE;
    for (size_t i = 0; i < count; i++) {
        bitmap_unset(block + i);
    }
    phys_used_block_count -= count;
}

static void pmm_mark_range_as(void *base, size_t length, int usedness) {
    size_t block_idx = (uintptr_t)base / PHYS_BLOCK_SIZE,
        num_blocks = length / PHYS_BLOCK_SIZE;
    while (num_blocks--) {
        if (usedness == PHYS_BLOCK_USED) {
            if (!bitmap_test(block_idx)) {
                bitmap_set(block_idx);
                phys_used_block_count++;
            }
        } else {
            if (bitmap_test(block_idx)) {
                bitmap_unset(block_idx);
                phys_used_block_count--;
            }
        }
        block_idx++;
    }
}


void pmm_free_available_memory(multiboot_info_t *mb) {
    __auto_type mm = (multiboot_memory_map_entry_t*)mb->mmap_addr;
    while ((uintptr_t)mm < mb->mmap_addr + mb->mmap_length) {
        if (mm->type == MULTIBOOT_MEMORY_AVAILABLE) {
            if (mm->addr != initrd_mmap_entry_addr) { // ADDED BECAUSE INITRD RELOCATION
                pmm_mark_range_as((void*)(mm->addr), mm->len, PHYS_BLOCK_FREE);
            } else {
                uintptr_t initrd_begin = *(uint32_t*) (mb->mods_addr),
                    initrd_end = *(uint32_t*) (mb->mods_addr + 4);
                size_t initrd_size = initrd_end - initrd_begin;
                pmm_mark_range_as((void*)initrd_begin, initrd_size, PHYS_BLOCK_USED);
                pmm_mark_range_as((void*)(mm->addr), mm->len - initrd_size - 2, PHYS_BLOCK_FREE); // Why -2????
            }
        }
        mm = (multiboot_memory_map_entry_t*) ((uintptr_t)mm + mm->size + sizeof(mm->size));
    }
    bitmap_set(0);
}

void pmm_relocate_initrd_to_high_mem(multiboot_info_t *mb) {
    uintptr_t initrd_begin = *(uint32_t*) (mb->mods_addr),
        initrd_end = *(uint32_t*) (mb->mods_addr + 4);
    size_t initrd_size = initrd_end - initrd_begin;

    __auto_type mm = (multiboot_memory_map_entry_t*)mb->mmap_addr;
    mmap_avail_entries_count = 0;

    while ((uintptr_t)mm < mb->mmap_addr + mb->mmap_length) {
        if (mm->type == MULTIBOOT_MEMORY_AVAILABLE) { 
            mmap_avail_entries_array[mmap_avail_entries_count] = *mm;
            mmap_avail_entries_count++;
        }
        mm = (multiboot_memory_map_entry_t*) ((uintptr_t)mm + mm->size + sizeof(mm->size));
    }

    //tty_printf("mmap_avail_entries_count = %x \n\n", mmap_avail_entries_count);
    for (size_t i = mmap_avail_entries_count - 1; i >= 0; i--) {
        //tty_printf("addr = %x  | len = %x \n", mmap_avail_entries_array[i].addr, mmap_avail_entries_array[i].len);
        if (mmap_avail_entries_array[i].len >= initrd_size) {
            //tty_printf("addr = %x\n", mmap_avail_entries_array[i].addr);
            initrd_mmap_entry_addr = mmap_avail_entries_array[i].addr;
            // NOTE: here assuming that address is 32 bit, when porting to 64bit platform need change
            memcpy((void*)((uint32_t)initrd_mmap_entry_addr + (uint32_t)mmap_avail_entries_array[i].len - initrd_size - 1), (void*)initrd_begin, initrd_size);
            initrd_begin = initrd_mmap_entry_addr + mmap_avail_entries_array[i].len - initrd_size - 1;
            initrd_end = initrd_begin + initrd_size;
            //pmm_free_chunk(initrd_end + 1, mmap_avail_entries_array[i].len - initrd_size);
            break;
        }
    }

    *(uint32_t*) (mb->mods_addr) = initrd_begin;
    *(uint32_t*) (mb->mods_addr + 4) = initrd_end;
    //tty_printf("initrd_begin = %x, initrd_end = %x", initrd_begin, initrd_end);
}


/*
    Initialization of Physical Memory Manager
*/
void pmm_init(multiboot_info_t *mboot_info) {
    __auto_type mmap = (multiboot_memory_map_entry_t*)mboot_info->mmap_addr;
    pmm_parse_memory_map(mmap, mboot_info->mmap_length); // It also calculates the phys_installed_memory_size

    pmm_relocate_initrd_to_high_mem(mboot_info);

    phys_block_count = (phys_installed_memory_size /** 1024*/) / PHYS_BLOCK_SIZE; // How many blocks will be
    phys_used_block_count = phys_block_count; // Initially all blocks are used
    phys_memory_bitmap = (uint32_t*)KERNEL_END_PADDR; // Physical memory bitmap starts after kernel
    memset(phys_memory_bitmap, 0xFF, phys_block_count / PHYS_BLOCKS_PER_BYTE); // Initially we mark all installed memory as used
    
    //tty_printf("Total blocks: %d\n", phys_block_count);

    // Frees memory GRUB considers available
    pmm_free_available_memory(mboot_info);

    // From the freed memory, we need to allocate the ones used by the Kernel
    pmm_mark_range_as((void*)KERNEL_START_PADDR, KERNEL_SIZE, PHYS_BLOCK_USED);

    //tty_printf("KERNEL_START_PADDR = %x, KERNEL_END_PADDR = %x, KERNEL_SIZE = %d bytes ", KERNEL_START_PADDR, KERNEL_END_PADDR, KERNEL_SIZE);
    //tty_printf("MemMap addr = %x\n", mboot_info->mmap_addr);
    
    // We also need to allocate the memory used by the Physical Map itself
    pmm_mark_range_as(phys_memory_bitmap, phys_block_count, PHYS_BLOCK_USED); // WHY first argument *phys_memory_bitmap ???????????????????????????????????????????????????
    kernel_phys_map_start = (uint32_t) phys_memory_bitmap;
    kernel_phys_map_end = kernel_phys_map_start + (phys_block_count / PHYS_BLOCKS_PER_BYTE);

    //tty_printf("Physical memory manager initialized. Physical memory bitmap start: %x, end: %x, size = %d bytes\n", kernel_phys_map_start, kernel_phys_map_end, kernel_phys_map_end - kernel_phys_map_start);
}

void pmm_test() {
    tty_printf("TEST: ");
    void *myptr= pmm_alloc_block();
    tty_printf("myptr = %x,  ", myptr);

    char *str = "Hello world!";
    memcpy(myptr, str, 13);

    char buf[25];
    memcpy(buf, myptr, 13);
    tty_printf("read from memory str = %s\n", buf);
}

void update_phys_memory_bitmap_addr(void *addr) {
    phys_memory_bitmap = (uint32_t*)addr;
}
