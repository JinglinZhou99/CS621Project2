#ifndef TRAFFIC_CLASS_H
#define TRAFFIC_CLASS_H

#include "ns3/packet.h"
#include "ns3/object.h"
#include "filter.h"
#include <queue>

namespace ns3 {

class TrafficClass : public Object {
public:
    static TypeId GetTypeId(void);

    TrafficClass();
    virtual ~TrafficClass();

    // Enqueue a packet into this TrafficClass
    bool Enqueue(Ptr<Packet> p);

    // Dequeue a packet from this TrafficClass
    Ptr<Packet> Dequeue();

    // Check if the TrafficClass matches the packet
    bool Match(Ptr<Packet> p) const;

   // Getters and setters for TrafficClass properties
    void SetMaxPackets(uint32_t maxPackets);
    uint32_t GetMaxPackets() const;
    void SetWeight(double weight);
    double GetWeight() const;
    void SetPriorityLevel(uint32_t priorityLevel);
    uint32_t GetPriorityLevel() const;
    void AddFilter(Ptr<Filter> filter);
    uint32_t GetPacketCount() const;

private:
    std::queue<Ptr<Packet>> m_packets; // Queue of packets
    uint32_t m_maxPackets;             // Maximum number of packets allowed
    double m_weight;                   // Weight for DRR
    uint32_t m_priorityLevel;          // Priority level for SPQ
    std::vector<Ptr<Filter>> m_filters; // Collection of filters
};

} // namespace ns3

#endif /* TRAFFIC_CLASS_H */
