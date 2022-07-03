/*
 * EOS - Experimental Operating System
 * Kernel heap manager module
 */

#include <kernel/mm/kheap.h>

kheap_item_t *kheap_begin, *kheap_end; // pointers to current heap bounds
// uint8_t *kheap_limit; // maximal address of kheap_end
// how much memory was used and how many allocations were
size_t kheap_memory_used, kheap_allocs_num;

void kheap_init(void) {
    // qemu_printf("kheap_init\n");
    kheap_begin = (kheap_item_t*)KHEAP_START_VADDR;
    kheap_end = NULL;
    // kheap_limit = _kheap_limit;

    kheap_allocs_num = 0;
    kheap_memory_used = 0;
    // qemu_printf("kheap_init end\n");
}

// increase kernel heap by some amount, this will be rounded up by the page size 
// return by how many bytes heap was increased
size_t kheap_increase(size_t size) {
    // if (kheap_end) {
        // printf("\nkheap_end = %p, size = %u, kheap_limit = %p\n", kheap_end, size, kheap_limit);
        // assert((uint8_t*)kheap_end + size < kheap_limit);
    // }
    // Calculate how many pages we will need
    size_t pages = (size / PAGE_SIZE) + (size % PAGE_SIZE != 0);
    // When kheap_end == NULL we must create the initial heap
    if (kheap_end == NULL) {
        kheap_end = kheap_begin;
    }

    // Create the pages
    // qemu_printf("pages = %u\n", pages);
    for (size_t i = 0; i < pages; i++) {

        vmm_alloc_page(kheap_end);
        memset((void*)kheap_end, 0, PAGE_SIZE);
        kheap_end = (kheap_item_t*)((uintptr_t)kheap_end + PAGE_SIZE);
    }
    return pages*PAGE_SIZE;
}

void kheap_merge_free_adjacent(void) {
    // merge any adjacent free items
    for (kheap_item_t* item = (kheap_item_t*)kheap_begin; item != NULL; item = item->next) {
        while (!item->used && item->next != NULL && !item->next->used) {
            item->size += sizeof(kheap_item_t) + item->next->size;
            item->next = item->next->next;
        }
    }
}

// Free a previously allocated item from the kernel heap
void kfree(void* address) {
    qemu_printf("kfree(%x)\n", address);
    if (address == NULL) {
        return;
    }
    for (kheap_item_t* item = kheap_begin; item != NULL; item = item->next) {
        if (item == (kheap_item_t*)address - 1) {
            item->used = 0;
            kheap_memory_used -= item->size;
            kheap_allocs_num--;
            kheap_merge_free_adjacent();
            break;
        }
    }
}

// Allocates an arbitrary size of memory
void* kmalloc(size_t size) {
    qemu_printf("kmalloc(%u)\n", size);
    if (size == 0) {
        return NULL;
    }
    kheap_item_t *new_item = NULL, *tmp_item;
    // add header size:
    size_t total_size = size + sizeof(kheap_item_t);
    // printf("total_size = %u\n", total_size);

    // last_item - pointer to last element in the list
    kheap_item_t* last_item = kheap_begin;
    if (kheap_end != NULL) { // if the heap exists
        // search for first fit:
        for (new_item = kheap_begin; new_item != NULL; new_item = new_item->next) {
            if (new_item->next == NULL) {
                last_item = new_item;
            }

            if (!new_item->used && (total_size <= new_item->size)) {
                break;
            }
        }
    }
    
    if (new_item != NULL) { // if we found suitable item
        // if it has more free bytes than we need
        if (new_item->size - total_size > 0) {
            // allocate the remaining memory into its own item
            tmp_item = (kheap_item_t*)((uintptr_t)new_item + total_size);
            tmp_item->size = new_item->size - total_size;
            tmp_item->used = 0;
            tmp_item->next = new_item->next;
            new_item->next = tmp_item;
        }
    } else {
        // place new item in the end and then increase heap
        new_item = (kheap_end == NULL) ? kheap_begin : kheap_end;
        // printf(">> new_item = %p\n", new_item);
        // Didn't find a fit or heap is empty so we must increase the heap to fit
        size_t bytes_added = kheap_increase(total_size);

        // Create an empty item for the extra space kheap_increase() gave us
        // We can calculate the size because kheap_increase() allocates space that is page aligned
        // printf("bytes_added = %u, total_size = %u\n", bytes_added, total_size);
        if (bytes_added - total_size > sizeof(kheap_item_t) ) { // if we have extra space to place least item
            tmp_item = (kheap_item_t*)((uintptr_t)new_item + total_size);
            tmp_item->size = bytes_added - total_size - sizeof(kheap_item_t);
            tmp_item->used = 0;
            tmp_item->next = NULL;
            new_item->next = tmp_item;
        } else {
            new_item->next = NULL;
        }
        if (new_item != kheap_begin) { // avoid cycling in new_item == kheap_begin case
            last_item->next = new_item;
        }
    }
    new_item->size = size;
    new_item->used = 1;
    
    kheap_allocs_num++;
    kheap_memory_used += total_size;

    // Return the newly allocated memory location
    // qemu_printf("kmalloc_end\n");
    return (void*)(new_item + 1);
}

