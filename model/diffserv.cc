#include "diffserv.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED(DiffServ);

TypeId DiffServ::GetTypeId(void) {
    static TypeId tid = TypeId("ns3::DiffServ")
        .SetParent<Queue<Packet>>()
        .SetGroupName("Network");
    return tid;
}

DiffServ::DiffServ() {
}

DiffServ::~DiffServ() {
    q_class.clear();
}

bool DiffServ::DoEnqueue(Ptr<Packet> p) {
    uint32_t queueIndex = Classify(p);
    if (queueIndex >= q_class.size()) {
        return false;
    }

    return q_class[queueIndex]->Enqueue(p);
}

Ptr<Packet> DiffServ::DoDequeue() {
    return Schedule();
}

Ptr<Packet> DiffServ::DoRemove() {
    return nullptr;
}

Ptr<Packet> DiffServ::DoPeek() const {
    return nullptr;
}

} // namespace ns3
