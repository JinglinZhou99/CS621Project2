#ifndef DIFFSERV_H
#define DIFFSERV_H

#include "ns3/queue.h"
#include "traffic-class.h"
#include <vector>

namespace ns3 {

class DiffServ : public Queue<Packet> {
public:
    DiffServ();
    bool Enqueue(Ptr<Packet> p) override;
    Ptr<Packet> Dequeue() override;
    Ptr<Packet> Remove() override;
    Ptr<const Packet> Peek() const override;
    virtual uint32_t Classify(Ptr<Packet> p) = 0;
    virtual Ptr<const Packet> Schedule() const = 0;
    virtual void AddQueue (TrafficClass *q);
protected:
    bool DoEnqueue(Ptr<Packet> p);
    Ptr<Packet> DoDequeue();
    Ptr<Packet> DoRemove();
    Ptr<const Packet> DoPeek() const;

    std::vector<TrafficClass*> q_class;
    std::vector<TrafficClass*> GetQueues() const;
};
} // namespace ns3
#endif /* DIFFSERV_H */
