#include "destination-port-number.h"
#include "ns3/log.h"
#include "ns3/udp-header.h"
#include "ns3/tcp-header.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE("DestinationPortNumber");

NS_OBJECT_ENSURE_REGISTERED(DestinationPortNumber);

TypeId DestinationPortNumber::GetTypeId(void) {
    static TypeId tid = TypeId("ns3::DestinationPortNumber")
        .SetParent<FilterElement>()
        .SetGroupName("Network")
        .AddConstructor<DestinationPortNumber>();
    return tid;
}

DestinationPortNumber::DestinationPortNumber()
    : m_port(0) { // Default port (0, typically not used)
    NS_LOG_FUNCTION(this);
}

DestinationPortNumber::DestinationPortNumber(uint16_t port)
    : m_port(port) {
    NS_LOG_FUNCTION(this << port);
}

DestinationPortNumber::~DestinationPortNumber() {
    NS_LOG_FUNCTION(this);
}

bool DestinationPortNumber::Match(Ptr<Packet> p) const {
    NS_LOG_FUNCTION(this << p);

    UdpHeader udpHeader;
    TcpHeader tcpHeader;

    if (p->PeekHeader(udpHeader)) {
        return udpHeader.GetDestinationPort() == m_port;
    } else if (p->PeekHeader(tcpHeader)) {
        return tcpHeader.GetDestinationPort() == m_port;
    }
    return false;
}

} // namespace ns3
