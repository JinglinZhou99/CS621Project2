#include "traffic-class.h"
#include "ns3/log.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE("TrafficClass");

NS_OBJECT_ENSURE_REGISTERED(TrafficClass);

TypeId TrafficClass::GetTypeId(void) {
    static TypeId tid = TypeId("ns3::TrafficClass")
        .SetParent<ns3::Object>()
        .SetGroupName("Network")
        .AddConstructor<TrafficClass>();
    return tid;
}

TrafficClass::TrafficClass()
    : m_maxPackets(100), m_weight(1.0), m_priorityLevel(0) {
    NS_LOG_FUNCTION(this);
}

TrafficClass::~TrafficClass() {
    NS_LOG_FUNCTION(this);
    m_packets = std::queue<Ptr<Packet>>();
    m_filters.clear();
}

bool TrafficClass::Enqueue(Ptr<Packet> p) {
    NS_LOG_FUNCTION(this << p);

    if (m_packets.size() >= m_maxPackets) {
        NS_LOG_WARN("Queue is full, dropping packet");
        return false;
    }

    m_packets.push(p);
    return true;
}

Ptr<Packet> TrafficClass::Dequeue() {
    NS_LOG_FUNCTION(this);

    if (m_packets.empty()) {
        return nullptr;
    }

    Ptr<Packet> p = m_packets.front();
    m_packets.pop();
    return p;
}

bool TrafficClass::Match(Ptr<Packet> p) const {
    NS_LOG_FUNCTION(this << p);

    // A packet matches if it satisfies any of the filters
    for (const auto& filter : m_filters) {
        if (filter->Match(p)) {
            return true;
        }
    }
    return false;
}

void TrafficClass::SetMaxPackets(uint32_t maxPackets) {
    m_maxPackets = maxPackets;
}

uint32_t TrafficClass::GetMaxPackets() const {
    return m_maxPackets;
}

void TrafficClass::SetWeight(double weight) {
    m_weight = weight;
}

double TrafficClass::GetWeight() const {
    return m_weight;
}

void TrafficClass::SetPriorityLevel(uint32_t priorityLevel) {
    m_priorityLevel = priorityLevel;
}

uint32_t TrafficClass::GetPriorityLevel() const {
    return m_priorityLevel;
}

void TrafficClass::AddFilter(Ptr<Filter> filter) {
    m_filters.push_back(filter);
}

uint32_t TrafficClass::GetPacketCount() const {
    return m_packets.size();
}

} // namespace ns3
