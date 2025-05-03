#include "source-mask.h"
#include "ns3/log.h"
#include "ns3/ipv4-header.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE("SourceMask");

NS_OBJECT_ENSURE_REGISTERED(SourceMask);

TypeId SourceMask::GetTypeId(void) {
    static TypeId tid = TypeId("ns3::SourceMask")
        .SetParent<FilterElement>()
        .SetGroupName("Network")
        .AddConstructor<SourceMask>();
    return tid;
}

SourceMask::SourceMask()
    : m_mask("0.0.0.0") { // Default mask (matches any IP)
    NS_LOG_FUNCTION(this);
}

SourceMask::SourceMask(Ipv4Mask mask)
    : m_mask(mask) {
    NS_LOG_FUNCTION(this << mask);
}

SourceMask::~SourceMask() {
    NS_LOG_FUNCTION(this);
}

bool SourceMask::Match(Ptr<Packet> p) const {
    NS_LOG_FUNCTION(this << p);

    Ipv4Header header;
    if (p->PeekHeader(header)) {
        Ipv4Address source = header.GetSource();
        return (source.CombineMask(m_mask) == source);
    }
    return false;
}

} // namespace ns3
