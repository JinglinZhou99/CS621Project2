#ifndef DIFFSERV_H
#define DIFFSERV_H

#include "ns3/queue.h"
#include "traffic-class.h"
#include <vector>

namespace ns3 {

class DiffServ : public Queue<Packet> {
public:
    static TypeId GetTypeId(void);

    DiffServ();
    virtual ~DiffServ();

    // Pure virtual methods for classification and scheduling
    virtual uint32_t Classify(Ptr<Packet> p) = 0;
    virtual Ptr<Packet> Schedule() = 0;

protected:
    // Override Queue methods
    virtual bool DoEnqueue(Ptr<Packet> p);
    virtual Ptr<Packet> DoDequeue();
    virtual Ptr<Packet> DoRemove();
    virtual Ptr<Packet> DoPeek() const;

    std::vector<Ptr<TrafficClass>> q_class; // Vector of TrafficClass pointers
};

} // namespace ns3

#endif /* DIFFSERV_H */
