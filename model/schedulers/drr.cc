#include "drr.h"
#include "ns3/string.h"
#include "ns3/simulator.h" // Added to use Simulator::Now()
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
    // No manual cleanup needed; Ptr<TrafficClass> handles memory management
}
/**
std::pair<uint32_t, Ptr<const Packet>> DRR::Schedule(void) {
    if (q_class.empty()) {
        std::cout << "DRR::Schedule: No queues available" << std::endl;
        return {q_class.size(), nullptr};
    }

    // Keep trying until a packet is scheduled or all queues are empty
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
                // Ensure deficit is sufficient by adding quantum until it is
                while (deficits[currentQueue] < peekedPacket->GetSize()) {
                    deficits[currentQueue] += queue->GetWeight();
                    std::cout << "DRR::Schedule: Insufficient deficit for queue " << currentQueue 
                              << ", added quantum, new deficit=" << deficits[currentQueue] << std::endl;
                }
                deficits[currentQueue] -= peekedPacket->GetSize();
                uint32_t scheduledQueue = currentQueue;
                currentQueue = (currentQueue + 1) % q_class.size();
                std::cout << "DRR::Schedule: Scheduled packet from queue " << scheduledQueue 
                          << ", size=" << peekedPacket->GetSize() << ", new deficit=" << deficits[scheduledQueue] 
                          << ", weight=" << queue->GetWeight() << std::endl;
                return {scheduledQueue, peekedPacket};
            } else {
                // Reset deficit for empty queues to ensure fairness
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
*/
std::pair<uint32_t, Ptr<const Packet>> DRR::Schedule(void) {
    if (q_class.empty()) {
        std::cout << "DRR::Schedule: No queues available" << std::endl;
        return {q_class.size(), nullptr};
    }

    // Keep trying until a packet is scheduled or all queues are empty
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
                // —— DRR 正式逻辑 ——  
                // 1) 本次轮到就先加一次量子
                deficits[currentQueue] += queue->GetWeight();
                std::cout << "DRR::Schedule: Added quantum for queue " << currentQueue 
                          << ", new deficit=" << deficits[currentQueue] << std::endl;

                // 2) 只有当累计的 deficit 够发一个包时才真正 schedule
                if (deficits[currentQueue] < peekedPacket->GetSize()) {
                    // 不够就跳到下一个队列，继续找
                    currentQueue = (currentQueue + 1) % q_class.size();
                    continue;
                }
                // 够了就扣掉包大小
                deficits[currentQueue] -= peekedPacket->GetSize();
                uint32_t scheduledQueue = currentQueue;

                // 3) 如果扣完后已经不足以再发下一个包，就下次切队
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
                // Reset deficit for empty queues to ensure fairness
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
    // Drop packets that don't match any queue (no default queue usage)
    std::cout << "DRR::Classify: Packet dropped (no matching queue)" << std::endl;
    return q_class.size(); // No match, return invalid index to drop the packet
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

    deficits.resize(q_class.size(), 0); // Initialize deficits
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
                delete filter; // Avoid memory leak if queueId is invalid
            }
        }
    }
}

} // namespace ns3
