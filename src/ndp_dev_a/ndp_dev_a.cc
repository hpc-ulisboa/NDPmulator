#include "ndp_dev_a/ndp_dev_a.hh"

namespace gem5
{
	NDPDevA::NDPDevA(const NDPDevAParams &params) :
	NDP(params)
	{

	}

	uint64_t 
	NDPDevA::readPI(uint64_t ridx)
	{
		switch (ridx)
		{
		case 5: 
			// DPRINTF(NDPDevAPI, "NDP device PI: r[%lu] = %lu\n", ridx, pi_stat_rgst);
			return pi_stat_rgst;
	    case 6: 
	    	// DPRINTF(NDPDevAPI, "NDP device PI: r[%lu] = %lu\n", ridx, pi_last_rslt);
	    	return pi_last_rslt;
	    default:
	    	panic("NDPDevA does not have readable r[%lu] register!\n", ridx);
		}
	}

	void 
	NDPDevA::writePI(uint64_t ridx, uint64_t data)
	{
		DPRINTF(NDPDevAPI, "NDP device PI: %lu -> r[%lu]\n", data, ridx);

		if (!pi_stat_rgst)
		{
			panic("Tried to started workload when previous one is not finished!\n");
		}

		switch (ridx)
		{
		case 0: pi_addr_data = data; break;
	    case 1: pi_data_size = data; break;
	    case 2: pi_data_skey = data; break;
	    case 3: pi_cmmd_code = data; break;
	    case 4:
    		DPRINTF(
    			NDPDevAPI,
    			"NDPDevA started processing...\n"
    			"===== pi_addr_data = %lu\n"
	    		"===== pi_data_size = %lu\n"
	    		"===== pi_data_skey = %lu\n"
	    		"===== pi_cmmd_code = %lu\n"
	    		"===== pi_stat_rgst = %lu\n"
	    		"===== pi_last_rslt = %lu\n",
	    		pi_addr_data,
	    		pi_data_size,
	    		pi_data_skey,
	    		pi_cmmd_code,
	    		pi_stat_rgst,
	    		pi_last_rslt
    		);
    		pi_stat_rgst = 0;
    		scaleFactor = data;
    		process_fsm();
	    	break;
	    default:
	    	panic("NDPDevA does not have writable r[%lu] register!\n", ridx);
		}
	}

	void 
	NDPDevA::recvData(Addr addr, uint8_t *data, size_t size)
	{
		DPRINTF(NDPDevAMem, "NDPDevA received %u bytes from %p\n", size, addr);

		has_operands = true;

		process_fsm();
	}

	uint64_t
	NDPDevA::compare_n_hit(uint64_t *data, uint64_t size, uint64_t skey)
	{
	    for (int i = 0; i < size; ++i)
	    {
	        if (data[i] == skey)
	        {
	            pi_last_rslt = 1;

	            return i;
	        }
	    }

	    return size;
	}

	uint64_t
	NDPDevA::compare_n_count(uint64_t *data, uint64_t size, uint64_t skey)
	{
	    uint64_t n = 0;

	    for (int i = 0; i < size; ++i)
	    {
	        if (data[i] == skey)
	        {
	            n++;
	        }
	    }

	    pi_last_rslt = n;
	    
	    return size;
	}

	uint64_t
	NDPDevA::compare_n_max(uint64_t *data, uint64_t size)
	{
	    uint64_t max = data[0];
	    for (int i = 1; i < size; ++i)
	    {
	        if (data[i] > max)
	        {
	            max = data[i];
	        }
	    }

	    pi_last_rslt = max;

	    return size;
	}

	void
	NDPDevA::process_fsm()
	{
		// Retrive operands from memory
		if (!has_operands)
		{
			DPRINTF(
				NDPDevA,
				"Retrieving operads from memory: %lu bytes from %p\n",
				pi_data_size * sizeof(uint64_t),
				pi_addr_data
			);
			operands = new uint64_t[pi_data_size];
			accessMemory(
				Addr(pi_addr_data),
				pi_data_size * sizeof(uint64_t),
				false,
				(uint8_t *) operands
			);
		}
		else
		{
			uint64_t cycles;

			switch (pi_cmmd_code)
			{
			case 0: 
				cycles = compare_n_hit(operands, pi_data_size, pi_data_skey);
				break;
			case 1:
				cycles = compare_n_count(operands, pi_data_size, pi_data_skey);
				break;
			case 2:
				cycles = compare_n_max(operands, pi_data_size);
				break;
			default:
				panic("Invalid command was issued to NDPDevA!\n");
			}

			schedule(
				new EventFunctionWrapper(
					[this]
					{
						pi_stat_rgst = 1;
						delete operands;
						has_operands = false;
					},
					name() + ".writeBackResultsEvent",
					true
				),
				clockEdge(Cycles(cycles * scaleFactor))
			);
		}
	}

} // namespace gem5
