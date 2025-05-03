#include "protocol-number.h"
#include "ns3/log.h"
#include "ns3/ipv4-header.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE("ProtocolNumber");

NS_OBJECT_ENSURE_REGISTERED(ProtocolNumber);

TypeId ProtocolNumber::GetTypeId(void) {
    static TypeId tid = TypeId("ns3::ProtocolNumber")
        .SetParent<FilterElement>()
        .SetGroupName("Network")
        .AddConstructor<ProtocolNumber>();
    return tid;
}

ProtocolNumber::ProtocolNumber()
    : m_protocol(0) { // Default protocol (0, typically not used)
    NS_LOG_FUNCTION(this);
}

ProtocolNumber::ProtocolNumber(uint8_t protocol)
    : m_protocol(protocol) {
    NS_LOG_FUNCTION(this << (uint32_t)protocol);
}

ProtocolNumber::~ProtocolNumber() {
    NS_LOG_FUNCTION(this);
}

bool ProtocolNumber::Match(Ptr<Packet> p) const {
    NS_LOG_FUNCTION(this << p);

    Ipv4Header header;
    if (p->PeekHeader(header)) {
        return header.GetProtocol() == m_protocol;
    }
    return false;
}

} // namespace ns3
