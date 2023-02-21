from m5.util import warn
from m5.objects import NDP

from gem5.utils.requires import requires
from gem5.components.boards.riscv_board import RiscvBoard
from gem5.components.memory import DualChannelDDR4_2400
from gem5.components.processors.simple_processor import SimpleProcessor
from gem5.components.processors.cpu_types import CPUTypes
from gem5.isas import ISA
from gem5.simulate.simulator import Simulator
from gem5.resources.resource import CustomResource, CustomDiskImageResource
from gem5.components.cachehierarchies.classic.ndp_compatible_cache_hierarchy import NDPCompatibleCacheHierarchy

# Check if simulator was compiled for RISCV
requires(isa_required=ISA.RISCV)

warn(
	"This board is only for NDP devices evaluation purposes, and any performance"
	"results obtained with it are only meaningful when compared with other results"
	"obtained with the same board."
)

memory = DualChannelDDR4_2400(size="3GB")

processor = SimpleProcessor(
	cpu_type=CPUTypes.TIMING,
	isa=ISA.RISCV,
	num_cores=1
)

ndp_device = NDP(ndp_rnge=('0x80000000', '0xc0000000'))

cache_hierarchy = NDPCompatibleCacheHierarchy(
	l1d_size="16kB",
	l1i_size="16kB",
	l2_size="256kB",
	ndp_device=ndp_device,
)

board = RiscvBoard(
	clk_freq="3GHz",
	processor=processor,
	memory=memory,
	cache_hierarchy=cache_hierarchy,
)

board.set_kernel_disk_workload(
	kernel=CustomResource("fs_files/binaries/riscv-bootloader-vmlinux-5.10"),
	kernel_args=[
		"earlyprintk=ttyS0",
		"console=ttyS0",
		"root=/dev/vda1",
		"rw",
		"mem=2G",
		"memmap=2G@0",
		"memmap=1G$2G",
	],
	disk_image=CustomDiskImageResource("fs_files/disks/riscv-ubuntu-20.04-img"),
)

simulator = Simulator(board=board)
simulator.run()