void kheap_print_stat(void) {
    tty_printf("\nallocs number = %u", kheap_allocs_num);
    tty_printf("\nmemory used = %u bytes\n", kheap_memory_used);
    
    for (kheap_item_t *item = kheap_begin; item != NULL; item = item->next) {
        tty_printf("  size: %u, used: %u\n", item->size, item->used);
    }
    tty_printf("\n");
}


void* krealloc(void *p, size_t newsz) {
    qemu_printf("krealloc(%x, %u)\n", p, newsz);
    void *p2 = kmalloc(newsz);
    memcpy(p2, p, *((uint8_t*)p - 1 - 4));
    kfree(p);
    return p2;
}


void kheap_test(void) {
    uint32_t sz = 4096 - sizeof(kheap_item_t); // 1024 * 768 * 4;
    uint8_t* mas = (uint8_t*)kmalloc(sz);

    memset(mas, 5, sz);
    tty_printf("mas[sz-1] = %d\n", mas[sz - 1]);
    tty_printf("mas_addr = %p\n", mas);

    kheap_print_stat();

    int cnt = 12;
    int* arr = (int*)kmalloc(cnt * sizeof(int));

    int i;
    for (i = 0; i < cnt; i++) {
        arr[i] = i * 2;
    }

    // for (i = 0; i < cnt; i++) {
    //     tty_printf("%d ", arr[i]);
    // }

    tty_printf("arr = %p", arr);
    kheap_print_stat();

    //heap_free(arr);
    //tty_printf("\narr[0] = %d ", arr[1]);
    //heap_print_stat();

    int* arr2 = (int*)kmalloc(24 * sizeof(int));
    for (i = 0; i < 24; i++) {
        arr2[i] = i * 3;
    }

    tty_printf("\n");

    /*
    for (i = 0; i < 24; i++) {
        tty_printf("%d ", arr2[i]);
    }
    */

    tty_printf("arr2 = %p", arr2);
    kheap_print_stat();
    kfree(arr2);
    kheap_print_stat();

    char* arr3 = (char*)kmalloc(5 * sizeof(char));
    tty_printf("arr3 = %p", arr3);

    int* arr4 = (int*)kmalloc(8200 * sizeof(int));
    for (i = 0; i < 8200; i++) {
        arr4[i] = i * 2;
    }

    tty_printf("\n");

    /*
    for (i = 0; i < 8200; i++) {
        tty_printf("%d ", arr4[i]);
    }
    */

    tty_printf("(arr4) = %p\n", arr4);
    tty_printf("(arr4-hdr) = %lx   heap_end = %p\n", (uintptr_t)arr4 - (uintptr_t)sizeof(kheap_item_t), kheap_end);

    kheap_print_stat();
    kfree(arr4);
    kheap_print_stat();
}
