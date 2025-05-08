#include "drr.h"
#include "ns3/string.h"
#include "ns3/simulator.h"
#include <fstream>
#include <sstream>

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED(DRR);

TypeId DRR::GetTypeId(void) {
    static TypeId tid = TypeId("ns3::DRR")
        .SetParent<DiffServ>()
        .SetGroupName("Network")
        .AddConstructor<DRR>();
    return tid;
}

DRR::DRR() : currentQueue(0) {
}

DRR::~DRR() {
}

std::pair<uint32_t, Ptr<const Packet>> DRR::Schedule(void) {
    if (q_class.empty()) {
        std::cout << "DRR::Schedule: No queues available" << std::endl;
        return {q_class.size(), nullptr};
    }

    bool anyQueueNonEmpty = true;
    while (anyQueueNonEmpty) {
        anyQueueNonEmpty = false;
        uint32_t startQueue = currentQueue;
        std::cout << "DRR::Schedule: Starting at queue " << startQueue << std::endl;
        do {
            Ptr<TrafficClass> queue = q_class[currentQueue];
            if (!queue->IsEmpty()) {
                anyQueueNonEmpty = true;
                Ptr<const Packet> peekedPacket = queue->Peek();
                if (!peekedPacket) {
                    std::cout << "DRR::Schedule: Peek returned nullptr for queue " << currentQueue << std::endl;
                    currentQueue = (currentQueue + 1) % q_class.size();
                    continue;
                }
                deficits[currentQueue] += queue->GetWeight();
                std::cout << "DRR::Schedule: Added quantum for queue " << currentQueue 
                          << ", new deficit=" << deficits[currentQueue] << std::endl;

                if (deficits[currentQueue] < peekedPacket->GetSize()) {
                    currentQueue = (currentQueue + 1) % q_class.size();
                    continue;
                }
                deficits[currentQueue] -= peekedPacket->GetSize();
                uint32_t scheduledQueue = currentQueue;

                Ptr<TrafficClass> nextQ = q_class[scheduledQueue];
                uint32_t nextSize = nextQ->IsEmpty() ? 0 : nextQ->Peek()->GetSize();
                if (deficits[scheduledQueue] < nextSize) {
                    currentQueue = (scheduledQueue + 1) % q_class.size();
                }

                std::cout << "DRR::Schedule: Scheduled packet from queue " << scheduledQueue 
                          << ", size=" << peekedPacket->GetSize() 
                          << ", leftover deficit=" << deficits[scheduledQueue] 
                          << ", weight=" << queue->GetWeight() << std::endl;
                return {scheduledQueue, peekedPacket};
            } else {
                deficits[currentQueue] = 0;
                std::cout << "DRR::Schedule: Queue " << currentQueue << " is empty, deficit reset to 0" << std::endl;
            }
            currentQueue = (currentQueue + 1) % q_class.size();
        } while (currentQueue != startQueue);

        if (!anyQueueNonEmpty) {
            std::cout << "DRR::Schedule: All queues empty after full round" << std::endl;
            break;
        }
    }

    std::cout << "DRR::Schedule: No packet scheduled (all queues empty)" << std::endl;
    return {q_class.size(), nullptr};
}

uint32_t DRR::Classify(Ptr<Packet> p) {
    for (uint32_t i = 0; i < q_class.size(); ++i) {
        if (q_class[i]->match(p)) {
            std::cout << "DRR::Classify: Packet matched queue " << i << std::endl;
            return i;
        }
    }
    std::cout << "DRR::Classify: Packet dropped (no matching queue)" << std::endl;
    return q_class.size();
}

bool DRR::ReadConfigFile(std::string filename) {
    std::ifstream file(filename);
    std::string line;

    if (!file.is_open()) {
        std::cerr << "Failed to open DRR config file: " << filename << std::endl;
        return false;
    }

    while (std::getline(file, line)) {
        ParseConfigLine(line);
    }
    file.close();

    deficits.resize(q_class.size(), 0);
    std::cout << "DRR::ReadConfigFile: Configured " << q_class.size() << " queues" << std::endl;
    return true;
}

void DRR::ParseConfigLine(const std::string& line) {
    std::istringstream iss(line);
    std::string token;
    iss >> token;

    if (token == "queue") {
        uint32_t queueId, quantum, maxPackets;
        if (iss >> queueId >> quantum >> maxPackets) {
            Ptr<TrafficClass> tc = CreateObject<TrafficClass>();
            tc->SetWeight(quantum);
            tc->SetMaxPackets(maxPackets);
            AddQueue(tc);
            std::cout << "DRR::ParseConfigLine: Added queue " << queueId << ", quantum=" << quantum 
                      << ", maxPackets=" << maxPackets << std::endl;
        }
    } else if (token == "filter") {
        uint32_t queueId;
        std::string filterType, value;
        if (iss >> queueId >> filterType >> value) {
            Filter* filter = new Filter();
            if (filterType == "src_ip") {
                filter->AddElement(new SrcIPAddress(Ipv4Address(value.c_str())));
            } else if (filterType == "dst_ip") {
                filter->AddElement(new DstIPAddress(Ipv4Address(value.c_str())));
            } else if (filterType == "src_port") {
                filter->AddElement(new SrcPortNumber(std::stoi(value)));
            } else if (filterType == "dst_port") {
                filter->AddElement(new DstPortNumber(std::stoi(value)));
                std::cout << "DRR::ParseConfigLine: Added filter for dst_port=" << value << " to queue " << queueId << std::endl;
            } else if (filterType == "protocol") {
                filter->AddElement(new ProtocolNumber(std::stoi(value)));
            }
            if (queueId < q_class.size()) {
                q_class[queueId]->AddFilter(filter);
                std::cout << "DRR::ParseConfigLine: Added filter to queue " << queueId 
                          << ", type=" << filterType << ", value=" << value << std::endl;
            } else {
                std::cerr << "DRR::ParseConfigLine: Invalid queueId " << queueId << " for filter" << std::endl;
                delete filter;
            }
        }
    }
}

} // namespace ns3
