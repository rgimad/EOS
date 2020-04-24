/*
*    EOS - Experimental Operating System
*    Process methods
*/
#include <kernel/pm/process.h>
#include <kernel/pm/thread.h>
#include <kernel/pm/scheduler.h>
#include <kernel/pm/elf.h>
#include <kernel/mm/phys_memory.h>
#include <kernel/mm/virt_memory.h>
#include <kernel/mm/kheap.h>
#include <kernel/mm/uheap.h>
#include <kernel/fs/vfs.h>
#include <kernel/tty.h>
#include <libk/string.h>

process_t* create_process(char *filepath)
{
	void *file_buffer = elf_open(filepath);
	if (file_buffer == 0) { return 0; }

	struct elf_hdr *hdr = (struct elf_hdr*)file_buffer;
	if (elf_check_header(hdr) != 0) // if elf header is not valid then quit
	{
		kheap_free(file_buffer);
		return 0;
	}

	//asm("cli;"); // disable interrupts

	//we allocate new physical page for proc_page_dir
	page_directory *proc_page_dir = (page_directory*)pmm_alloc_block();
	if (proc_page_dir == (page_directory *)0xFFFFFFFF)//if no free space
	{
		tty_printf("Failed to allocate phys memory for kernel page dir\n");
		//panic
		return 0;
	}
	//we copy kernel page_dir contents to proc_page_dir
	page_directory *pdbuf = kheap_malloc(PAGE_SIZE);//temporary buffer
	virtual_addr kernel_page_dir_tmp_vaddr = vmm_temp_map_page(kernel_page_dir);//temporary mapping
	memcpy(pdbuf, (void*)kernel_page_dir_tmp_vaddr, PAGE_SIZE);
	virtual_addr proc_page_dir_tmp_vaddr = vmm_temp_map_page(proc_page_dir);//temporary mapping
	memcpy((void*)proc_page_dir_tmp_vaddr, (void*)pdbuf, PAGE_SIZE);
	kheap_free(pdbuf);//free temporary buffer

	// CHECK: will lines 40-46 copy kernel page_dir contents to proc_page_dir correcly?

	process_t *proc = kheap_malloc(sizeof(process_t));
	memset(proc, 0, sizeof(process_t));
	proc->pid = pid_counter++;
	proc->state = PROCESS_INTERRUPTIBLE;
	proc->threads_count = 0;
	proc->thread_list = list_create();
	// now add to scheduler's process_list
	proc->self_item = list_push(process_list, proc);

	proc->page_dir = proc_page_dir;
	
	vmm_switch_page_directory(proc_page_dir);

	// loading elf sections (from program header) to memory. There is code does the same in run_elf_file
	int i;
	for (i = 0; i < hdr->ph_ent_cnt; i++)
	{
		//printf("Segment [%i/%i]: ", i, hdr->ph_ent_cnt);
		struct elf_program_header *phdr = elf_get_program_header(file_buffer, i);
		if (phdr->type != SEGTYPE_LOAD)
		{
			continue; //We only can load segments to the memory, so just skip it.
		}
		//tty_printf("Loading %x bytes to %x\n", phdr->size_in_mem, phdr->load_to);
		//alloc needed amount of pages
		uint32_t alloc_addr;
		for (alloc_addr = phdr->load_to; alloc_addr < phdr->load_to + phdr->size_in_mem; alloc_addr += PAGE_SIZE)
		{
			vmm_alloc_page(alloc_addr);
		}
		memset((void*)phdr->load_to, 0, phdr->size_in_mem); //Null segment memory.
		memcpy((void*)phdr->load_to, file_buffer + phdr->data_offset, phdr->size_in_file);
		//tty_printf("Loaded\n");
	}

	vmm_switch_page_directory(proc->page_dir);
	uheap_init(proc);
	vmm_switch_page_directory(kernel_page_dir);

	create_user_thread(proc, hdr->entry); // now we create main thread of new process. 

	kheap_free(file_buffer);

	// name of process is the name of executable file
	vfs_get_file_name_from_path(filepath, proc->name);

	//asm("sti;"); // enable interrupts
	return proc;
}