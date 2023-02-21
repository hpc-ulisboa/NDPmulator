#ifndef __NDPDevA_HH__
#define __NDPDevA_HH__

#include "ndp/ndp.hh"

#include "params/NDPDevA.hh"
#include "debug/NDPDevA.hh"
#include "debug/NDPDevAPI.hh"
#include "debug/NDPDevAMem.hh"

namespace gem5
{
	class NDPDevA : public NDP
	{
	private:

		uint64_t scaleFactor;

		uint64_t pi_addr_data = 0;
	    uint64_t pi_data_size = 0;
	    uint64_t pi_data_skey = 0;
	    uint64_t pi_cmmd_code = 0;
	    uint64_t pi_stat_rgst = 1;
	    uint64_t pi_last_rslt = 0;

	    bool has_operands = false;
	    uint64_t *operands;

	    uint64_t compare_n_hit(uint64_t *data, uint64_t size, uint64_t skey);

		uint64_t compare_n_count(uint64_t *data, uint64_t size, uint64_t skey);

		uint64_t compare_n_max(uint64_t *data, uint64_t size);

	    void process_fsm();

	public:

		NDPDevA(const NDPDevAParams &params);

		uint64_t readPI(uint64_t ridx) override;

		void writePI(uint64_t ridx, uint64_t data) override;

		void recvData(Addr addr, uint8_t *data, size_t size) override;

	};

}

#endif //__NDPDevA_HH__
