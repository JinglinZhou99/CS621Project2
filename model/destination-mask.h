#ifndef DESTINATION_MASK_H
#define DESTINATION_MASK_H

#include "filter-element.h"
#include "ns3/ipv4-address.h"

namespace ns3 {

class DestinationMask : public FilterElement {
public:
    static TypeId GetTypeId(void);

    DestinationMask();
    DestinationMask(Ipv4Mask mask);
    virtual ~DestinationMask();

    bool Match(Ptr<Packet> p) const override;

private:
    Ipv4Mask m_mask; // Destination IP mask to match
};

} // namespace ns3

#endif /* DESTINATION_MASK_H */
