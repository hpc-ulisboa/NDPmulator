#include "baseline.h"

uint64_t
compare_n_hit(uint64_t *data, uint64_t size, uint64_t skey)
{
    for (int i = 0; i < size; ++i)
        if (data[i] == skey)
            return 1;

    return 0;
}

uint64_t
compare_n_count(uint64_t *data, uint64_t size, uint64_t skey)
{
    uint64_t n = 0;

    for (int i = 0; i < size; ++i)
        if (data[i] == skey)
            n++;

    return n;
}

uint64_t
compare_n_max(uint64_t *data, uint64_t size)
{
    uint64_t max = data[0];
    for (int i = 1; i < size; ++i)
        if (data[i] > max)
            max = data[i];

    return max;
}
