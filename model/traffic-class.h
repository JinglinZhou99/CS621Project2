#ifndef TRAFFIC_CLASS_H
#define TRAFFIC_CLASS_H

#include "filter.h"
#include "ns3/object.h" // Changed from ns3/packet.h to include Object
#include "ns3/packet.h"
#include <queue>
#include <vector>

namespace ns3 {

class TrafficClass : public Object { // Inherit from Object
public:
    static TypeId GetTypeId(void); // Add type system support
    TrafficClass();
    virtual ~TrafficClass();

    void SetDefault(bool d);
    bool GetDefault();

    bool Enqueue(Ptr<Packet> p);
    Ptr<Packet> Dequeue();
    Ptr<Packet> Remove();
    Ptr<const Packet> Peek();
    bool IsEmpty();

    bool match(Ptr<Packet> p);

    void SetMaxPackets(uint32_t max);
    void SetWeight(double w);
    double GetWeight();
    void SetPriorityLevel(uint32_t level);
    uint32_t GetPriorityLevel();
    void AddFilter(Filter* filter);
    uint32_t GetSize();

private:
    std::queue<Ptr<Packet>> m_queue;
    uint32_t packets;
    uint32_t maxPackets;
    double weight;
    uint32_t priority_level;
    bool isDefault;
    std::vector<Filter*> filters;
};

} // namespace ns3

#endif /* TRAFFIC_CLASS_H */