#include "interface.hh"
#include <stdlib.h>
#include <stdio.h>
#include <queue>
using namespace std;

#define INDEX_TABLE_SIZE 100
#define GLOBAL_HISTORY_BUFFER_SIZE 500

typedef struct ghb_entry {
	Addr addr;
	ghb_entry *next_ptr;
	ghb_entry *previous_ptr;
	int stride;
} ghb_entry_t;

typedef struct {
	Addr addr;
	ghb_entry_t *ghb_ptr;
} index_table_t;

FILE *file;
//index_table_t *index_table;
index_table_t index_table[INDEX_TABLE_SIZE];
queue<ghb_entry_t*> ghb;

unsigned int hash_index(Addr addr) {
	return (unsigned int)(addr % INDEX_TABLE_SIZE);
}

int in_index_table(Addr addr, unsigned int index) {
	if (index_table[index].addr == addr) return 1;
	return 0;
}

int calculate_stride(Addr a1, Addr a2)
{
	return (int)(a1 - a2);
}

void prefetch_init(void)
{		
	char buffer[50];
	sprintf(buffer, "/home/user/logs/logfile_%d.txt", getpid());
	
	file = fopen(buffer, "a+");
	fprintf(file, "NEW LOG\n");

	//index_table = (index_table_t *) calloc(INDEX_TABLE_SIZE, sizeof(index_table_t));

	// Initialize index table
	for (int i = 0; i < INDEX_TABLE_SIZE; i++) {
		index_table[i].addr = 0;
		index_table[i].ghb_ptr = NULL;
	}

    DPRINTF(HWPrefetch, "Initialized Markov prefetcher\n");
}

void prefetch_access(AccessStat stat)
{	
	unsigned int i;
	if (true) {
		i = hash_index(stat.pc);
		if (in_index_table(stat.pc, i) == 0)	{
			index_table[i].addr = stat.pc;

			// If FIFO is full, remove an entry to make room for a new one
			if (ghb.size() == GLOBAL_HISTORY_BUFFER_SIZE) {
				ghb_entry_t *tmp = ghb.front();

				// If there is an entry that points to the entry we are going 
				// to remove, set this pointer to NULL
				if (tmp->previous_ptr != NULL) tmp->previous_ptr->next_ptr = NULL;
				ghb.pop();
			}
			
			ghb_entry_t *new_ghb_entry = new ghb_entry_t; 
			
			if(!ghb.empty()) {
				ghb.back()->stride = calculate_stride(stat.mem_addr, ghb.back()->addr);
			}
			
			new_ghb_entry->addr = stat.mem_addr;
			new_ghb_entry->next_ptr = NULL;
			new_ghb_entry->previous_ptr = NULL;
			new_ghb_entry->stride = 0;
			ghb.push(new_ghb_entry);	
			index_table[i].ghb_ptr = new_ghb_entry;

			// Use sequential prefetching when the address appears for
			// the first time.
			//Addr prefetch_addr = stat.mem_addr + BLOCK_SIZE;
			//if (!in_cache(prefetch_addr)) issue_prefetch(prefetch_addr);
		}
		else {
			//fprintf(file, "Address already in index table (index %d). Searching for prefetch candidates\n", i);
			int j = 0;
			ghb_entry_t *old_entry = index_table[i].ghb_ptr;
			
			Addr prefetch_addr = (Addr) old_entry->stride + stat.mem_addr;	
			if (!in_cache(prefetch_addr) && prefetch_addr <= MAX_PHYS_MEM_ADDR) issue_prefetch(prefetch_addr);

			ghb_entry_t *next_entry = old_entry->next_ptr;
			
			//fprintf(file, "next_ptr: %p, next_entry->addr: %ld, stat.mem_addr: %ld\n", next_entry,
			//next_entry->addr, stat.mem_addr);

			while(next_entry != NULL && j < 1) {
				
				//fprintf(file, "INSIDE WHILE-LOOP!!!!\n");
				prefetch_addr = (Addr) next_entry->stride + stat.mem_addr;
				if (!in_cache(prefetch_addr) && prefetch_addr <= MAX_PHYS_MEM_ADDR) {
					issue_prefetch(prefetch_addr);
					j++;
					//fprintf(file, "Issuing a prefetch\n");
				}
				next_entry = next_entry->next_ptr;
			}

			if (ghb.size() == GLOBAL_HISTORY_BUFFER_SIZE) {
				ghb_entry_t *tmp = ghb.front();
				if (tmp->previous_ptr != NULL) tmp->previous_ptr->next_ptr = NULL;
				ghb.pop();
			}
			
			ghb_entry_t *new_entry = new ghb_entry_t;
			new_entry->addr = stat.mem_addr;
			new_entry->next_ptr = old_entry;
			new_entry->previous_ptr = NULL;
			new_entry->stride = 0;
			ghb.back()->stride = calculate_stride(stat.mem_addr, ghb.back()->addr);
			old_entry->previous_ptr = new_entry;
			ghb.push(new_entry);
			index_table[i].ghb_ptr = new_entry;
		}
	}

}

void prefetch_complete(Addr addr) {
    /*
     * Called when a block requested by the prefetcher has been loaded.
     */
}
