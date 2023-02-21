from ..abstract_cache_hierarchy import AbstractCacheHierarchy
from .abstract_classic_cache_hierarchy import AbstractClassicCacheHierarchy
from ..abstract_two_level_cache_hierarchy import AbstractTwoLevelCacheHierarchy
from .caches.l1dcache import L1DCache
from .caches.l1icache import L1ICache
from .caches.l2cache import L2Cache
from .caches.mmu_cache import MMUCache
from ...boards.abstract_board import AbstractBoard
from ....isas import ISA
from ....runtime import get_runtime_isa

from m5.objects import Cache, L2XBar, BaseXBar, SystemXBar, BadAddr, Port, NDP

from ....utils.override import *

class NDPCompatibleCacheHierarchy(
	AbstractClassicCacheHierarchy, AbstractTwoLevelCacheHierarchy
):
	"""
	A cache setup where the CPU core has a private L1 Data and Instruction 
	Cache and a shared L2 cache compatible with an NDP device.
	"""

	@staticmethod
	def _get_default_membus() -> SystemXBar:
		"""
		A method used to obtain the default memory bus of 64 bit in width for
		the NDPCompatibleCacheHierarchy CacheHierarchy.

		:returns: The default memory bus for the NDPCompatibleCacheHierarchy
		CacheHierarchy.

		:rtype: SystemXBar
		"""
		membus = SystemXBar(width=64)
		membus.badaddr_responder = BadAddr()
		membus.default = membus.badaddr_responder.pio
		return membus

	def __init__(
		self,
		l1d_size: str,
		l1i_size: str,
		l2_size: str,
		ndp_device: NDP,
		l1d_assoc: int = 8,
        l1i_assoc: int = 8,
        l2_assoc: int = 16,
		membus: BaseXBar = _get_default_membus.__func__(),
	) -> None:
		"""
		:param l1d_size: The size of the L1 Data Cache (e.g., "32kB").

		:type l1d_size: str

		:param  l1i_size: The size of the L1 Instruction Cache (e.g., "32kB").

		:type l1i_size: str

		:param l2_size: The size of the L2 Cache (e.g., "256kB").

		:type l2_size: str

		:param membus: The memory bus. This parameter is optional parameter and
		will default to a 64 bit width SystemXBar is not specified.

		:type membus: BaseXBar
		"""

		AbstractClassicCacheHierarchy.__init__(self=self)
		AbstractTwoLevelCacheHierarchy.__init__(
			self,
			l1i_size=l1i_size,
			l1i_assoc=l1i_assoc,
			l1d_size=l1d_size,
			l1d_assoc=l1d_assoc,
			l2_size=l2_size,
			l2_assoc=l2_assoc,
		)

		self.ndp_device = ndp_device
		self.membus = membus

	@overrides(AbstractClassicCacheHierarchy)
	def get_mem_side_port(self) -> Port:
		return self.membus.mem_side_ports

	@overrides(AbstractClassicCacheHierarchy)
	def get_cpu_side_port(self) -> Port:
		return self.membus.cpu_side_ports

	@overrides(AbstractCacheHierarchy)
	def incorporate_cache(self, board: AbstractBoard) -> None:

		if board.get_processor().get_num_cores() > 1:
			raise Exception(
				"Only one CPU core can be used with NDPCompatibleCacheHierarchy."
			)

		# Set up the system port for functional access from the simulator.
		board.connect_system_port(self.membus.cpu_side_ports)

		for cntr in board.get_memory().get_memory_controllers():
			cntr.port = self.membus.mem_side_ports

		self.l1icache = L1ICache(size=self._l1i_size)
		self.l1dcache = L1DCache(size=self._l1d_size)
		self.l2bus = L2XBar()
		self.l2cache = L2Cache(size=self._l2_size)
		
		# ITLB Page walk cache
		self.iptw_cache = MMUCache(size='8KiB')

		# DTLB Page walk cache
		self.dptw_cache = MMUCache(size='8KiB')

		if board.has_coherent_io():
			self._setup_io_cache(board)

		cpu = board.get_processor().get_cores()[0]

		cpu.connect_icache(self.l1icache.cpu_side)

		# Connect NDP device to CPU and L1DCache to NDP device.
		cpu.connect_dcache(self.ndp_device.cpu_side)
		self.ndp_device.mem_side = self.l1dcache.cpu_side

		# Connect NDP device dma_port to a memory bus
		self.ndp_device.dma_port = self.l2bus.cpu_side_ports

		self.l1icache.mem_side = self.l2bus.cpu_side_ports
		self.l1dcache.mem_side = self.l2bus.cpu_side_ports
		self.iptw_cache.mem_side = self.l2bus.cpu_side_ports
		self.dptw_cache.mem_side = self.l2bus.cpu_side_ports

		self.l2bus.mem_side_ports = self.l2cache.cpu_side

		self.membus.cpu_side_ports = self.l2cache.mem_side

		cpu.connect_walker_ports(
			self.iptw_cache.cpu_side, self.dptw_cache.cpu_side
		)

		if get_runtime_isa() == ISA.X86:
			int_req_port = self.membus.mem_side_ports
			int_resp_port = self.membus.cpu_side_ports
			cpu.connect_interrupt(int_req_port, int_resp_port)
		else:
			cpu.connect_interrupt()

	def _setup_io_cache(self, board: AbstractBoard) -> None:
		"""Create a cache for coherent I/O connections"""
		self.iocache = Cache(
			assoc=8,
			tag_latency=50,
			data_latency=50,
			response_latency=50,
			mshrs=20,
			size="1kB",
			tgts_per_mshr=12,
			addr_ranges=board.mem_ranges,
		)
		self.iocache.mem_side = self.membus.cpu_side_ports
		self.iocache.cpu_side = board.get_mem_side_coherent_io_port()
