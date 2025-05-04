#ifndef TRAFFIC_CLASS_H
#define TRAFFIC_CLASS_H

#include "ns3/packet.h"
#include "ns3/object.h"
#include "filter.h"
#include <queue>
#include <vector>

namespace ns3 {

class TrafficClass {
public:
    TrafficClass();

    // Default status of the traffic class
    void SetDefault(bool d);
    bool GetDefault();

    // Queue operations
    bool Enqueue(Ptr<Packet> p);
    Ptr<Packet> Dequeue();
    Ptr<Packet> Remove();
    Ptr<const Packet> Peek();
    bool IsEmpty()

    // Check if the TrafficClass matches the packet
    bool match(Ptr<Packet> p);

    // Max number of packets in the traffic class
    void SetMaxPackets(uint32_t max_p);

    // Weight of the traffic class
    void SetWeight(double_t w);
    double_t GetWeight();

    // Priority level of the traffic class
    void SetPriorityLevel(uint32_t level);
    uint32_t GetPriorityLevel();

    void AddFilter(Filter* filter);
    uint32_t GetSize();
    std::vector<Filter*> filters;

private:
    std::queue<Ptr<Packet>> m_queue; // Queue of packets
    uint32_t maxPackets;             // Maximum number of packets allowed
    double_t weight;                   // Weight for DRR
    uint32_t priority_level;          // Priority level for SPQ
    bool isDefault;
    uint32_t packets;
};

} // namespace ns3

#endif /* TRAFFIC_CLASS_H */
