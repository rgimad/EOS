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
#include <kernel/fs/vfs.h>

process_t* create_process(char *filepath)
{
	void *file_buffer = elf_open(filepath);
	if (file_buffer == 0) { return 0; }

	struct elf_hdr *hdr = (struct elf_hdr*)file_buffer;
	if (elf_check_header(hdr) != 0)//if elf header is not valid quit
	{
		kheap_free(file_buffer);
		return 0;
	}

	asm("cli;");//disable interrupts

	//we allocate new physical page for proc_page_dir
	page_directory *proc_page_dir = (page_directory*)pmm_alloc_block();
	if (proc_page_dir == 0xFFFFFFFF)//if no free space
	{
		tty_printf("Failed to allocate phys memory for kernel page dir\n");
		//panic
		return 0;
	}
	//we copy kernel page_dir contents to proc_page_dir
	page_directory *pdbuf = kheap_alloc(PAGE_SIZE);//temporary buffer
	virtual_addr kernel_page_dir_tmp_vaddr = vmm_temp_map_page(kernel_page_dir);//temporary mapping
	memcpy(pdbuf, (void*)kernel_page_dir_tmp_vaddr, PAGE_SIZE);
	virtual_addr proc_page_dir_tmp_vaddr = vmm_temp_map_page(proc_page_dir);//temporary mapping
	memcpy(proc_page_dir_tmp_vaddr, (void*)pdbuf, PAGE_SIZE);
	kheap_free(pdbuf);//free temporary buffer

	//TODO: make 40-46 code review maybe its totally incorrect

	process_t *proc = kheap_alloc(sizeof(process_t));
	memset(proc, 0, sizeof(process_t));

	//proc->page_dir = ;

	//TODO: switch to proc_page_dir

	// loading elf sections (from program header) to memory. There is code does the same in run_elf_file
	int i;
	for (i = 0; i < hdr->ph_ent_cnt; i++)
	{
		//printf("Segment [%i/%i]: ", i, hdr->ph_ent_cnt);
		struct elf_program_header *phdr = elf_get_program_header(elf_file, i);
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
		memcpy((void*)phdr->load_to, elf_file+phdr->data_offset, phdr->size_in_file);
		//tty_printf("Loaded\n");
	}

	//TODO: switch back to kernel page dir

	proc->pid = pid_counter++;
	proc->state = PROCESS_INTERRUPTIBLE;
	proc->threads_count = 1;
	proc->thread_list = list_create();

	thread_t *proc_main_thread = (thread_t*)kheap_malloc(sizeof(thread_t));
    memset(proc_main_thread, 0, sizeof(thread_t));

	// allocate and set the top of the kernel stack of new process'es main thread
	void *proc_main_thread_kernel_stack = kheap_malloc(THREAD_KSTACK_SIZE);
	memset(proc_main_thread_kernel_stack, 0, THREAD_KSTACK_SIZE);
	proc_main_thread->kernel_stack = proc_main_thread_kernel_stack;

	// the user-stack for proc's main thread must be allocated using user-heap manager
	// otherwise user-stack allcated with kheap_alloc will become unaccessible from user-mode cause kheap_alloc uses kheap_morecore which uses vmm_alloc page which doesnt set up the User bit for allocated PTE's
	// How to solve this problem:
	// 1) make vmm_alloc_page receive one more argument - User-bit of page being allocated
	// 2) make user heap manager which will call vmm_alloc_page with argument userbit = 1
	// 3) so difference between kernel heap manager and user heap manager is that first doesnt set user bit but second does
	// 4) before using user heap manager you need switch to proc->page_dir and after using user heap manager switch back to kernel page directory

	//TODO: allocating heap for process

	//TODO: initialize heap_start, heap_end. and maybe stack_begin, stack_end (why they are necessary??)

	//TODO: create stdin and stdout for process

	kheap_free(file_buffer);

	// name of process is the name of executable file
	proc->name = vfs_get_file_name_from_path(filepath);

	// now add to scheduler's process_list
	proc->self_item = list_push(process_list, proc);

	asm("sti;");
	return proc;
}