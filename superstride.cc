#include "interface.hh"
#include <cstdlib>


#define TABLE_SIZE 65521
#define HIST_SIZE 10
#define STRIDES 5

typedef struct table_entry_St {
	struct table_entry_St *next;
	Addr pc;
	Addr last[HIST_SIZE];
} table_entry_t;

table_entry_t *table[TABLE_SIZE];


static table_entry_t *
find_entry (Addr pc)
{
	int index = pc % TABLE_SIZE;
	table_entry_t *entry = table[index];

	for (;entry != NULL && entry->pc != pc; entry = entry->next);

	if (entry == NULL) {
		entry = new table_entry_t;
		entry->pc = pc;

		for (int i = 0; i < HIST_SIZE; i++) {
			entry->last[i] = 0;
		}

		entry->next = table[index];
		table[index] = entry;
	}

	return entry;
}

static void
add_access (Addr pc, Addr addr)
{
	table_entry_t *entry = find_entry (pc);

	memmove (entry->last + 1, entry->last, sizeof (Addr) * (HIST_SIZE - 1));
	entry->last[0] = addr;
}

static void
calc_stride (Addr pc, Addr addr)
{
	table_entry_t *entry = find_entry (pc);
	Addr s[STRIDES] = {0};
	Addr a = addr;

	for (int i = STRIDES; i > 0; i--) {
		int hits = 0;

		for (int j = 0; j < i; j++) {
			if ((entry->last[    j] - entry->last[j     + 1]) ==
				(entry->last[i + j] - entry->last[i + j + 1])) {
				hits++;
			}

			s[j] = entry->last[j] - entry->last[j + 1];
		}

		if (hits == i) {
			for (int j = i - 1; j >= 0; j--) {
				a += s[j];

				if (a <= MAX_PHYS_MEM_ADDR && !in_cache (a))
					issue_prefetch (a);
			}

			break;
		}
	}
}


void prefetch_init(void)
{
    DPRINTF(HWPrefetch, "Initialized stride prefetcher\n");
}

void prefetch_access(AccessStat stat)
{
	add_access  (stat.pc, stat.mem_addr);
	calc_stride (stat.pc, stat.mem_addr);
}

void prefetch_complete(Addr addr)
{
}
