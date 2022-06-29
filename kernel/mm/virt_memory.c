/*
 * EOS - Experimental Operating System
 * Virtual memory manager module
 */

#include <kernel/mm/virt_memory.h>
#include <kernel/mm/phys_memory.h>
#include <kernel/tty.h>

#include <kernel/libk/string.h>

page_directory *kernel_page_dir; // Pointer (physical) to kernel page dircetory structure

bool vmm_alloc_page(void *vaddr) {
    void *paddr = pmm_alloc_block();
    if (!paddr) {
        return false;
    }
    vmm_map_page(paddr, vaddr);
    return true;
}

bool vmm_alloc_page_with_userbit(void *vaddr) {
    void *paddr = pmm_alloc_block();
    if (!paddr) {
        return false;
    }
    vmm_map_page(paddr, vaddr);
    page_table_entry *pte = (void*)GET_PTE((uintptr_t)vaddr);
    page_table_entry_add_attrib(pte, I86_PTE_USER);
    return true;
}

void vmm_free_page(void *vaddr) {
    page_table_entry *pte = (void*)GET_PTE((uintptr_t)vaddr);
    if (!page_table_entry_is_present(*pte)) {
        tty_printf("oh, you try to delete not present page\n");
        // TODO: panic
        return;
    }
    void *block = page_table_entry_frame(*pte);
    if (block) {
        pmm_free_block(block);
    }
    page_table_entry_del_attrib(pte, I86_PTE_PRESENT);
}

void vmm_create_kernel_page_dir() {
    kernel_page_dir = (page_directory*)pmm_alloc_block();
    if (!kernel_page_dir) {
        tty_printf("Failed to allocate phys memory for kernel page dir\n");
        // TODO: panic
        return;
    }

    page_directory *pd = kernel_page_dir;
    memset(pd, 0, sizeof(page_directory));

    for (size_t i = 0; i < PAGE_ENTRIES; i++) {
        page_dir_entry *pde = (page_dir_entry*) &pd->entries[i];
        page_dir_entry_add_attrib(pde, I86_PTE_WRITABLE);
        page_dir_entry_del_attrib(pde, I86_PTE_PRESENT);

        if (i == PAGE_ENTRIES - 1) { // Fractal(recursive) mapping technique, which allows us to access PD and PT
            page_dir_entry_add_attrib(pde, I86_PTE_PRESENT);
            page_dir_entry_set_frame(pde, kernel_page_dir);

            //tty_printf("pd[1023] = %x\n", pd->entries[1023]);
        }
    }
}

void vmm_map_page(void *paddr, void *vaddr) {
    page_dir_entry *pde = (void*)GET_PDE((uintptr_t)vaddr);
    if (!page_dir_entry_is_present(*pde)) { // If page table isnt present, create it
        void *pt_p = pmm_alloc_block(); // It's phys addr!
        if (!pt_p) {
            tty_printf("wtf? no free phys memory\n");
            // TODO: panic
            return;
        }

        page_table *pt_v = (page_table*) vmm_temp_map_page(pt_p); // Because we need to write!
        memset(pt_v, 0, sizeof(page_table));
        page_dir_entry_add_attrib(pde, I86_PDE_PRESENT);
        page_dir_entry_add_attrib(pde, I86_PDE_WRITABLE);
        page_dir_entry_set_frame(pde, pt_p);
    }

    page_table_entry *pte = (void*)GET_PTE((uintptr_t)vaddr);
    page_table_entry_set_frame(pte, paddr);
    page_table_entry_add_attrib(pte, I86_PTE_PRESENT);
    page_table_entry_add_attrib(pte, I86_PTE_WRITABLE);
    flush_tlb_entry(vaddr);
}

void *vmm_temp_map_page(void *paddr) {
    page_table_entry *pte = (void*)GET_PTE((uintptr_t)TEMP_PAGE_ADDR);
    page_table_entry_set_frame(pte, (void*)PAGE_ALIGN_DOWN((uintptr_t)paddr));
    page_table_entry_add_attrib(pte, I86_PTE_PRESENT);
    page_table_entry_add_attrib(pte, I86_PTE_WRITABLE);

    //flush_tlb_entry(TEMP_PAGE_ADDR);
    asm volatile("invlpg %0" :: "m" (*(uint32_t *) TEMP_PAGE_ADDR) : "memory" );
    //flush_tlb_all();

    return (void*)TEMP_PAGE_ADDR;
}

// Switch page directory, reveives physical address
void vmm_switch_page_directory(page_directory *page_dir_phys_addr) {
    asm volatile("mov %0, %%cr3" :: "r"((uint32_t) page_dir_phys_addr));
}

