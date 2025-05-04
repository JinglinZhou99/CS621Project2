#include "traffic-class.h"
#include "ns3/packet.h"
#include <iostream>

namespace ns3 {

TrafficClass::TrafficClass() : packets(0), maxPackets(0), weight(0.0), priority_level(0), isDefault(false) {}

bool TrafficClass::Enqueue(Ptr<Packet> p) {
    if (packets >= maxPackets) {
        return false;
    }
    m_queue.push(p);
    packets++;
    return true;
}

Ptr<Packet> TrafficClass::Dequeue() {
    if (m_queue.empty()) {
        return nullptr;
    }
    Ptr<Packet> p = m_queue.front();
    m_queue.pop();
    packets--;
    return p;
}

Ptr<Packet> TrafficClass::Remove() {
    if (m_queue.empty()) {
        std::cout << "Queue empty." << std::endl;
        return nullptr;
    }
    Ptr<Packet> p = m_queue.front();
    m_queue.pop();
    packets--;
    return p;
}

Ptr<const Packet> TrafficClass::Peek() {
    if (IsEmpty()) {
        std::cout << "Queue empty." << std::endl;
        return nullptr;
    }
    return m_queue.front();
}

bool TrafficClass::match(Ptr<Packet> p) {
    if (filters.empty()) {
        return true;
    }
    for (Filter* filter : filters) {
        if (filter->match(p)) {
            return true;
        }
    }
    return false;
}

bool TrafficClass::IsEmpty() {
    return packets == 0;
}

uint32_t TrafficClass::GetSize() {
    return m_queue.size();
}

void TrafficClass::AddFilter(Filter* filter) {
    filters.push_back(filter);
}

void TrafficClass::SetMaxPackets(uint32_t max) {
    maxPackets = max;
}

void TrafficClass::SetWeight(double_t w) {
    weight = w;
}

double_t TrafficClass::GetWeight() {
    return weight;
}

void TrafficClass::SetPriorityLevel(uint32_t level) {
    priority_level = level;
}

uint32_t TrafficClass::GetPriorityLevel() {
    return priority_level;
}

void TrafficClass::SetDefault(bool default_queue) {
    isDefault = default_queue;
}

bool TrafficClass::GetDefault() {
    return isDefault;
}

} // namespace ns3
