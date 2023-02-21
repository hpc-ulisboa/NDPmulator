#include <stdlib.h>
#include <sys/mman.h>
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#define NDP_ADDR 0x620000000
#define NDP_SIZE 0x1000

int main(int argc, char const *argv[])
{
	
	
	int fd_dev_mem = open("/dev/mem", O_RDWR | O_SYNC);
	assert(fd_dev_mem);

	char *ndp_mem = (char *) mmap(
		NULL,					// addr
		(size_t) NDP_SIZE,		// lenght
		PROT_READ | PROT_WRITE,	// prot
		MAP_SHARED,				// flags
		fd_dev_mem,				// fd
		NDP_ADDR				// offset
	);

	printf("%s\n", ndp_mem);

	return 0;
}