void vmm_init() {
    //tty_printf("1\n");

    vmm_create_kernel_page_dir();

    page_table *table1 = (page_table*) pmm_alloc_block();
    page_table *table2 = (page_table*) pmm_alloc_block();

    // Clear allocated page tables
    memset((void*) table1, 0, sizeof(page_table));
    memset((void*) table2, 0, sizeof(page_table));

    // Maps first MB to 3GB
    uint8_t *frame, *virt;
    for (frame = (uint8_t*)0x0, virt = (uint8_t*)0xC0000000;
         frame < (uint8_t*)0x100000/*0x100000*/;
         frame += PAGE_SIZE, virt += PAGE_SIZE) {
        page_table_entry page = 0;
        page_table_entry_add_attrib(&page, I86_PTE_PRESENT);
        page_table_entry_set_frame(&page, frame);
        table1->entries[PAGE_TABLE_INDEX((uintptr_t)virt)] = page;
    }

    // Maps kernel pages and phys mem pages
    for (frame = (uint8_t*)KERNEL_START_PADDR, virt = (uint8_t*)KERNEL_START_VADDR;
         frame < (uint8_t*)KERNEL_PHYS_MAP_END;
         frame += PAGE_SIZE, virt += PAGE_SIZE) {
        page_table_entry page = 0;
        page_table_entry_add_attrib(&page, I86_PTE_PRESENT);
        page_table_entry_set_frame(&page, frame);
        table2->entries[PAGE_TABLE_INDEX((uintptr_t)virt)] = page;
    }

    page_dir_entry *pde1 = (page_dir_entry*) &kernel_page_dir->entries[PAGE_DIRECTORY_INDEX(0x00000000)]; //pdirectory_lookup_entry(cur_directory, 0x00000000);
    page_dir_entry_add_attrib(pde1, I86_PDE_PRESENT);
    page_dir_entry_add_attrib(pde1, I86_PDE_WRITABLE);
    page_dir_entry_set_frame(pde1, table1);

    page_dir_entry **pde2 = (page_dir_entry*) &kernel_page_dir->entries[PAGE_DIRECTORY_INDEX(0xC0100000)]; //pdirectory_lookup_entry(cur_directory, 0xC0100000);
    page_dir_entry_add_attrib(pde2, I86_PDE_PRESENT);
    page_dir_entry_add_attrib(pde2, I86_PDE_WRITABLE);
    page_dir_entry_set_frame(pde2, table2);

    update_phys_memory_bitmap_addr(KERNEL_END_VADDR);

    enable_paging(kernel_page_dir);

    //tty_printf("Virtual memory manager initialized!\n");
}

void vmm_test() {
    tty_printf("kernel_page_dir = %x\n", kernel_page_dir);

    void *padr1 = 0xC0500000;
    void *vadr1 = vmm_temp_map_page(padr1);
    *(uint8_t*) vadr1 = 77;
    tty_printf("%x = %x\n", padr1, *(uint8_t*) vadr1);

    //tty_printf("%x = %x\n", (0x00100000), *(uint8_t*) (0x00100000)); IT WILL CAUSE PAGE FAULT!!!! BEACUSE WE 1:1 MAPPED UP TO 1MB PHYS MEM BUT NEVKLYUCHITELNO!
    tty_printf("%x = %x\n", (0x00100000 - 1), *(uint8_t*) (0x00100000 - 1));
    //asm volatile( "movl %0, %%cr3" :: "r" (kernel_page_dir));

    int eip;
    asm volatile("1: lea 1b, %0;": "=a"(eip));
    tty_printf("EIP = %x  ", eip);
}

// Add attribute to pte
void page_table_entry_add_attrib(page_table_entry *entry, uint32_t attrib) {
    *entry |= attrib;
}

// Delete attribute to pte
void page_table_entry_del_attrib(page_table_entry *entry, uint32_t attrib) {
    *entry &= ~attrib;
}

// Map pte to physical frame
void page_table_entry_set_frame(page_table_entry *entry, void *addr) {
    *entry = (*entry & ~I86_PTE_FRAME) | (uintptr_t)addr;
}

bool page_table_entry_is_present(page_table_entry entry) {
    return entry & I86_PTE_PRESENT;
}

bool page_table_entry_is_writable(page_table_entry entry) {
    return entry & I86_PTE_WRITABLE;
}

// Return the address of physical frame which pte refers to
void *page_table_entry_frame(page_table_entry entry) {
    return entry & I86_PTE_FRAME;
}

// Functions for Page Directory Entries

// Add attribute to pde
void page_dir_entry_add_attrib(page_dir_entry *entry, uint32_t attrib) {
    *entry |= attrib;
}

// Delete attribute to pde
void page_dir_entry_del_attrib(page_dir_entry *entry, uint32_t attrib) {
    *entry &= ~attrib; //old: was without ~ !!
}

// Map pde to physical frame (where the appropriate page table stores)
void page_dir_entry_set_frame(page_dir_entry *entry, void *addr) {
    *entry = (*entry & ~I86_PDE_FRAME) | (uintptr_t)addr;
}

bool page_dir_entry_is_present(page_dir_entry entry) {
    return entry & I86_PDE_PRESENT;
}

bool page_dir_entry_is_user(page_dir_entry entry) {
    return entry & I86_PDE_USER;
}

bool page_dir_entry_is_4mb(page_dir_entry entry) {
    return entry & I86_PDE_4MB;
}

bool page_dir_entry_is_writable(page_dir_entry entry) {
    return entry & I86_PDE_WRITABLE;
}

// Return the address of physical frame which pde refers to
void *page_dir_entry_frame(page_dir_entry entry) {
    return entry & I86_PDE_FRAME;
}

void flush_tlb_entry(void *addr) {
    asm volatile("invlpg (%0)" : : "b"(addr) : "memory");
}
