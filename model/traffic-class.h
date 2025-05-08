#ifndef TRAFFIC_CLASS_H
#define TRAFFIC_CLASS_H

#include "ns3/object.h"
#include "ns3/packet.h"
#include "filter.h"
#include <queue>
#include <vector>

namespace ns3 {

class TrafficClass : public Object {
public:
    TrafficClass();
    ~TrafficClass() override;

    static TypeId GetTypeId(void);

    bool match(Ptr<Packet> p);
    bool Enqueue(Ptr<Packet> p);
    Ptr<Packet> Dequeue();
    Ptr<Packet> Remove();
    Ptr<const Packet> Peek();
    bool IsEmpty();

    void SetMaxPackets(uint32_t mp);
    uint32_t GetMaxPackets();
    void SetWeight(uint32_t w);
    uint32_t GetWeight();
    void SetPriorityLevel(uint32_t pl);
    uint32_t GetPriorityLevel();
    void SetDefault(bool d);
    bool GetDefault();
    void AddFilter(Filter* f);

private:
    std::queue<Ptr<Packet>> m_queue;
    uint32_t packets;
    uint32_t maxPackets;
    uint32_t weight;
    uint32_t priority_level;
    bool isDefault;
    std::vector<Filter*> filters;
};

} // namespace ns3

#endif /* TRAFFIC_CLASS_H */
