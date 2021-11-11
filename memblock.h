typedef unsigned long long phys_addr_t;

struct memblock_region {
    phys_addr_t base;
    phys_addr_t size;

};

struct memblock_type {
    unsigned long cnt;	/* number of regions */
	unsigned long max;	/* size of the allocated array */
	phys_addr_t total_size;	/* size of all regions */
	struct memblock_region *regions;
	char *name;
};

struct memblock {
    struct memblock_type memory;
	struct memblock_type reserved;
};
