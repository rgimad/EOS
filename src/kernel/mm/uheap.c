/*
*    EOS - Experimental Operating System
*    User heap manager module
*/
#include <kernel/mm/uheap.h>
#include <kernel/mm/virt_memory.h>
#include <libk/string.h>

void uheap_init(process_t *proc)
{
	proc->heap_begin = UHEAP_START_VADDR; // UHEAP_START_VADDR is from memlayout.h
	proc->heap_end = NULL;
	proc->heap_allocs_num = 0;
	proc->heap_memory_used = 0;
}

// increase heap of user process by some amount, this will be rounded up by the page size 
void *uheap_morecore(process_t *proc, uint32_t size)
{
	// calculate how many pages we will need
	int pages = (size / PAGE_SIZE) + 1;
	// when uheap_end == NULL we must create the initial heap
	if (proc->heap_end == NULL)
		proc->heap_end = proc->heap_begin;
	// set the address to return
	void *prev_heap_end = proc->heap_end;
	// create the pages
	for (; pages-- > 0; proc->heap_end += PAGE_SIZE)
	{
		vmm_alloc_page_with_userbit(proc->heap_end); // we use vmm_alloc_page version which sets up the User bit for allocated page
		memset(proc->heap_end, 0x00, PAGE_SIZE);
	}
	// return the start address of the memory we allocated to the heap
	//tty_printf("(prev_heap_end) = %x\n", prev_heap_end);
	return prev_heap_end;
}

// free a previously allocated item from the heap of user process
void uheap_free(process_t *proc, void *address)
{
	uheap_item *tmp_item, *item;
	// sanity check
	if (address == NULL) return;
	// set the item to remove
	item = (uheap_item*)((uint32_t)address - (uint32_t)sizeof(uheap_item));
	// find it
	for (tmp_item = proc->heap_begin; tmp_item != NULL; tmp_item = tmp_item->next)
	{
		//tty_printf("tmp_item = %x\n", tmp_item);
		if (tmp_item == item)
		{
			// free it
			tmp_item->used = FALSE;
			proc->heap_memory_used -= tmp_item->size;
			proc->heap_allocs_num--;
			// coalesce any adjacent free items
			for (tmp_item = proc->heap_begin; tmp_item != NULL; tmp_item = tmp_item->next)
			{
				while (!tmp_item->used && tmp_item->next != NULL && !tmp_item->next->used)
				{
					tmp_item->size += sizeof(uheap_item) + tmp_item->next->size;
					tmp_item->next = tmp_item->next->next;
				}
			}
			// break and return as we are finished
			break;
		}
	}
}


// allocates an arbiturary size of memory (via first fit) from the heap of process
void *uheap_malloc(process_t *proc, uint32_t size)
{
	uheap_item *new_item = NULL, *tmp_item;
	uint32_t total_size;
	// sanity check
	if (size == 0) return NULL;

	// round up by 8 bytes and add header size
	total_size = ( ( size + 7 ) & ~7 ) + sizeof(uheap_item);

	uheap_item *last_item;////////////////
	// if the heap exists
	if (proc->heap_end != NULL)
	{
		// search for first fit

		for (new_item = proc->heap_begin; new_item != NULL; new_item = new_item->next)
		{
			if (new_item->next == NULL) last_item = new_item;////////////////

			if (!new_item->used && (total_size <= new_item->size))
				break;
		}
	}
	// if we found one
	if (new_item != NULL)
	{
		tmp_item = (uheap_item*)((uint32_t)new_item + total_size);
		tmp_item->size = new_item->size - total_size;
		tmp_item->used = FALSE;
		tmp_item->next = new_item->next;
	}
	else
	{
		// didnt find a fit so we must increase the heap to fit
		new_item = uheap_morecore(proc, total_size);
		if (new_item == NULL)
		{
			// return NULL as we are out of physical memory!
			return NULL;
		}
		// create an empty item for the extra space uheap_morecore() gave us
		// we can calculate the size because morecore() allocates space that is page aligned
		tmp_item = (uheap_item*)((uint32_t)new_item + total_size);
		tmp_item->size = PAGE_SIZE - (total_size%PAGE_SIZE ? total_size%PAGE_SIZE : total_size) - sizeof(uheap_item);
		tmp_item->used = FALSE;
		tmp_item->next = NULL;

		//tty_printf("last_item = %x", last_item);///////////////why commenting this causes exception??? ANSWER IS BECAUSE OF FUCKING OPTIMIZATION -O1. i disabled it and it works now witout this line
		last_item->next = new_item;/////////////////////
	}
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! A KAK etot new_item svyazan c posledney item v spiske???? to est new_item eto ne poslednyaa item. solved!
	// create the new item
	new_item->size = size;
	new_item->used = TRUE;
	new_item->next = tmp_item;

	proc->heap_allocs_num++;
	proc->heap_memory_used += total_size;
	// return the newly allocated memory location
	return (void *)((uint32_t)new_item + (uint32_t)sizeof(uheap_item));//old (int)... + ...
}
