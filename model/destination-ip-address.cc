#include "destination-ip-address.h"
#include "ns3/log.h"
#include "ns3/ipv4-header.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE("DestinationIPAddress");

NS_OBJECT_ENSURE_REGISTERED(DestinationIPAddress);

TypeId DestinationIPAddress::GetTypeId(void) {
    static TypeId tid = TypeId("ns3::DestinationIPAddress")
        .SetParent<FilterElement>()
        .SetGroupName("Network")
        .AddConstructor<DestinationIPAddress>();
    return tid;
}

DestinationIPAddress::DestinationIPAddress()
    : m_address("0.0.0.0") { // Default address (matches any IP)
    NS_LOG_FUNCTION(this);
}

DestinationIPAddress::DestinationIPAddress(Ipv4Address address)
    : m_address(address) {
    NS_LOG_FUNCTION(this << address);
}

DestinationIPAddress::~DestinationIPAddress() {
    NS_LOG_FUNCTION(this);
}

bool DestinationIPAddress::Match(Ptr<Packet> p) const {
    NS_LOG_FUNCTION(this << p);

    Ipv4Header header;
    if (p->PeekHeader(header)) {
        return header.GetDestination() == m_address;
    }
    return false;
}

} // namespace ns3
