#include "traffic-class.h"
#include "ns3/packet.h"
#include <iostream>

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED(TrafficClass);

TypeId TrafficClass::GetTypeId(void) {
    static TypeId tid = TypeId("ns3::TrafficClass")
        .SetParent<Object>()
        .SetGroupName("Network")
        .AddConstructor<TrafficClass>();
    return tid;
}

TrafficClass::TrafficClass() : packets(0), maxPackets(0), weight(0.0), priority_level(0), isDefault(false) {
}

TrafficClass::~TrafficClass() {
    // Clean up filters
    for (Filter* filter : filters) {
        for (FilterElement* element : filter->elements) {
            delete element;
        }
        delete filter;
    }
}

bool TrafficClass::Enqueue(Ptr<Packet> p) {
    if (packets >= maxPackets) {
        std::cout << "TrafficClass::Enqueue: Packet dropped (queue full, maxPackets=" << maxPackets << ")" << std::endl;
        return false;
    }
    m_queue.push(p);
    packets++;
    std::cout << "TrafficClass::Enqueue: Packet enqueued, current size=" << packets << std::endl;
    return true;
}

Ptr<Packet> TrafficClass::Dequeue() {
    if (m_queue.empty()) {
        std::cout << "TrafficClass::Dequeue: Queue empty" << std::endl;
        return nullptr;
    }
    Ptr<Packet> p = m_queue.front();
    m_queue.pop();
    packets--;
    std::cout << "TrafficClass::Dequeue: Packet dequeued, remaining size=" << packets << std::endl;
    return p;
}

Ptr<Packet> TrafficClass::Remove() {
    if (m_queue.empty()) {
        std::cout << "TrafficClass::Remove: Queue empty" << std::endl;
        return nullptr;
    }
    Ptr<Packet> p = m_queue.front();
    m_queue.pop();
    packets--;
    std::cout << "TrafficClass::Remove: Packet removed, remaining size=" << packets << std::endl;
    return p;
}

Ptr<const Packet> TrafficClass::Peek() {
    if (m_queue.empty()) {
        std::cout << "TrafficClass::Peek: Queue empty" << std::endl;
        return nullptr;
    }
    Ptr<const Packet> p = m_queue.front();
    std::cout << "TrafficClass::Peek: Peeked packet, size=" << packets << std::endl;
    return p;
}

bool TrafficClass::match(Ptr<Packet> p) {
    if (filters.empty()) {
        std::cout << "TrafficClass::match: No filters, packet accepted" << std::endl;
        return true;
    }
    for (Filter* filter : filters) {
        if (!filter->match(p)) {
            std::cout << "TrafficClass::match: Packet rejected by filter" << std::endl;
            return false;
        }
    }
    std::cout << "TrafficClass::match: Packet accepted by all filters" << std::endl;
    return true;
}

bool TrafficClass::IsEmpty() {
    bool empty = m_queue.empty();
    if (empty != (packets == 0)) {
        std::cerr << "TrafficClass::IsEmpty: Inconsistency detected: m_queue.empty()=" << empty 
                  << ", packets=" << packets << std::endl;
    }
    return empty;
}

uint32_t TrafficClass::GetSize() {
    uint32_t size = m_queue.size();
    if (size != packets) {
        std::cerr << "TrafficClass::GetSize: Inconsistency detected: m_queue.size()=" << size 
                  << ", packets=" << packets << std::endl;
    }
    return size;
}

void TrafficClass::AddFilter(Filter* filter) {
    filters.push_back(filter);
    std::cout << "TrafficClass::AddFilter: Added filter, total filters=" << filters.size() << std::endl;
}

void TrafficClass::SetMaxPackets(uint32_t max) {
    maxPackets = max;
    std::cout << "TrafficClass::SetMaxPackets: Set maxPackets=" << maxPackets << std::endl;
}

void TrafficClass::SetWeight(double w) {
    weight = w;
    std::cout << "TrafficClass::SetWeight: Set weight=" << weight << std::endl;
}

double TrafficClass::GetWeight() {
    return weight;
}

void TrafficClass::SetPriorityLevel(uint32_t level) {
    priority_level = level;
    std::cout << "TrafficClass::SetPriorityLevel: Set priority_level=" << priority_level << std::endl;
}

uint32_t TrafficClass::GetPriorityLevel() {
    return priority_level;
}

void TrafficClass::SetDefault(bool default_queue) {
    isDefault = default_queue;
    std::cout << "TrafficClass::SetDefault: Set isDefault=" << (isDefault ? "true" : "false") << std::endl;
}

bool TrafficClass::GetDefault() {
    return isDefault;
}

} // namespace ns3