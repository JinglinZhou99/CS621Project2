#include "filter-element.h"
#include "ns3/packet.h"
#include "ns3/ppp-header.h"
#include "ns3/ipv4-header.h"
#include "ns3/udp-header.h"
#include "ns3/tcp-header.h"

namespace ns3 {

SrcIPAddress::SrcIPAddress(Ipv4Address addr) : default_address(addr) {}

bool SrcIPAddress::match(Ptr<Packet> p) const {
    Ptr<Packet> p_copy = p->Copy();
    PppHeader pppHeader;
    p_copy->RemoveHeader(pppHeader);
    Ipv4Header header;
    p_copy->PeekHeader(header);
    return header.GetSource() == default_address;
}

SrcMask::SrcMask(Ipv4Address addr, Ipv4Mask mask) : default_address(addr), default_mask(mask) {}

bool SrcMask::match(Ptr<Packet> p) const {
    Ptr<Packet> p_copy = p->Copy();
    PppHeader pppHeader;
    p_copy->RemoveHeader(pppHeader);
    Ipv4Header header;
    p_copy->PeekHeader(header);
    return default_mask.IsMatch(header.GetSource(), default_address);
}

SrcPortNumber::SrcPortNumber(uint32_t port) : default_port(port) {}

bool SrcPortNumber::match(Ptr<Packet> p) const {
    Ptr<Packet> p_copy = p->Copy();
    PppHeader pppHeader;
    p_copy->RemoveHeader(pppHeader);
    Ipv4Header header;
    p_copy->RemoveHeader(header);
    uint8_t protocol = header.GetProtocol();
    if (protocol == 17) { // UDP
        UdpHeader udpHeader;
        p_copy->PeekHeader(udpHeader);
        return udpHeader.GetSourcePort() == default_port;
    } else if (protocol == 6) { // TCP
        TcpHeader tcpHeader;
        p_copy->PeekHeader(tcpHeader);
        return tcpHeader.GetSourcePort() == default_port;
    }
    return false;
}

DstIPAddress::DstIPAddress(Ipv4Address addr) : default_address(addr) {}

bool DstIPAddress::match(Ptr<Packet> p) const {
    Ptr<Packet> p_copy = p->Copy();
    PppHeader pppHeader;
    p_copy->RemoveHeader(pppHeader);
    Ipv4Header header;
    p_copy->PeekHeader(header);
    return header.GetDestination() == default_address;
}

DstMask::DstMask(Ipv4Address addr, Ipv4Mask mask) : default_address(addr), default_mask(mask) {}

bool DstMask::match(Ptr<Packet> p) const {
    Ptr<Packet> p_copy = p->Copy();
    PppHeader pppHeader;
    p_copy->RemoveHeader(pppHeader);
    Ipv4Header header;
    p_copy->PeekHeader(header);
    return default_mask.IsMatch(header.GetDestination(), default_address); // Fixed: Use GetDestination
}

DstPortNumber::DstPortNumber(uint32_t port) : default_port(port) {}

bool DstPortNumber::match(Ptr<Packet> p) const {
    Ptr<Packet> p_copy = p->Copy();
    PppHeader pppHeader;
    p_copy->RemoveHeader(pppHeader);
    Ipv4Header header;
    p_copy->RemoveHeader(header);
    uint8_t protocol = header.GetProtocol();
    if (protocol == 17) { // UDP
        UdpHeader udpHeader;
        p_copy->PeekHeader(udpHeader);
        return udpHeader.GetDestinationPort() == default_port;
    } else if (protocol == 6) { // TCP
        TcpHeader tcpHeader;
        p_copy->PeekHeader(tcpHeader);
        return tcpHeader.GetDestinationPort() == default_port;
    }
    return false;
}

ProtocolNumber::ProtocolNumber(uint32_t protocol) : default_protocol(protocol) {}

bool ProtocolNumber::match(Ptr<Packet> p) const {
    Ptr<Packet> p_copy = p->Copy();
    PppHeader pppHeader;
    p_copy->RemoveHeader(pppHeader);
    Ipv4Header header;
    p_copy->PeekHeader(header);
    return header.GetProtocol() == default_protocol;
}

} // namespace ns3
