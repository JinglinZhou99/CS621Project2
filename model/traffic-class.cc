#include "traffic-class.h"
#include "ns3/packet.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED(TrafficClass);

/**
 * @brief Returns the TypeId for TrafficClass.
 *
 * Registers the TrafficClass with the ns-3 object system and sets its parent and group.
 *
 * @return The TypeId of the TrafficClass.
 */
TypeId TrafficClass::GetTypeId(void) {
    static TypeId tid = TypeId("ns3::TrafficClass")
        .SetParent<Object>()
        .SetGroupName("Network")
        .AddConstructor<TrafficClass>();
    return tid;
}

TrafficClass::TrafficClass()
    : packets(0), maxPackets(1000), weight(0), priority_level(0), isDefault(false) {
}

/**
 * @brief Destructor for TrafficClass.
 *
 * Frees memory allocated for all filters and clears the filters vector.
 */
TrafficClass::~TrafficClass() {
    for (Filter* filter : filters) {
        delete filter;
    }
    filters.clear();
}

/**
 * @brief Checks if a packet matches any filter in the traffic class.
 *
 * If no filters are present, the packet is accepted. Otherwise, the packet is evaluated
 * against each filter, and accepted if any filter matches. Logs the outcome.
 *
 * @param p Pointer to the packet to be evaluated.
 * @return True if the packet matches any filter or no filters exist, false otherwise.
 */
bool TrafficClass::match(Ptr<Packet> p) {
    if (filters.empty()) {
        std::cout << "TrafficClass::match: No filters, packet accepted" << std::endl;
        return true;
    }

    for (Filter* filter : filters) {
        if (filter->match(p)) {
            std::cout << "TrafficClass::match: Packet accepted by filter" << std::endl;
            return true;
        }
    }
    std::cout << "TrafficClass::match: Packet rejected by filter" << std::endl;
    return false;
}

/**
 * @brief Enqueues a packet into the traffic class queue.
 *
 * Checks if the queue has reached its maximum capacity. If not, enqueues the packet
 * and increments the packet count. Logs the outcome.
 *
 * @param p Pointer to the packet to be enqueued.
 * @return True if the packet was successfully enqueued, false if the queue is full.
 */
bool TrafficClass::Enqueue(Ptr<Packet> p) {
    if (packets >= maxPackets) {
        std::cout << "TrafficClass::Enqueue: Queue full, packet dropped" << std::endl;
        return false;
    }
    m_queue.push(p);
    packets++;
    std::cout << "TrafficClass::Enqueue: Packet enqueued, current size=" << packets << std::endl;
    return true;
}

/**
 * @brief Dequeues a packet from the traffic class queue.
 *
 * Removes and returns the front packet from the queue, if available, and decrements
 * the packet count. Logs the outcome.
 *
 * @return Pointer to the dequeued packet, or nullptr if the queue is empty.
 */
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

bool TrafficClass::IsEmpty() {
    bool empty = m_queue.empty();
    std::cout << "TrafficClass::IsEmpty: Queue " << (empty ? "is empty" : "is not empty") << std::endl;
    return empty;
}

/**
 * @brief Sets the maximum number of packets the queue can hold.
 *
 * Updates the maximum packet capacity and logs the new value.
 *
 * @param mp The maximum number of packets.
 */
void TrafficClass::SetMaxPackets(uint32_t mp) {
    maxPackets = mp;
    std::cout << "TrafficClass::SetMaxPackets: Set maxPackets=" << maxPackets << std::endl;
}

uint32_t TrafficClass::GetMaxPackets() {
    return maxPackets;
}

/**
 * @brief Sets the weight of the traffic class.
 *
 * Updates the weight value used for scheduling and logs the new value.
 *
 * @param w The weight value.
 */
void TrafficClass::SetWeight(uint32_t w) {
    weight = w;
    std::cout << "TrafficClass::SetWeight: Set weight=" << weight << std::endl;
}

uint32_t TrafficClass::GetWeight() {
    return weight;
}

void TrafficClass::SetPriorityLevel(uint32_t pl) {
    priority_level = pl;
    std::cout << "TrafficClass::SetPriorityLevel: Set priority_level=" << priority_level << std::endl;
}

uint32_t TrafficClass::GetPriorityLevel() {
    return priority_level;
}

void TrafficClass::SetDefault(bool d) {
    isDefault = d;
    std::cout << "TrafficClass::SetDefault: Set isDefault=" << (isDefault ? "true" : "false") << std::endl;
}

bool TrafficClass::GetDefault() {
    return isDefault;
}

/**
 * @brief Adds a filter to the traffic class.
 *
 * Appends the provided filter to the list of filters and logs the updated filter count.
 *
 * @param f Pointer to the filter to be added.
 */
void TrafficClass::AddFilter(Filter* f) {
    filters.push_back(f);
    std::cout << "TrafficClass::AddFilter: Added filter, total filters=" << filters.size() << std::endl;
}

} // namespace ns3
