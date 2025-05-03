#include "source-port-number.h"
#include "ns3/log.h"
#include "ns3/udp-header.h"
#include "ns3/tcp-header.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE("SourcePortNumber");

NS_OBJECT_ENSURE_REGISTERED(SourcePortNumber);

TypeId SourcePortNumber::GetTypeId(void) {
    static TypeId tid = TypeId("ns3::SourcePortNumber")
        .SetParent<FilterElement>()
        .SetGroupName("Network")
        .AddConstructor<SourcePortNumber>();
    return tid;
}

SourcePortNumber::SourcePortNumber()
    : m_port(0) { // Default port (0, typically not used)
    NS_LOG_FUNCTION(this);
}

SourcePortNumber::SourcePortNumber(uint16_t port)
    : m_port(port) {
    NS_LOG_FUNCTION(this << port);
}

SourcePortNumber::~SourcePortNumber() {
    NS_LOG_FUNCTION(this);
}

bool SourcePortNumber::Match(Ptr<Packet> p) const {
    NS_LOG_FUNCTION(this << p);

    UdpHeader udpHeader;
    TcpHeader tcpHeader;

    if (p->PeekHeader(udpHeader)) {
        return udpHeader.GetSourcePort() == m_port;
    } else if (p->PeekHeader(tcpHeader)) {
        return tcpHeader.GetSourcePort() == m_port;
    }
    return false;
}

} // namespace ns3
