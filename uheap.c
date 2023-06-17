#include <inc/lib.h>

// malloc()
//	This function use FIRST FIT strategy to allocate space in heap
//  with the given size and return void pointer to the start of the allocated space

//	To do this, we need to switch to the kernel, allocate the required space
//	in Page File then switch back to the user again.
//
//	We can use sys_allocateMem(uint32 virtual_address, uint32 size); which
//		switches to the kernel mode, calls allocateMem(struct Env* e, uint32 virtual_address, uint32 size) in
//		"memory_manager.c", then switch back to the user mode here
//	the allocateMem function is empty, make sure to implement it.

//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//



/*
// all pages in heap
uint32 arr_all_heap[100000];
uint32 arr_size_all_pages[100000];
int size_arr_all_heap =0; // counter
*/
struct all_heap
{
	uint32 address_of_page;
	uint32 size_of_page;

};
struct all_heap arr_all_heap[100000];
int size_arr_all_heap =0; // counter

/*
// all free pages
uint32 arr_page_free [100000];
uint32 arr_size_free_pages[100000];
int size_arr_page_free =0; // counter
*/
struct all_free_page
{
	uint32 address_of_free_page;
	uint32 size_of_free_page;
};
struct all_free_page arr_page_free[100000];
int size_arr_page_free =0; // counter

// last pages added
uint32 stop_final_address = USER_HEAP_START;

void* malloc(uint32 size)
{
	//TODO: [PROJECT 2021 - [2] User Heap] malloc() [User Side]
	// Write your code here, remove the panic and write your code
	//panic("malloc() is not implemented yet...!!");

	//This function should find the space of the required range
	//using the BEST FIT strategy

	//refer to the project presentation and documentation for details


	uint32 space = ROUNDUP(size , PAGE_SIZE);
	uint32 number_pages_to_add = space/PAGE_SIZE;

			// add normal
			if (size_arr_page_free == 0)
			{
				//cprintf("\nppppppp\n");
				uint32 available_page = (USER_HEAP_MAX - stop_final_address) /PAGE_SIZE;
				if (available_page < number_pages_to_add){
					return NULL;
				}
				uint32 start_virtual_addr = stop_final_address;
				sys_allocateMem((uint32)start_virtual_addr, size);

				arr_all_heap[size_arr_all_heap].address_of_page = start_virtual_addr;
				arr_all_heap[size_arr_all_heap].size_of_page = (number_pages_to_add*PAGE_SIZE);
				size_arr_all_heap++;

				stop_final_address = stop_final_address + (number_pages_to_add*PAGE_SIZE);
				return (void*)start_virtual_addr;
			}



/*
 *
 * best fit
 *
 **/

	if (size_arr_page_free > 0)
	{

		/*cprintf("no _page to add = %d \n " , number_pages_to_add);
		for(int i=0;i<size_arr_page_free;i++)
		{
			cprintf("va = %x \n" , arr_page_free[i]);
			cprintf("no_page = %d \n" , arr_size_free_pages[i]/PAGE_SIZE);

		}*/
		//cprintf("pppppppppppp\n");
		int ind=-1;
		uint32 min_allocate =1000000000;
		//cprintf("9999\n");

		//cprintf("size = %d" , size_arr_page_free);

		for (int i=0;i<size_arr_page_free;i++)
		{

			uint32 n_of_free_page = arr_page_free[i].size_of_free_page / PAGE_SIZE;
			if (min_allocate > n_of_free_page && n_of_free_page >= number_pages_to_add )
			{
			//	cprintf("6666");
				min_allocate = n_of_free_page ;
				ind =i;
			}
		}

		uint32 available_pages = (USER_HEAP_MAX-stop_final_address) / PAGE_SIZE;
		if ((min_allocate > available_pages && available_pages >=number_pages_to_add) ||
				min_allocate ==1000000000)
		{

			uint32 available_page = (USER_HEAP_MAX - stop_final_address)/PAGE_SIZE;
			if (available_page< number_pages_to_add){
				return NULL;
			}
			//cprintf("8888");
			uint32 start_virtual_addr = stop_final_address;
			sys_allocateMem((uint32)start_virtual_addr, size);

			arr_all_heap[size_arr_all_heap].address_of_page = start_virtual_addr;
			arr_all_heap[size_arr_all_heap].size_of_page = number_pages_to_add*PAGE_SIZE;
			size_arr_all_heap++;

			stop_final_address = stop_final_address + (number_pages_to_add*PAGE_SIZE);
			return (void*)start_virtual_addr;



		}


		else
		{

				if(ind != -1){
					uint32 start_address = arr_page_free[ind].address_of_free_page;
					sys_allocateMem((uint32)start_address, size);

			//		cprintf("7777");
					// add in heap array
					arr_all_heap[size_arr_all_heap].address_of_page = start_address;
					arr_all_heap[size_arr_all_heap].size_of_page = (number_pages_to_add*PAGE_SIZE);
					size_arr_all_heap++;


					// delete of array free page
					for(int i =ind ;i <(size_arr_page_free-1);i++)
					{
						arr_page_free[i].address_of_free_page = arr_page_free[i+1].address_of_free_page;
						arr_page_free[i].size_of_free_page = arr_page_free[i+1].size_of_free_page;

					}
					size_arr_page_free--;

					if(min_allocate != number_pages_to_add)
					{
						uint32 newStartAddress = start_address +( number_pages_to_add*PAGE_SIZE);
						arr_page_free[size_arr_page_free].address_of_free_page = newStartAddress;
						arr_page_free[size_arr_page_free].size_of_free_page = (min_allocate*PAGE_SIZE) - (number_pages_to_add*PAGE_SIZE);
						size_arr_page_free++;
					}

					return (void*)start_address;
				}



		}

	}



	return NULL;
}

