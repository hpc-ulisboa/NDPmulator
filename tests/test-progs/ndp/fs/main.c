#include <stdlib.h>
#include <sys/mman.h>
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#define NDP_ADDR 0x80000000
#define NDP_SIZE 0x40000000

unsigned long hexstr2ulong(char const *hexsrt) {
	assert(hexsrt[0] == '0' && hexsrt[1] == 'x');

	unsigned long ulong = 0;
	for (unsigned long i = strlen(hexsrt) - 1, w = 1; i > 1; i--, w *= 16) {
		switch (hexsrt[i]) {
		case 'f': case 'F': ulong += 15 * w; break;
		case 'e': case 'E': ulong += 14 * w; break;
		case 'd': case 'D': ulong += 13 * w; break;
		case 'c': case 'C': ulong += 12 * w; break;
		case 'b': case 'B': ulong += 11 * w; break;
		case 'a': case 'A': ulong += 10 * w; break;
		default:			ulong += (hexsrt[i] - 48) * w;
		}
	}

	return ulong;
}

int main(int argc, char const *argv[])
{
	unsigned long ndp_addr = (argc > 1) ? hexstr2ulong(argv[1]) : NDP_ADDR;
	unsigned long ndp_size = (argc > 2) ? hexstr2ulong(argv[2]) : NDP_SIZE;

	int fd_dev_mem = open("/dev/mem", O_RDWR | O_SYNC);
	assert(fd_dev_mem);

	void *ndp_mem = mmap(
		NULL,					// addr
		(size_t) ndp_size,		// lenght
		PROT_READ | PROT_WRITE,	// prot
		MAP_SHARED,				// flags
		fd_dev_mem,				// fd
		ndp_addr				// offset
	);

	int *var = (int *) ndp_mem;

	*var = 1;

	return 0;
}
