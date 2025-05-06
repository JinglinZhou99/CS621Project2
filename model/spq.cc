#include "spq.h"
#include "ns3/string.h"
#include <fstream>
#include <sstream>

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED(SPQ);

TypeId SPQ::GetTypeId(void) {
    static TypeId tid = TypeId("ns3::SPQ")
        .SetParent<DiffServ>()
        .SetGroupName("Network")
        .AddConstructor<SPQ>();
    return tid;
}

SPQ::SPQ() {
}

SPQ::~SPQ() {
    // No manual cleanup needed; Ptr<TrafficClass> handles memory management
}

std::pair<uint32_t, Ptr<const Packet>> SPQ::Schedule(void) {
    // Keep trying until a packet is scheduled or all queues remain empty
    while (true) {
        uint32_t highestPriority = 0;
        int selectedQueue = -1;

        for (uint32_t i = 0; i < q_class.size(); ++i) {
            if (!q_class[i]->IsEmpty() && q_class[i]->GetPriorityLevel() >= highestPriority) {
                highestPriority = q_class[i]->GetPriorityLevel();
                selectedQueue = i;
            }
        }

        if (selectedQueue >= 0) {
            Ptr<TrafficClass> queue = q_class[selectedQueue];
            Ptr<Packet> packet = queue->Dequeue();
            if (packet) {
                std::cout << "SPQ::Schedule: Scheduled packet from queue " << selectedQueue 
                          << ", priority=" << highestPriority << ", size=" << packet->GetSize() << std::endl;
                return {static_cast<uint32_t>(selectedQueue), packet};
            } else {
                std::cout << "SPQ::Schedule: Dequeue failed for queue " << selectedQueue << std::endl;
            }
        } else {
            // All queues are empty, safe to return nullptr
            std::cout << "SPQ::Schedule: No packet scheduled (all queues empty)" << std::endl;
            break;
        }
    }
    return {q_class.size(), nullptr};
}

uint32_t SPQ::Classify(Ptr<Packet> p) {
    for (uint32_t i = 0; i < q_class.size(); ++i) {
        if (q_class[i]->match(p)) {
            std::cout << "SPQ::Classify: Packet matched queue " << i << std::endl;
            return i;
        }
    }
    // Look for a default queue
    for (uint32_t i = 0; i < q_class.size(); ++i) {
        if (q_class[i]->GetDefault()) {
            std::cout << "SPQ::Classify: Packet assigned to default queue " << i << std::endl;
            return i;
        }
    }
    std::cout << "SPQ::Classify: Packet dropped (no default queue)" << std::endl;
    return q_class.size(); // No match, return invalid index
}

bool SPQ::ReadConfigFile(std::string filename) {
    std::ifstream file(filename);
    std::string line;

    if (!file.is_open()) {
        std::cerr << "Failed to open SPQ config file: " << filename << std::endl;
        return false;
    }

    while (std::getline(file, line)) {
        ParseConfigLine(line);
    }
    file.close();

    // Add a default queue for unmatched packets if none exists
    bool hasDefault = false;
    for (Ptr<TrafficClass> tc : q_class) {
        if (tc->GetDefault()) {
            hasDefault = true;
            break;
        }
    }
    if (!hasDefault) {
        Ptr<TrafficClass> defaultQueue = CreateObject<TrafficClass>();
        defaultQueue->SetDefault(true);
        defaultQueue->SetPriorityLevel(0); // Lowest priority
        defaultQueue->SetMaxPackets(1000);
        AddQueue(defaultQueue);
        std::cout << "SPQ::ReadConfigFile: Added default queue for unmatched packets" << std::endl;
    }

    std::cout << "SPQ::ReadConfigFile: Configured " << q_class.size() << " queues" << std::endl;
    return true;
}

void SPQ::ParseConfigLine(const std::string& line) {
    std::istringstream iss(line);
    std::string token;
    iss >> token;

    if (token == "queue") {
        uint32_t queueId, priority, maxPackets;
        if (iss >> queueId >> priority >> maxPackets) {
            Ptr<TrafficClass> tc = CreateObject<TrafficClass>();
            tc->SetPriorityLevel(priority);
            tc->SetMaxPackets(maxPackets);
            AddQueue(tc);
            std::cout << "SPQ::ParseConfigLine: Added queue " << queueId << ", priority=" << priority 
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
            } else if (filterType == "protocol") {
                filter->AddElement(new ProtocolNumber(std::stoi(value)));
            }
            if (queueId < q_class.size()) {
                q_class[queueId]->AddFilter(filter);
                std::cout << "SPQ::ParseConfigLine: Added filter to queue " << queueId 
                          << ", type=" << filterType << ", value=" << value << std::endl;
            } else {
                std::cerr << "SPQ::ParseConfigLine: Invalid queueId " << queueId << " for filter" << std::endl;
                delete filter; // Avoid memory leak if queueId is invalid
            }
        }
    }
}

} // namespace ns3