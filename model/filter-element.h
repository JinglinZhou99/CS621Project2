#ifndef FILTER_ELEMENT_H
#define FILTER_ELEMENT_H

#include "ns3/packet.h"
#include "ns3/object.h"

namespace ns3 {

class FilterElement : public Object {
public:
    static TypeId GetTypeId(void);

    FilterElement();
    virtual ~FilterElement();

    // Pure virtual method to check if the packet matches the condition
    virtual bool Match(Ptr<Packet> p) const = 0;
};

} // namespace ns3

#endif /* FILTER_ELEMENT_H */
