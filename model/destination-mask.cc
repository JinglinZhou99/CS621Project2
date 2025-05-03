#include "destination-mask.h"
#include "ns3/log.h"
#include "ns3/ipv4-header.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE("DestinationMask");

NS_OBJECT_ENSURE_REGISTERED(DestinationMask);

TypeId DestinationMask::GetTypeId(void) {
    static TypeId tid = TypeId("ns3::DestinationMask")
        .SetParent<FilterElement>()
        .SetGroupName("Network")
        .AddConstructor<DestinationMask>();
    return tid;
}

DestinationMask::DestinationMask()
    : m_mask("0.0.0.0") { // Default mask (matches any IP)
    NS_LOG_FUNCTION(this);
}

DestinationMask::DestinationMask(Ipv4Mask mask)
    : m_mask(mask) {
    NS_LOG_FUNCTION(this << mask);
}

DestinationMask::~DestinationMask() {
    NS_LOG_FUNCTION(this);
}

bool DestinationMask::Match(Ptr<Packet> p) const {
    NS_LOG_FUNCTION(this << p);

    Ipv4Header header;
    if (p->PeekHeader(header)) {
        Ipv4Address dest = header.GetDestination();
        return (dest.CombineMask(m_mask) == dest);
    }
    return false;
}

} // namespace ns3
