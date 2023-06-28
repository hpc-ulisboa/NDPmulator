from m5.params import *
from m5.proxy import *
from m5.objects.ClockedObject import ClockedObject

class NDP(ClockedObject):
    type = 'NDP'
    cxx_header = "ndp/ndp.hh"
    cxx_class = 'gem5::NDP'

    cpu_side = ResponsePort("CPU side port, receives requests")
    mem_side = RequestPort("Memory side port, sends requests")
    dma_port = RequestPort("DMA port that connects the NDP device to the memory hierarchy")
    
    ndp_ctrl = Param.AddrRange(('0x40000000', '0x40001000'), "Memory Range reserved for the NDP device API")
    ndp_data = Param.AddrRange(('0x40001000', '0x80000000'), "Memory Range shared between the NDP device and the CPU")
    max_rsze = Param.Unsigned(0x800, "Maximum request size to memory")
    max_reqs = Param.Unsigned(1, "Number of clock cycles between requests to prevent the overflow of the packet queue")

    system = Param.System(Parent.any, "The system this NDP device is part of")
