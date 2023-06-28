import m5
from m5.objects import *
from caches import *

# Program to execute
binary = 'tests/test-progs/ndp/ndp_dev_a/bench_se'

# Simulation system
system = System()

# Clock configuration
system.clk_domain = SrcClockDomain()
system.clk_domain.clock = '2GHz'
system.clk_domain.voltage_domain = VoltageDomain()

# Memory configuration
system.mem_mode = 'timing'
system.mem_ranges = [AddrRange('2GB')]

# Create CPU
system.cpu = TimingSimpleCPU()

# Create NDP device
system.ndp_accel = NDPDevA(
    ndp_ctrl=('0x40000000', '0x40001000'),
    ndp_data=('0x40001000', '0x80000000'),
    max_rsze=0x40
)

# Create L1 caches
system.cpu.icache = L1Cache(assoc=4)
system.cpu.dcache = L1Cache()
system.cpu.dcache.addr_ranges = system.mem_ranges

# Connect L1I cache to the CPU
system.cpu.icache.cpu_side = system.cpu.icache_port

# Connect NDP device to the CPU and L1D to NDP device
system.ndp_accel.cpu_side = system.cpu.dcache_port
system.cpu.dcache.cpu_side = system.ndp_accel.mem_side

# Create L1 to L2 interconnect
system.l2bus = L2XBar()

# Link L1 with interconnect
system.cpu.icache.mem_side = system.l2bus.cpu_side_ports
system.cpu.dcache.mem_side = system.l2bus.cpu_side_ports

# Create L2 cache
system.l2cache = L2Cache()

# Link L2 cache with L1 to L2 interconnect
system.l2cache.cpu_side = system.l2bus.mem_side_ports

# Create memory bus
system.membus = SystemXBar()

# Link L2 with interconnect
system.l2cache.mem_side = system.membus.cpu_side_ports

# Connect NDP device to DDR
system.ndp_accel.dma_port = system.membus.cpu_side_ports

# Create interrupt controller
system.cpu.createInterruptController()

# Connect interruptions and IO with memory bus (required by X86)
if m5.defines.buildEnv['USE_X86_ISA']:
    system.cpu.interrupts[0].pio = system.membus.mem_side_ports
    system.cpu.interrupts[0].int_master = system.membus.cpu_side_ports
    system.cpu.interrupts[0].int_slave = system.membus.mem_side_ports

# Connect special port to allow read/write memory
system.system_port = system.membus.cpu_side_ports

# Create a DDR3 memory controller
system.mem_ctrl = MemCtrl()
system.mem_ctrl.dram = DDR3_1600_8x8()
system.mem_ctrl.dram.range = system.mem_ranges[0]
system.mem_ctrl.port = system.membus.mem_side_ports

system.workload = SEWorkload.init_compatible(binary)

# Create a process for a the application
process = Process()

# Command is a list which begins with the executable (like argv)
process.cmd = [binary]

# Set the cpu to use the process as its workload and create thread contexts
system.cpu.workload = process
system.cpu.createThreads()

# Set up the root SimObject and start the simulation
root = Root(full_system = False, system = system)

# Instantiate all of the objects we've created above
m5.instantiate()

# Dedicate upper 1GB to NDP device
system.cpu.workload[0].map(0x40000000, 0x40000000, 0x40000000, cacheable=False)

print("========== Beginning simulation ==========")
exit_event = m5.simulate()

print('Exiting @ tick {} because {}' .format(m5.curTick(), exit_event.getCause()))
