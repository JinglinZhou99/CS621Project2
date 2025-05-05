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
}

Ptr<const Packet> DRR::Schedule(void) const {
    if (q_class.empty()) {
        return nullptr;
    }

    uint32_t startQueue = currentQueue;
    do {
        TrafficClass* queue = q_class[currentQueue];
        if (!queue->IsEmpty()) {
            if (deficits[currentQueue] >= queue->Peek()->GetSize()) {
                Ptr<const Packet> packet = queue->Peek();
                deficits[currentQueue] -= packet->GetSize();
                currentQueue = (currentQueue + 1) % q_class.size();
                return packet;
            } else {
                deficits[currentQueue] += queue->GetWeight(); // Add quantum
            }
        }
        currentQueue = (currentQueue + 1) % q_class.size();
    } while (currentQueue != startQueue);

    return nullptr;
}

uint32_t DRR::Classify(Ptr<Packet> p) {
    for (uint32_t i = 0; i < q_class.size(); ++i) {
        if (q_class[i]->match(p)) {
            return i;
        }
    }
    return q_class.size(); // No match, return invalid index
}

void DRR::ReadConfigFile(std::string filename) {
    std::ifstream file(filename);
    std::string line;

    if (!file.is_open()) {
        return;
    }

    while (std::getline(file, line)) {
        ParseConfigLine(line);
    }
    file.close();

    deficits.resize(q_class.size(), 0); // Initialize deficits
}

void DRR::ParseConfigLine(const std::string& line) {
    std::istringstream iss(line);
    std::string token;
    iss >> token;

    if (token == "queue") {
        uint32_t queueId, quantum, maxPackets;
        if (iss >> queueId >> quantum >> maxPackets) {
            TrafficClass* tc = new TrafficClass();
            tc->SetWeight(quantum);
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