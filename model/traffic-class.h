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

    void SetDefault(bool d);
    bool GetDefault();

    bool Enqueue(Ptr<Packet> p);
    Ptr<Packet> Dequeue();
    Ptr<Packet> Remove();
    Ptr<const Packet> Peek();
    bool IsEmpty();

    bool match(Ptr<Packet> p);

    void SetMaxPackets(uint32_t max_p);
    void SetWeight(double_t w);
    double_t GetWeight();
    void SetPriorityLevel(uint32_t level);
    uint32_t GetPriorityLevel();
    void AddFilter(Filter* filter);
    uint32_t GetSize();

    std::vector<Filter*> filters;

private:
    std::queue<Ptr<Packet>> m_queue;
    uint32_t maxPackets;
    double_t weight;
    uint32_t priority_level;
    bool isDefault;
    uint32_t packets;
};

} // namespace ns3

#endif /* TRAFFIC_CLASS_H */
