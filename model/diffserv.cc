#include "diffserv.h"

namespace ns3 {

DiffServ::DiffServ() {}

bool DiffServ::Enqueue(Ptr<Packet> p){
    return DoEnqueue(p);
}

bool DiffServ::DoEnqueue(Ptr<Packet> p) {
    uint32_t queue_index = Classify(p);
    if (queue_index >= q_class.size()) {
        return false;
    }
    return q_class[queue_index]->Enqueue(p);
}

Ptr<Packet> DiffServ::Dequeue() {
    return DoDequeue();
}

Ptr<Packet> DiffServ::DoDequeue() {
    Ptr<const Packet> dpacket = Schedule();
    if (dpacket)
    {
        uint32_t index = Classify(dpacket->Copy());
        return q_class[index]->Dequeue();
    }
    return nullptr;
}

Ptr<Packet> DiffServ::Remove() {
    return DoRemove();
}

Ptr<Packet> DiffServ::DoRemove() {
    Ptr<const Packet> rpacket = Schedule();
    if (rpacket != nullptr)
    {
        uint32_t index = Classify(rpacket->Copy());
        return q_class[index]->Remove();
    }
    return nullptr;
}

Ptr<const Packet> DiffServ::Peek() const {
    return DoPeek();
}

Ptr<const Packet> DiffServ::DoPeek() const {
    Ptr<const Packet> next_packet = Schedule()->Copy();
    return next_packet;
}

uint32_t DiffServ::Classify(Ptr<Packet> p) {
    uint32_t index = -1;
    for (int i = 0; i < q_class.size(); i++)
    {
        if (q_class[i]->match(p))
        {
            return i;
        }
        if (q_class[i]->GetDefault())
        {
            index = i;
        }
    }
    return index;
}

void DiffServ::AddQueue(TrafficClass* trafficClass)
{
    q_class.push_back(trafficClass);
}

std::vector<TrafficClass*> DiffServ::GetQueues() const
{
    return q_class;
}
} // namespace ns3
