#include "diffserv.h"
#include "ns3/packet.h"

namespace ns3 {

DiffServ::DiffServ() {}

bool DiffServ::Enqueue(Ptr<Packet> p) {
    return DoEnqueue(p);
}

/**
 * @brief Performs the actual enqueuing of a packet.
 *
 * Classifies the packet to determine the target queue and enqueues it.
 * Logs the outcome of the operation.
 *
 * @param p Pointer to the packet to be enqueued.
 * @return True if the packet was successfully enqueued, false if no matching queue is found or enqueuing fails.
 */
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

/**
 * @brief Performs the actual dequeuing of a packet.
 *
 * Uses the scheduling mechanism to select a queue and dequeues a packet from it.
 * Logs the outcome of the operation.
 *
 * @return Pointer to the dequeued packet, or nullptr if no packet is available or dequeuing fails.
 */
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

/**
 * @brief Adds a traffic class queue to the DiffServ system.
 *
 * Appends the provided traffic class to the list of queues and logs the updated queue count.
 *
 * @param trafficClass Pointer to the traffic class to be added.
 */
void DiffServ::AddQueue(Ptr<TrafficClass> trafficClass) {
    q_class.push_back(trafficClass);
    std::cout << "DiffServ::AddQueue: Added queue, total queues=" << q_class.size() << std::endl;
}

std::vector<Ptr<TrafficClass>> DiffServ::GetQueues() const {
    return q_class;
}

} // namespace ns3
