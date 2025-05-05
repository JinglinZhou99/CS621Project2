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
}

Ptr<const Packet> SPQ::Schedule(void) const {
    uint32_t highestPriority = 0;
    int selectedQueue = -1;

    for (uint32_t i = 0; i < q_class.size(); ++i) {
        if (!q_class[i]->IsEmpty() && q_class[i]->GetPriorityLevel() >= highestPriority) {
            highestPriority = q_class[i]->GetPriorityLevel();
            selectedQueue = i;
        }
    }

    if (selectedQueue >= 0) {
        return q_class[selectedQueue]->Peek();
    }
    return nullptr;
}

uint32_t SPQ::Classify(Ptr<Packet> p) {
    for (uint32_t i = 0; i < q_class.size(); ++i) {
        if (q_class[i]->match(p)) {
            return i;
        }
    }
    return q_class.size(); // No match, return invalid index
}

void SPQ::ReadConfigFile(std::string filename) {
    std::ifstream file(filename);
    std::string line;

    if (!file.is_open()) {
        return;
    }

    while (std::getline(file, line)) {
        ParseConfigLine(line);
    }
    file.close();
}

void SPQ::ParseConfigLine(const std::string& line) {
    std::istringstream iss(line);
    std::string token;
    iss >> token;

    if (token == "queue") {
        uint32_t queueId, priority, maxPackets;
        if (iss >> queueId >> priority >> maxPackets) {
            TrafficClass* tc = new TrafficClass();
            tc->SetPriorityLevel(priority);
            tc->SetMaxPackets(maxPackets);
            AddQueue(tc);
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
            }
        }
    }
}

} // namespace ns3