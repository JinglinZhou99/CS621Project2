#include "source-ip-address.h"
#include "ns3/log.h"
#include "ns3/ipv4-header.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE("SourceIPAddress");

NS_OBJECT_ENSURE_REGISTERED(SourceIPAddress);

TypeId SourceIPAddress::GetTypeId(void) {
    static TypeId tid = TypeId("ns3::SourceIPAddress")
        .SetParent<FilterElement>()
        .SetGroupName("Network")
        .AddConstructor<SourceIPAddress>();
    return tid;
}

SourceIPAddress::SourceIPAddress()
    : m_address("0.0.0.0") { // Default address (matches any IP)
    NS_LOG_FUNCTION(this);
}

SourceIPAddress::SourceIPAddress(Ipv4Address address)
    : m_address(address) {
    NS_LOG_FUNCTION(this << address);
}

SourceIPAddress::~SourceIPAddress() {
    NS_LOG_FUNCTION(this);
}

bool SourceIPAddress::Match(Ptr<Packet> p) const {
    NS_LOG_FUNCTION(this << p);

    // Extract the IPv4 header from the packet
    Ipv4Header header;
    if (p->PeekHeader(header)) {
        return header.GetSource() == m_address;
    }
    return false;
}

} // namespace ns3
