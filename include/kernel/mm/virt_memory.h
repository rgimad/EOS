/*
*    EOS - Experimental Operating System
*    Virtual memory manager header
*/
#ifndef _VIRT_MEMORY_H_
#define _VIRT_MEMORY_H_

#include <kernel/mm/memlayout.h>
#include <stdbool.h>
#include <stdint.h>

//-------------------------------------------------------------------

// Page Table Entry points to a single page stored somewhere in memory/disk
typedef uint32_t page_table_entry;
// Page Directory Entry points to a diretory with 1024 PT Entries
typedef uint32_t page_dir_entry;

// Page Directory contains 1024 page directory entries
typedef struct __attribute__((aligned(4096))) page_directory {
  page_dir_entry entries[PAGES_PER_DIR];
} page_directory;

// Page Table contains 1024 page table entries
typedef struct __attribute__((aligned(4096))) page_table {
	page_table_entry entries[PAGES_PER_TABLE];
} page_table;


enum PAGE_PTE_FLAGS {
	I86_PTE_PRESENT = 1,
	I86_PTE_WRITABLE = 2,
	I86_PTE_USER = 4,
	I86_PTE_WRITETHOUGH = 8,
	I86_PTE_NOT_CACHEABLE = 0x10,
	I86_PTE_ACCESSED = 0x20,
	I86_PTE_DIRTY = 0x40,
	I86_PTE_PAT = 0x80,
	I86_PTE_CPU_GLOBAL = 0x100,
	I86_PTE_LV4_GLOBAL = 0x200,
	I86_PTE_FRAME = 0x7FFFF000
};

enum PAGE_PDE_FLAGS {
	I86_PDE_PRESENT = 1,
	I86_PDE_WRITABLE = 2,
	I86_PDE_USER = 4,
	I86_PDE_PWT = 8,
	I86_PDE_PCD = 0x10,
	I86_PDE_ACCESSED = 0x20,
	I86_PDE_DIRTY = 0x40,
	I86_PDE_4MB = 0x80,
	I86_PDE_CPU_GLOBAL = 0x100,
	I86_PDE_LV4_GLOBAL = 0x200,
	I86_PDE_FRAME = 0x7FFFF000
};
//------------------------------------------------------------------------------------

#define PAGE_ALIGN_DOWN(x) ((x) & -PAGE_SIZE)
#define PAGE_ALIGN_UP( address )	( ( (address) + PAGE_SIZE - 1 ) & ~( PAGE_SIZE - 1 ) )//wtf???

#define PAGE_DIRECTORY_INDEX(x) (((x) >> 22) & 0x3FF)
#define PAGE_TABLE_INDEX(x) (((x) >> 12) & 0x3FF)

#define PAGE_GET_TABLE_ADDRESS(x) (*x & ~0xFFF)//read address(20bits) from pde, other 12 bits are flags
#define PAGE_GET_PHYSICAL_ADDRESS(x) (*x & ~0xFFF)//read address(20bits) from pte, other 12 bits are flags

#define GET_PDE(v)		(page_dir_entry*)(0xFFFFF000 +  (v >> 22) * 4) //get pointer to pde using recursive mapping
#define GET_PTE(v)		(page_table_entry*)(0xFFC00000 + (v >> 12) * 4)
//-------------------------------------------------------------------------------------

page_directory *kernel_page_dir;//pointer (physical) to kernel page dircetory structure


//TODO: rewrite all these functions so that they will work using recursive pd techinque
//functions for Page Table Entries

//add attribute to pte
inline void page_table_entry_add_attrib(page_table_entry* entry, uint32_t attrib) {*entry |= attrib;}

//delete attribute to pte
inline void page_table_entry_del_attrib(page_table_entry* entry, uint32_t attrib) {*entry &= ~attrib;}

//map pte to physical frame
inline void page_table_entry_set_frame(page_table_entry* entry, physical_addr addr) {*entry = (*entry & ~I86_PTE_FRAME) | addr;}

inline bool page_table_entry_is_present(page_table_entry entry) {return entry & I86_PTE_PRESENT;}

inline bool page_table_entry_is_writable(page_table_entry entry) {return entry & I86_PTE_WRITABLE;}

//return the address of physical frame which pte refers to
inline physical_addr page_table_entry_frame(page_table_entry entry) {return entry & I86_PTE_FRAME;}

//--------------------------------------------------------------------
//functions for Page Directory Entries

//add attribute to pde
inline void page_dir_entry_add_attrib(page_dir_entry* entry, uint32_t attrib) {*entry |= attrib;}

//delete attribute to pde
inline void page_dir_entry_del_attrib(page_dir_entry* entry, uint32_t attrib) {*entry &= ~attrib;}//old: was without ~ !!

//map pde to physical frame (where the appropriate page table stores)
inline void page_dir_entry_set_frame(page_dir_entry* entry, physical_addr addr) {*entry = (*entry & ~I86_PDE_FRAME) | addr;}

inline bool page_dir_entry_is_present(page_dir_entry entry) {return entry & I86_PDE_PRESENT;}

inline bool page_dir_entry_is_user(page_dir_entry entry) { return entry & I86_PDE_USER; }

inline bool page_dir_entry_is_4mb(page_dir_entry entry) { return entry & I86_PDE_4MB; }

inline bool page_dir_entry_is_writable(page_dir_entry entry) {return entry & I86_PDE_WRITABLE;}

//return the address of physical frame which pde refers to
inline physical_addr page_dir_entry_frame(page_dir_entry entry) {return entry & I86_PDE_FRAME;}

//inline void page_dir_entry_enable_global(page_dir_entry entry) {}

inline void flush_tlb_entry(virtual_addr addr) { asm volatile("invlpg (%0)" : : "b"(addr) : "memory"); }//???
inline void flush_tlb_all() {asm volatile("movl %%cr3, %%eax\n"
	"movl %%eax, %%cr3" : : : "%eax");}

//----------------------------------------------------------------------------------

extern enable_paging(physical_addr page_dir);

void vmm_init();
void vmm_create_kernel_page_dir();

bool vmm_alloc_page(virtual_addr vaddr);
void vmm_free_page(virtual_addr vaddr);
void vmm_map_page(physical_addr paddr, virtual_addr vaddr);
virtual_addr vmm_temp_map_page(physical_addr paddr);


#endif /* _VIRT_MEMORY_H_ */
