#ifndef __NDP_HH__
#define __NDP_HH__

#include "mem/packet_access.hh"

#include "sim/system.hh"
#include "sim/clocked_object.hh"
#include "mem/port.hh"

#include "params/NDP.hh"
#include "debug/NDPPI.hh"
#include "debug/NDPMem.hh"

namespace gem5
{

	class NDP : public ClockedObject
	{
	private:

		class CPUSidePort : public ResponsePort
		{
		private:
			NDP *owner;

		public:
			CPUSidePort(const std::string& name, NDP *owner) :
			ResponsePort(name, owner), owner(owner)
			{ }

			AddrRangeList getAddrRanges() const override;

		protected:
			Tick recvAtomic(PacketPtr pkt) override
			{ panic("recvAtomic unimpl."); }

			void recvFunctional(PacketPtr pkt) override;

			bool recvTimingReq(PacketPtr pkt) override;

			void recvRespRetry() override;
		};

		class MemSidePort : public RequestPort
		{
		private:
			NDP *owner;

		public:
			MemSidePort(const std::string& name, NDP *owner) :
			RequestPort(name, owner), owner(owner)
			{ }

		protected:
			bool recvTimingResp(PacketPtr pkt) override;

			void recvReqRetry() override;

			void recvRangeChange() override;
		};

		class BurstRequest
		{
		private:

			class SubRequest
			{
			private:

				AddrRange addrRange;
				uint8_t *dataPtr;

			public:

				SubRequest(AddrRange addrRange, uint8_t *dataPtr) :
				addrRange(addrRange), dataPtr(dataPtr)
				{ }

				AddrRange getAddrRange()
				{ return addrRange; };

				uint8_t *getDataPtr()
				{ return dataPtr; };
			};

			AddrRange addrRange;
			uint8_t *dataPtr;
			bool writeRequest;
			std::list<SubRequest *> pendingSubRequests;

		public:

			BurstRequest(AddrRange addrRange, uint8_t *dataPtr, bool writeRequest) :
			addrRange(addrRange), dataPtr(dataPtr), writeRequest(writeRequest)
			{ }

			void addSubRequest(AddrRange addrRange, uint8_t *dataPtr)
			{ pendingSubRequests.push_back(new SubRequest(addrRange, dataPtr)); };
			
			uint8_t *popPendingSubRequest(AddrRange addrRange);
			
			bool requestComplete()
			{ return pendingSubRequests.empty(); };

			Addr getAddr()
			{ return addrRange.start(); };

			uint8_t *getDataPtr()
			{ return dataPtr; };

			size_t getSize()
			{ return addrRange.size(); };

			bool isWrite()
			{ return writeRequest; };

			int countPendingSubRequests()
			{ return pendingSubRequests.size(); }
		};

		AddrRangeList getAddrRanges() const;

		void sendRangeChange() const;

		bool handleRequest(PacketPtr pkt);

		bool memCallback(PacketPtr pkt);

		CPUSidePort cpuPort;
		MemSidePort memPort, dmaPort;
		AddrRange ndpCtrl, ndpData;
		bool dmaActv;
		uint64_t maxRSze, maxReqs, flyReqs;
		std::list<BurstRequest *> pendingRequests;
		std::list<PacketPtr> pendingReqPackets, pendingRespPackets;

	protected:

		virtual uint64_t readPI(uint64_t ridx)
		{ panic("readPI must be implemented in subclass of NDP."); };

		virtual void writePI(uint64_t ridx, uint64_t data)
		{ panic("writePI must be implemented in subclass of NDP."); };

		void accessMemory(Addr addr, size_t size, bool write, uint8_t *data);

		virtual void recvData(Addr addr, uint8_t *data, size_t size)
		{ panic("recvData must be implemented in subclass of NDP."); };

	public:

		NDP(const NDPParams &params);

		Port &getPort(const std::string &if_name,
			PortID idx=InvalidPortID) override;

		void sendData();

	};

} // namespace gem5

#endif // __NDP_HH__
