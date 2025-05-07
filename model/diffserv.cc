#include "diffserv.h"
#include "ns3/packet.h"

namespace ns3 {

DiffServ::DiffServ() {}

bool DiffServ::Enqueue(Ptr<Packet> p) {
    return DoEnqueue(p);
}

bool DiffServ::DoEnqueue(Ptr<Packet> p) {
    uint32_t queue_index = Classify(p);
    if (queue_index >= q_class.size()) {
        std::cout << "DiffServ::DoEnqueue: Packet dropped (no matching queue)" << std::endl;
        return false;
    }
    bool success = q_class[queue_index]->Enqueue(p);
    std::cout << "DiffServ::DoEnqueue: Packet enqueued in queue " << queue_index 
              << ", success=" << (success ? "true" : "false") << std::endl;
    return success;
}

Ptr<Packet> DiffServ::Dequeue() {
    return DoDequeue();
}

Ptr<Packet> DiffServ::DoDequeue() {
    auto [index, dpacket] = Schedule();
    if (dpacket && index < q_class.size()) {
        std::cout << "DiffServ::DoDequeue: Dequeuing packet from queue " << index << std::endl;
        Ptr<Packet> packet = q_class[index]->Dequeue();
        if (!packet) {
            std::cout << "DiffServ::DoDequeue: Dequeue returned nullptr for queue " << index << std::endl;
        }
        return packet;
    }
    std::cout << "DiffServ::DoDequeue: No packet to dequeue (index=" << index 
              << ", dpacket=" << (dpacket ? "valid" : "nullptr") << ")" << std::endl;
    return nullptr;
}

Ptr<Packet> DiffServ::Remove() {
    return DoRemove();
}

Ptr<Packet> DiffServ::DoRemove() {
    auto [index, rpacket] = Schedule();
    if (rpacket && index < q_class.size()) {
        std::cout << "DiffServ::DoRemove: Removing packet from queue " << index << std::endl;
        return q_class[index]->Remove();
    }
    std::cout << "DiffServ::DoRemove: No packet to remove (index=" << index 
              << ", rpacket=" << (rpacket ? "valid" : "nullptr") << ")" << std::endl;
    return nullptr;
}

Ptr<const Packet> DiffServ::Peek() const {
    auto [index, next_packet] = const_cast<DiffServ*>(this)->Schedule();
    if (next_packet) {
        std::cout << "DiffServ::DoPeek: Peeking packet from queue " << index << std::endl;
        return next_packet->Copy();
    }
    std::cout << "DiffServ::DoPeek: No packet to peek (index=" << index 
              << ", next_packet=" << (next_packet ? "valid" : "nullptr") << ")" << std::endl;
    return nullptr;
}

void DiffServ::AddQueue(Ptr<TrafficClass> trafficClass) {
    q_class.push_back(trafficClass);
    std::cout << "DiffServ::AddQueue: Added queue, total queues=" << q_class.size() << std::endl;
}

std::vector<Ptr<TrafficClass>> DiffServ::GetQueues() const {
    return q_class;
}

} // namespace ns3