// free():
//	This function frees the allocation of the given virtual_address
//	To do this, we need to switch to the kernel, free the pages AND "EMPTY" PAGE TABLES
//	from page file and main memory then switch back to the user again.
//
//	We can use sys_freeMem(uint32 virtual_address, uint32 size); which
//		switches to the kernel mode, calls freeMem(struct Env* e, uint32 virtual_address, uint32 size) in
//		"memory_manager.c", then switch back to the user mode here
//	the freeMem function is empty, make sure to implement it.

void free(void* virtual_address)
{
	//TODO: [PROJECT 2021 - [2] User Heap] free() [User Side]
	// Write your code here, remove the panic and write your code
	//panic("free() is not implemented yet...!!");

	//you should get the size of the given allocation using its address

	//refer to the project presentation and documentation for details


	uint32 size_of_virtual_to_remove;
		int ch=0;
		int index =0;
		int i;
		int flag_up =0;
		int flag_down =0;
		int index_up;
		int index_down;

		for( i =0 ; i<size_arr_all_heap ; i++)
		{
			if (arr_all_heap[i].address_of_page == (uint32)virtual_address)
			{
				//cprintf("111");
				size_of_virtual_to_remove = arr_all_heap[i].size_of_page;
				index = i;
				//ch=1;
				break;
			}
		}



		// up
	uint32 v = (uint32)virtual_address + (size_of_virtual_to_remove);
		for (int i=0;i<size_arr_page_free;i++)
		{
			//cprintf("up\n");
			if (v == arr_page_free[i].address_of_free_page)
			{
			flag_up = 1;
			index_up =i;

			}
		}


		// con down

		for(int i=0 ; i<size_arr_page_free ; i++)
		{
			//cprintf("down\n");

			uint32 x = arr_page_free[i].address_of_free_page + (arr_page_free[i].size_of_free_page);
			if (x == (uint32)virtual_address)
			{
				flag_down =1;
				index_down = i;

			}
		}
		//up only
		if (flag_up == 1 && flag_down ==0)
		{

			arr_page_free[index_up].address_of_free_page = (uint32)virtual_address;
			arr_page_free[index_up].size_of_free_page += size_of_virtual_to_remove;

		}
		//down only
		else if (flag_up == 0 && flag_down ==1)
		{
			arr_page_free[index_down].size_of_free_page+=size_of_virtual_to_remove;

		}
		//up and down
		else if(flag_up == 1 && flag_down ==1)
		{
			arr_page_free[index_down].size_of_free_page+=size_of_virtual_to_remove +arr_page_free[index_up].size_of_free_page ;
			for(int i=index_up; i < (size_arr_page_free-1 ); i++)
			{
				arr_page_free[i].address_of_free_page = arr_page_free[i+1].address_of_free_page;
				arr_page_free[i].size_of_free_page = arr_page_free[i+1].size_of_free_page;

			}
			size_arr_page_free--; //free
		}


	// no con
		else
		{
			arr_page_free[size_arr_page_free].address_of_free_page = (uint32)virtual_address;
			arr_page_free[size_arr_page_free].size_of_free_page = size_of_virtual_to_remove;
			size_arr_page_free++;
		}








		sys_freeMem((uint32)virtual_address,size_of_virtual_to_remove);

					// Delete from array of heap
					for(int i=index ; i <(size_arr_all_heap-1);i++)
					{
						arr_all_heap[i].address_of_page=arr_all_heap[i+1].address_of_page;
						arr_all_heap[i].size_of_page = arr_all_heap[i+1].size_of_page;
					}
					size_arr_all_heap--;





}

//==================================================================================//
//================================ OTHER FUNCTIONS =================================//
//==================================================================================//

void* smalloc(char *sharedVarName, uint32 size, uint8 isWritable)
{
	panic("this function is not required...!!");
	return 0;
}

void* sget(int32 ownerEnvID, char *sharedVarName)
{
	panic("this function is not required...!!");
	return 0;
}

void sfree(void* virtual_address)
{
	panic("this function is not required...!!");
}

void *realloc(void *virtual_address, uint32 new_size)
{
	panic("this function is not required...!!");
	return 0;
}

void expand(uint32 newSize)
{
	panic("this function is not required...!!");
}
void shrink(uint32 newSize)
{
	panic("this function is not required...!!");
}

void freeHeap(void* virtual_address)
{
	panic("this function is not required...!!");
}
