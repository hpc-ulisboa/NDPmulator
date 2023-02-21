# configured for Intel Skylake X
# https://www.7-cpu.com/cpu/Skylake_X.html

from m5.objects import Cache, L2XBar

# L1 cache declaration
class L1Cache(Cache):
    size = '32kB'
    tag_latency = 5      # assumed to be the same as the following
    data_latency = 5     # assumed to be the same as the following
    response_latency = 5 # worst case (best case is 4 cycles)
    mshrs = 4            # there's no info about this (gem5 default is used)
    tgts_per_mshr = 20   # there's no info about this (gem5 default is used)

    def __init__(self, assoc=8):
        super(L1Cache, self).__init__(assoc=assoc)

# L2 cache declaration
class L2Cache(Cache):
    size = '256kB'
    assoc = 8
    tag_latency = 14      # assumed to be the same as the following
    data_latency = 14     # assumed to be the same as the following
    response_latency = 14 # worst case (best case is 13 cycles)
    mshrs = 20            # there's no info about this (gem5 default is used)
    tgts_per_mshr = 12    # there's no info about this (gem5 default is used)

    def __init__(self):
        super(L2Cache, self).__init__()

# L1 XBar declaration
class L1XBar(L2XBar):
    def __init__(self):
        super(L1XBar, self).__init__()
