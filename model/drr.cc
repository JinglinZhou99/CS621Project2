#include "drr.h"
#include "ns3/string.h"
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
                Ptr<Packet> packet = queue->Dequeue();
                if (packet) {
                    deficits[currentQueue] -= packet->GetSize();
                    currentQueue = (currentQueue + 1) % q_class.size();
                    std::cout << "DRR::Schedule: Scheduled packet from queue " << currentQueue 
                              << ", size=" << packet->GetSize() << ", new deficit=" << deficits[currentQueue] << std::endl;
                    return {currentQueue, packet};
                } else {
                    std::cout << "DRR::Schedule: Dequeue failed for queue " << currentQueue << std::endl;
                }
            } else {
                std::cout << "DRR::Schedule: Queue " << currentQueue << " is empty" << std::endl;
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

    // Remove the default queue addition
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
                filter->AddElement(new DstPortNumber(std::stoi(value))); // Updated to handle dst_port
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