/*
 *
 *
 */

#include "interface.hh"
#include <stdlib.h>
#include <stdio.h>

#define INDEX_TABLE_SIZE 3000
#define GLOBAL_HISTORY_BUFFER_SIZE 15000

typedef struct {
	Addr addr;
	int link_ptr;
} ghb_t;

typedef struct {
	Addr addr;
	int ghb_ptr;
} index_table_t;

FILE *file;
index_table_t *index_table;
ghb_t *ghb;
int first;
int last;
int count;

unsigned int hash_index(Addr addr) {
	return (unsigned int)(addr % INDEX_TABLE_SIZE);
}

int in_index_table(Addr addr, int index) {
	if (index_table[index].addr == addr) return 1;
	return 0;
}

int enqueue_index(void) {
	if (count >= GLOBAL_HISTORY_BUFFER_SIZE) {
		first = (first+1) % GLOBAL_HISTORY_BUFFER_SIZE;
		count -= 1;
	}
	last = (last+1) % GLOBAL_HISTORY_BUFFER_SIZE;
	count += 1;
	
	return last;
}

void prefetch_init(void)
{		
		file = fopen("/home/ole/skole/dmark/framework/prefetcher/prefetch_log.txt", "w");
		fprintf(file, "LOG FILE:\n");

		index_table = (index_table_t *) malloc( sizeof(index_table_t) * INDEX_TABLE_SIZE );
		ghb = (ghb_t *) malloc( sizeof(ghb_t) * GLOBAL_HISTORY_BUFFER_SIZE );

		first = 0;
		last = GLOBAL_HISTORY_BUFFER_SIZE-1;
		count = 0;

    DPRINTF(HWPrefetch, "Initialized Markov prefetcher\n");
}

void prefetch_access(AccessStat stat)
{	
	int i;
	if (stat.miss) {
		//printf("A MISS OCCURED\n");
		i = hash_index(stat.mem_addr);
		if (!in_index_table(stat.mem_addr, i))	{
			//printf("ADDING ADDRESS TO INDEX %d IN INDEX TABLE\n", i);
			index_table[i].addr = stat.mem_addr;
			index_table[i].ghb_ptr = enqueue_index(); 
			ghb[index_table[i].ghb_ptr].addr = stat.mem_addr;
			ghb[index_table[i].ghb_ptr].link_ptr = -1;
		}
		else {
			fprintf(file, "Address already in index table (index %d). Search for prefetch candidates\n", i);
			Addr prefetch_addr;
			int j = 0;
			int old_ptr = index_table[i].ghb_ptr;
			int next_ptr = ghb[old_ptr].link_ptr;
			int new_ptr;
			
			fprintf(file, "next_ptr: %d, ghb[next_ptr].addr: %ld, stat.mem_addr: %ld", next_ptr,
			ghb[next_ptr].addr, stat.mem_addr);

			while((next_ptr != -1) && 
						(ghb[next_ptr].addr == stat.mem_addr) && 
						j < 2) {
				fprintf(file, "INSIDE WHILE-LOOP!!!!\n");
				prefetch_addr = ghb[(next_ptr-1) % GLOBAL_HISTORY_BUFFER_SIZE].addr;
				if (!in_cache(prefetch_addr)) {
					issue_prefetch(prefetch_addr);
					j++;
					fprintf(file, "Issuing a prefetch\n");
				}
				next_ptr = ghb[next_ptr].link_ptr;
			}
			new_ptr = enqueue_index();
			ghb[new_ptr].addr = stat.mem_addr;
			if (((old_ptr-1) % GLOBAL_HISTORY_BUFFER_SIZE) == new_ptr) ghb[new_ptr].link_ptr = -1;
			else ghb[new_ptr].link_ptr = old_ptr;
		}
	}

}

void prefetch_complete(Addr addr) {
    /*
     * Called when a block requested by the prefetcher has been loaded.
     */
}
