#include "ndp/ndp.hh"

namespace gem5
{

	NDP::NDP(const NDPParams &params) :
	ClockedObject(params),
	cpuPort(params.name + ".cpu_side", this),
	memPort(params.name + ".mem_side", this),
	dmaPort(params.name + ".dma_port", this),
	ndpRnge(params.ndp_rnge),
	maxRSze(params.max_rsze)
	{

	}

	Port &
	NDP::getPort(const std::string &if_name, PortID idx)
	{
		if (if_name == "mem_side")
			return memPort;
		else if (if_name == "cpu_side")
			return cpuPort;
		else if (if_name == "dma_port")
			return dmaPort;
		else
			return ClockedObject::getPort(if_name, idx);
	}

	AddrRangeList
	NDP::CPUSidePort::getAddrRanges() const
	{
		return owner->getAddrRanges();
	}

	void
	NDP::CPUSidePort::recvFunctional(PacketPtr pkt)
	{
		owner->memPort.sendFunctional(pkt);
	}

	bool
	NDP::CPUSidePort::recvTimingReq(PacketPtr pkt)
	{
		if (owner->ndpRnge.contains(pkt->getAddr()))
			return owner->handleRequest(pkt);
		else
			return owner->memPort.sendTimingReq(pkt);
	}

	void 
	NDP::CPUSidePort::recvRespRetry()
	{
		panic("CPUSidePort::recvRespRetry not implemented!\n");
	}

	bool
	NDP::handleRequest(PacketPtr pkt)
	{
		uint64_t data, ridx = (pkt->getAddr() - ndpRnge.start()) / sizeof(uint64_t);

		if (pkt->isRead())
		{
			data = readPI(ridx);
			pkt->setRaw<uint64_t>(data);

			DPRINTF(NDPPI, "NDP PI read request: %p (%u)\n", ridx, data);
		}
		else if (pkt->isWrite())
		{
			data = pkt->getRaw<uint64_t>();
			writePI(ridx, data);

			DPRINTF(NDPPI, "NDP PI write request: %p (%u)\n", ridx, data);
		}
		else
		{
			panic("NDP received packet that is not read nor write!\n");
		}

		pkt->makeResponse();
		schedule(
			new EventFunctionWrapper(
				[this, pkt]
				{
					panic_if(
						!cpuPort.sendTimingResp(pkt),
						"CPU did not accept response from NDP device!\n"
					);
				},
				name() + ".sendTimingRespEvent",
				true
			),
			// Reading or writing the PI always takes one cycle
			clockEdge(Cycles(1))
		);
		
		return true;
	}

	void
	NDP::sendData()
	{
		PacketPtr pkt = pendingReqPackets.front();

		DPRINTF(
			NDPMem,
			"NDP device %s %lu bytes %s %p\n",
			pkt->isWrite() ? "writing" : "reading",
			pkt->getSize(),
			pkt->isWrite() ? "to" : "from",
			pkt->getAddr()
		);

		// Send oldest pending packet
		if (dmaPort.sendTimingReq(pkt))
		{
			// Remove pending packet from list
			pendingReqPackets.pop_front();

			// Schedule next packet
			if (!pendingReqPackets.empty())
			{
				schedule(
					new EventFunctionWrapper(
						[this, pkt]
						{
							sendData();
						},
						name() + ".NDPDevASendMemoryRequest",
						true
					),
					// Send the next packet on the next cycle
					clockEdge(Cycles(1))
				);
			}
		}
	}

