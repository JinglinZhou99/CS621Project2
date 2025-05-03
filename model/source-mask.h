#ifndef SOURCE_MASK_H
#define SOURCE_MASK_H

#include "filter-element.h"
#include "ns3/ipv4-address.h"

namespace ns3 {

class SourceMask : public FilterElement {
public:
    static TypeId GetTypeId(void);

    SourceMask();
    SourceMask(Ipv4Mask mask);
    virtual ~SourceMask();

    bool Match(Ptr<Packet> p) const override;

private:
    Ipv4Mask m_mask; // Source IP mask to match
};

} // namespace ns3

#endif /* SOURCE_MASK_H */
