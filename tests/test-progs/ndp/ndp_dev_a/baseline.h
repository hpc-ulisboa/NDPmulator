#ifndef __BASELINE_H__
#define __BASELINE_H__

#include <cstdint>

uint64_t compare_n_hit(uint64_t *data, uint64_t size, uint64_t skey);

uint64_t compare_n_count(uint64_t *data, uint64_t size, uint64_t skey);

uint64_t compare_n_max(uint64_t *data, uint64_t size);

#endif
