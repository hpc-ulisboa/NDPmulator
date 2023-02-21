#include <stdlib.h>
#include <sys/mman.h>
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

#define NDP_ADDR 0x620000000
#define NDP_SIZE 0x1000

int main(int argc, char const *argv[])
{
	// Map memory directly
	int fd_dev_mem = open("/dev/mem", O_RDWR | O_SYNC);
	assert(fd_dev_mem);

	uint64_t *ndp_mem = (uint64_t *) mmap(
		NULL,					// addr
		(size_t) NDP_SIZE,		// lenght
		PROT_READ | PROT_WRITE,	// prot
		MAP_SHARED,				// flags
		fd_dev_mem,				// fd
		NDP_ADDR				// offset
	);

	// Perform diferent operations depending on argument
	if (!strcmp(argv[1], "read"))
	{
		for (int i = 0; i < 7; ++i)
		{
			printf("ndp_mem[%d] = %lu\n", i, ndp_mem[i]);
		}
	}
	else if (!strcmp(argv[1], "write"))
	{
		for (int i = 0; i < 7; ++i)
		{
			ndp_mem[i] = (i + 1) * 10;
		}
	}
	else if (!strcmp(argv[1], "driver"))
	{
		uint64_t dwri[5] = {1, 2, 3 ,4, 5};
		uint64_t drea[2];

		int fd_ndp_dev_a = open("/dev/ndp_dev_a", O_RDWR | O_SYNC);
		assert(fd_ndp_dev_a);

		assert(write(fd_ndp_dev_a, (void *) dwri, 0x28) == 0x28);
		assert(read(fd_ndp_dev_a, (void *) drea, 0x10) == 0x10);

		for (int i = 0; i < 2; ++i)
		{
			printf("ndp_mem[%d] = %lu\n", i + 5, drea[i]);
		}
	}

	return 0;
}