	void
	NDP::accessMemory(Addr addr, size_t size, bool write, uint8_t *data)
	{
		// Create new burst request
		BurstRequest *newRequest = new BurstRequest(
			AddrRange(addr, addr + size),
			data,
			write
		);

		uint64_t nRequests = 0;
		for (Addr saddr = addr; saddr < addr + size; saddr += maxRSze, ++nRequests)
		{
			// Calculate size of new request
			size_t ssize = nRequests * maxRSze + maxRSze > size ?
				size % maxRSze :
				maxRSze;

			// Add offset to original data pointer
			uint8_t *sdata = data + nRequests * maxRSze;

			// Add sub request to pending list
			newRequest->addSubRequest(AddrRange(saddr, saddr + ssize), sdata);

			PacketPtr pkt = new Packet(
				std::make_shared<Request> (saddr, ssize, 0, 0),	// Create request
				write ? MemCmd::WriteReq : MemCmd::ReadReq,		// Read or write
				ssize
			);

			// Allocate and initialize packet buffer
			uint8_t *sdataBuffer = new uint8_t[ssize];
			
			if (write)
			{
				for (int i = 0; i < ssize; i++)
				{
					sdataBuffer[i] = sdata[i];
				}
			}

			// Delete packet buffer when deleting packet
			pkt->dataDynamic(sdataBuffer);

			pendingReqPackets.push_back(pkt);
		}

		pendingRequests.push_back(newRequest);

		schedule(
			new EventFunctionWrapper(
				[this]
				{
					sendData();
				},
				name() + ".NDPDevASendMemoryRequest",
				true
			),
			// Send the next packet on the next cycle
			clockEdge(Cycles(1))
		);
	}

	bool
	NDP::memCallback(PacketPtr pkt)
	{
		DPRINTF(
			NDPMem,
			"NDP device received %s response (%p, %lu bytes)\n",
			pkt->isWrite() ? "write" : "read",
			pkt->getAddr(),
			pkt->getSize()
		);

		// Iterate all pending requests
		for (auto it = pendingRequests.begin(); it != pendingRequests.end(); ++it)
		{
			// Pending request is the same type as pkt
			if ((*it)->isWrite() == pkt->isWrite())
			{
				// Get original data pointer
				uint8_t *dataPtr = (*it)->popPendingSubRequest(pkt->getAddrRange());
				
				// The subrequest belonged to this request
				if (dataPtr)
				{
					// Copy data to original pointer if is read
					if (pkt->isRead())
					{
						pkt->writeDataToBlock(dataPtr, pkt->getSize());
					}

					// Check if request is complete
					// If so delete request and return data
					if ((*it)->requestComplete())
					{
						DPRINTF(
							NDPMem,
							"Completed %s request (%p, %lu bytes)\n",
							pkt->isWrite() ? "write" : "read",
							pkt->getAddr(),
							pkt->getSize()
						);

						recvData(
							(*it)->getAddr(),
							(*it)->getDataPtr(),
							(*it)->getSize()
						);

						delete *it;
						pendingRequests.remove(*it);
						break;	// One subrequest belongs only to one request
					}
				}
			}
		}

		// Finally delete the packet
		delete pkt;

		return true;
	}

	bool
	NDP::MemSidePort::recvTimingResp(PacketPtr pkt)
	{
		// Request was made by the CPU through mem_side
		if (name().find("mem_side") != std::string::npos)
		{
			panic_if(
				!owner->cpuPort.sendTimingResp(pkt),
				"CPU did not accept response from memory!\n"
			);
			return true;
		}
		else
		{
			return owner->memCallback(pkt);
		}
	}

	void
	NDP::MemSidePort::recvReqRetry()
	{
		// Request was made by the CPU through mem_side
		if (name().find("mem_side") != std::string::npos)
			owner->cpuPort.sendRetryReq();
		else
			owner->sendData();
	}

	void
	NDP::MemSidePort::recvRangeChange()
	{
		owner->sendRangeChange();
	}

	AddrRangeList
	NDP::getAddrRanges() const
	{	
		return memPort.getAddrRanges();
	}

	void
	NDP::sendRangeChange() const
	{
		cpuPort.sendRangeChange();
	}
	
	uint8_t * 
	NDP::BurstRequest::popPendingSubRequest(AddrRange addrRange)
	{
		for (auto it = pendingSubRequests.begin(); it != pendingSubRequests.end(); ++it)
		{
			// Subrequest matches addrRange
			if ((*it)->getAddrRange() == addrRange)
			{
				uint8_t *rDataPtr =
					(*it)->getDataPtr();		// Retrieve original pointer
				delete *it;						// Delete subrequest
				pendingSubRequests.remove(*it);	// Remove subrequest from pending requests
				return rDataPtr;				// Return original pointer
			}
		}

		return NULL;
	}

} // namespace gem5