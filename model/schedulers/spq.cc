#include "spq.h"
#include "ns3/string.h"
#include "ns3/simulator.h"
#include <fstream>
#include <sstream>

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED(SPQ);

/**
 * @brief Returns the TypeId for SPQ.
 *
 * Registers the SPQ class with the ns-3 object system, setting it as a child of DiffServ
 * and assigning it to the "Network" group.
 *
 * @return The TypeId of the SPQ class.
 */
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
}

/**
 * @brief Schedules a packet for dequeuing using the Strict Priority Queue algorithm.
 *
 * Selects the non-empty queue with the highest priority level and returns its peeked packet.
 * Logs the scheduling decision, including queue index, priority, packet size, and simulation time.
 *
 * @return A pair containing the index of the scheduled queue and a pointer to the peeked packet,
 *         or {q_class.size(), nullptr} if no packet is scheduled.
 */
std::pair<uint32_t, Ptr<const Packet>> SPQ::Schedule(void) {
    int selectedQueue = -1;
    int maxPriority = -1;

    for (uint32_t i = 0; i < q_class.size(); ++i) {
        int priority = q_class[i]->GetPriorityLevel();
        if (!q_class[i]->IsEmpty() && priority > maxPriority) {
            maxPriority = priority;
            selectedQueue = i;
        }
    }

    if (selectedQueue >= 0) {
        Ptr<const Packet> peekedPacket = q_class[selectedQueue]->Peek();
        if (peekedPacket) {
            std::cout << "SPQ::Schedule: Scheduled packet from queue " << selectedQueue 
                      << ", priority=" << maxPriority << ", size=" << peekedPacket->GetSize() 
                      << ", time=" << Simulator::Now().GetSeconds() << "s" << std::endl;
            return {static_cast<uint32_t>(selectedQueue), peekedPacket};
        }
    }

    std::cout << "SPQ::Schedule: No packet scheduled (all queues empty)" << std::endl;
    return {q_class.size(), nullptr};
}

/**
 * @brief Classifies a packet to determine the appropriate queue.
 *
 * Iterates through the traffic class queues and returns the index of the first queue
 * whose filter matches the packet. Logs the classification result and simulation time.
 *
 * @param p Pointer to the packet to be classified.
 * @return The index of the matching queue, or q_class.size() if no queue matches.
 */
uint32_t SPQ::Classify(Ptr<Packet> p) {
    for (uint32_t i = 0; i < q_class.size(); ++i) {
        if (q_class[i]->match(p)) {
            std::cout << "SPQ::Classify: Packet matched queue " << i << " at time " 
                      << Simulator::Now().GetSeconds() << "s" << std::endl;
            return i;
        }
    }
    std::cout << "SPQ::Classify: Packet dropped (no matching queue) at time " 
              << Simulator::Now().GetSeconds() << "s" << std::endl;
    return q_class.size();
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

    std::cout << "SPQ::ReadConfigFile: Configured " << q_class.size() << " queues" << std::endl;
    return true;
}

/**
 * @brief Parses a single line from the configuration file.
 *
 * Interprets the line to configure a queue (with priority and max packets) or a filter
 * (e.g., source/destination IP, port, or protocol) for a specific queue. Logs the parsing result.
 *
 * @param line The configuration line to parse.
 */
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
                std::cout << "SPQ::ParseConfigLine: Added filter for dst_port=" << value << " to queue " << queueId << std::endl;
            } else if (filterType == "protocol") {
                filter->AddElement(new ProtocolNumber(std::stoi(value)));
            }
            if (queueId < q_class.size()) {
                q_class[queueId]->AddFilter(filter);
                std::cout << "SPQ::ParseConfigLine: Added filter to queue " << queueId 
                          << ", type=" << filterType << ", value=" << value << std::endl;
            } else {
                std::cerr << "SPQ::ParseConfigLine: Invalid queueId " << queueId << " for filter" << std::endl;
                delete filter;
            }
        }
    }
}

} // namespace ns3
