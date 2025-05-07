#ifndef DIFFSERV_H
#define DIFFSERV_H

#include "ns3/queue.h"
#include "traffic-class.h"
#include <vector>
#include <utility> // For std::pair

namespace ns3 {

class DiffServ : public Queue<Packet> {
public:
    DiffServ();

    bool Enqueue(Ptr<Packet> p) override;
    Ptr<Packet> Dequeue() override;
    Ptr<Packet> Remove() override;
    Ptr<const Packet> Peek() const override;
    virtual uint32_t Classify(Ptr<Packet> p) = 0;
    virtual std::pair<uint32_t, Ptr<const Packet>> Schedule() = 0;
    void AddQueue(Ptr<TrafficClass> q);
    std::vector<Ptr<TrafficClass>> GetQueues() const;

protected:
    bool DoEnqueue(Ptr<Packet> p);
    Ptr<Packet> DoDequeue();
    Ptr<Packet> DoRemove();
    Ptr<const Packet> DoPeek() const;

    std::vector<Ptr<TrafficClass>> q_class; // Uses Ptr for memory management
};

} // namespace ns3

#endif /* DIFFSERV_H */