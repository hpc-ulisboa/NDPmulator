"""
gem5-ndp final ARM FS Simulation script
autor: Joao Vieira
date: 2023/01/27
"""

from m5.util import warn
from m5.objects import NDP

from gem5.isas import ISA
from m5.objects import ArmDefaultRelease
from gem5.utils.requires import requires
from gem5.resources.resource import CustomResource, CustomDiskImageResource
from gem5.simulate.simulator import Simulator
from m5.objects import VExpress_GEM5_Foundation
from gem5.components.boards.arm_board import ArmBoard
from gem5.components.memory import DualChannelDDR4_2400
from gem5.components.processors.cpu_types import CPUTypes
from gem5.components.processors.simple_processor import SimpleProcessor
from gem5.components.cachehierarchies.classic.ndp_compatible_cache_hierarchy import NDPCompatibleCacheHierarchy

from gem5.resources.workload import Workload

# Check if simulator was compiled for ARM
requires(isa_required=ISA.ARM)

warn(
	"This board is only for NDP devices evaluation purposes, and any performance"
	"results obtained with it are only meaningful when compared with other results"
	"obtained with the same board."
)

memory = DualChannelDDR4_2400(size="8GB")

processor = SimpleProcessor(
	cpu_type=CPUTypes.TIMING,
	num_cores=1,
	isa=ISA.ARM
)

ndp_device = NDP(ndp_rnge=('0x1c0000000', '0x200000000'))

cache_hierarchy = NDPCompatibleCacheHierarchy(
	l1d_size="16kB",
	l1i_size="16kB",
	l2_size="256kB",
	ndp_device=ndp_device,
)

release = ArmDefaultRelease()
platform = VExpress_GEM5_Foundation()

board = ArmBoard(
	clk_freq="3GHz",
	processor=processor,
	memory=memory,
	cache_hierarchy=cache_hierarchy,
	release=release,
	platform=platform,
)

board.set_kernel_disk_workload(
	kernel=CustomResource("fs_files/binaries/arm64-linux-kernel-5.4.49"),
	bootloader=CustomResource("fs_files/bootloaders/arm64-bootloader-foundation"),
	kernel_args=[
		"console=ttyAMA0",
		"lpj=19988480",
		"norandmaps",
		"root=/dev/vda1",
		"rw",
		"mem=7G",
		"memmap=7G@0",
		"memmap=1G$7G",
	],
	disk_image=CustomDiskImageResource("fs_files/disks/arm64-ubuntu-20.04-img"),
)

simulator = Simulator(board=board)
simulator.run()
