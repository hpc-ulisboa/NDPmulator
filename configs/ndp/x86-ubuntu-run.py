"""
gem5-ndp final X86 FS Simulation script
autor: Joao Vieira
date: 2023/01/27
"""

from m5.util import warn
from m5.objects import NDPDevA

from gem5.utils.requires import requires
from gem5.components.memory.single_channel import SingleChannelDDR3_1600
from gem5.isas import ISA
from gem5.resources.resource import CustomResource, CustomDiskImageResource
from gem5.simulate.simulator import Simulator
from gem5.components.processors.cpu_types import CPUTypes
from gem5.components.boards.x86_board import X86Board
from gem5.components.processors.simple_processor import SimpleProcessor
from gem5.components.cachehierarchies.classic.ndp_compatible_cache_hierarchy import (
    NDPCompatibleCacheHierarchy,
)

# Check if simulator was compiled for X86
requires(isa_required=ISA.X86)

warn(
    "This board is only for NDP devices evaluation purposes, and any performance"
    "results obtained with it are only meaningful when compared with other results"
    "obtained with the same board."
)

memory = SingleChannelDDR3_1600(size="2GB")

processor = SimpleProcessor(
    cpu_type=CPUTypes.TIMING,
    isa=ISA.X86,
    num_cores=1
)

ndp_device = NDPDevA(
    ndp_ctrl=('0x40000000', '0x40001000'),
    ndp_data=('0x40001000', '0x80000000'),
    max_rsze=0x40
)

cache_hierarchy = NDPCompatibleCacheHierarchy(
    l1d_size="32kB",
    l1i_size="32kB",
    l2_size="256kB",
    l1d_assoc=8,
    l1i_assoc=4,
    l2_assoc=8,
    ndp_device=ndp_device,
)

board = X86Board(
    clk_freq="2GHz",
    processor=processor,
    memory=memory,
    cache_hierarchy=cache_hierarchy,
)

board.set_kernel_disk_workload(
    kernel=CustomResource("fs_files/binaries/x86-linux-kernel-5.4.0-105-generic"),
    kernel_args=[
        "earlyprintk=ttyS0",
        "console=ttyS0",
        "lpj=7999923",
        "root=/dev/sda1",
        "mem=1G",
        "memmap=1G@0",
        "memmap=1G$1G",
    ],
    disk_image=CustomDiskImageResource("fs_files/disks/x86-ubuntu-18.04-img"),
)

simulator = Simulator(board=board)
simulator.run()
