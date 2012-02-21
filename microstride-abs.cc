#include "interface.hh"
#include <cstdlib>


#define TABLE_SIZE 372
#define STRIDES 5
#define HIST_SIZE (STRIDES * 2 + 1)


typedef struct table_entry_St {
	short last[HIST_SIZE];
} table_entry_t;

table_entry_t table[TABLE_SIZE];

static table_entry_t *
find_entry (Addr pc)
{
	int index = (pc / 4) % TABLE_SIZE;

	return &table[index];
}

static void
add_access (Addr pc, Addr addr)
{
	table_entry_t *entry = find_entry (pc);

	memmove (entry->last + 1, entry->last, sizeof (short) * (HIST_SIZE - 1));
	entry->last[0] = addr / 64;
}

static void
issue (Addr pc, Addr addr)
{
	if (addr <= MAX_PHYS_MEM_ADDR && !in_cache (addr) && !in_mshr_queue (addr)) {
		issue_prefetch (addr);
	}
}

static void
prefetch (Addr pc, Addr addr)
{
	table_entry_t *entry = find_entry (pc);
	Addr s[HIST_SIZE - 1] = {0};

	for (int i = STRIDES; i > 0; i--) {
		int hits = 0;

		for (int j = 0; j < i; j++) {
			if (entry->last[j] - entry->last[i] == entry->last[i + j] - entry->last[2 * i])
				hits++;

			s[j] = (Addr)(entry->last[j] - entry->last[i]) * 64;
		}

		if (hits == i) {
			for (int j = 0; j < i; j++) {
				issue (pc, addr + s[j]);
			}
			/*
			for (int j = 0, k = i - 1, l = 1; j < HIST_SIZE / 2; j++) {
				issue (pc, addr + s[k] * l);

				k--;
				if (k < 0) {
					k = i - 1;
					l++;
				}
			}
			*/

			break;
		}
	}


	/* Find most common stride */
	for (int i = 0; i < HIST_SIZE - 1; i++) {
		s[i] = (entry->last[i] - entry->last[i + 1]) * 64;
	}

	Addr mcs = 0;
	int count = 0;

	for (int i = 0; i < HIST_SIZE - 1; i++) {
		int c = 0;
		Addr m = s[i];

		for (int j = i + 1; j < HIST_SIZE - 1; j++) {
			if (s[j] == m)
				c++;
		}

		if (c > count && m != 0) {
			mcs = m;
			count = c;
		}
	}

	if (count > 3) {
		issue (pc, addr + mcs);
		issue (pc, addr + mcs * 2);
		issue (pc, addr + mcs * 3);
		issue (pc, addr + mcs * 4);
	}
}


void prefetch_init(void)
{
    DPRINTF(HWPrefetch, "Initialized stride prefetcher\n");
}

void prefetch_access(AccessStat stat)
{
	add_access (stat.pc, stat.mem_addr);
	prefetch   (stat.pc, stat.mem_addr);
}

void prefetch_complete(Addr addr)
{
}
