#include "filter-element.h"
#include "ns3/packet.h"
#include "ns3/ppp-header.h"
#include "ns3/ipv4-header.h"
#include "ns3/udp-header.h"
#include "ns3/tcp-header.h"

namespace ns3 {

SrcIPAddress::SrcIPAddress(Ipv4Address addr) : default_address(addr) {}

/**
 * @brief Checks if the packet's source IP address matches the filter's address.
 *
 * Removes the PPP header, peeks at the IPv4 header, and compares the packet's source IP address
 * with the stored address. Logs the matching result.
 *
 * @param p Pointer to the packet to be evaluated.
 * @return True if the packet's source IP address matches, false otherwise.
 */
bool SrcIPAddress::match(Ptr<Packet> p) const {
    Ptr<Packet> p_copy = p->Copy();
    PppHeader pppHeader;
    p_copy->RemoveHeader(pppHeader);
    Ipv4Header header;
    p_copy->PeekHeader(header);
    bool matches = (header.GetSource() == default_address);
    std::cout << "SrcIPAddress::match: Source IP=" << header.GetSource() 
              << ", expected=" << default_address << ", match=" << (matches ? "true" : "false") << std::endl;
    return matches;
}

SrcMask::SrcMask(Ipv4Address addr, Ipv4Mask mask) : default_address(addr), default_mask(mask) {}

/**
 * @brief Checks if the packet's source IP address matches the filter's address and mask.
 *
 * Removes the PPP header, peeks at the IPv4 header, applies the mask to the packet's source IP
 * address, and compares it with the stored address. Logs the matching result.
 *
 * @param p Pointer to the packet to be evaluated.
 * @return True if the packet's source IP address matches, false otherwise.
 */
bool SrcMask::match(Ptr<Packet> p) const {
    Ptr<Packet> p_copy = p->Copy();
    PppHeader pppHeader;
    p_copy->RemoveHeader(pppHeader);
    Ipv4Header header;
    p_copy->PeekHeader(header);
    bool matches = default_mask.IsMatch(header.GetSource(), default_address);
    std::cout << "SrcMask::match: Source IP=" << header.GetSource() 
              << ", expected=" << default_address << ", mask=" << default_mask 
              << ", match=" << (matches ? "true" : "false") << std::endl;
    return matches;
}

SrcPortNumber::SrcPortNumber(uint32_t port) : default_port(port) {}

/**
 * @brief Checks if the packet's source port number matches the filter's port.
 *
 * Removes the PPP and IPv4 headers, checks the protocol (UDP or TCP), and compares the packet's
 * source port number with the stored port. Logs the matching result.
 *
 * @param p Pointer to the packet to be evaluated.
 * @return True if the packet's source port number matches, false otherwise.
 */
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
        bool matches = (udpHeader.GetSourcePort() == default_port);
        std::cout << "SrcPortNumber::match: Protocol=UDP, Source Port=" << udpHeader.GetSourcePort() 
                  << ", expected=" << default_port << ", match=" << (matches ? "true" : "false") << std::endl;
        return matches;
    } else if (protocol == 6) { // TCP
        TcpHeader tcpHeader;
        p_copy->PeekHeader(tcpHeader);
        bool matches = (tcpHeader.GetSourcePort() == default_port);
        std::cout << "SrcPortNumber::match: Protocol=TCP, Source Port=" << tcpHeader.GetSourcePort() 
                  << ", expected=" << default_port << ", match=" << (matches ? "true" : "false") << std::endl;
        return matches;
    }
    std::cout << "SrcPortNumber::match: Unknown protocol=" << static_cast<uint32_t>(protocol) << ", no match" << std::endl;
    return false;
}

DstIPAddress::DstIPAddress(Ipv4Address addr) : default_address(addr) {}

/**
 * @brief Checks if the packet's destination IP address matches the filter's address.
 *
 * Removes the PPP header, peeks at the IPv4 header, and compares the packet's destination IP
 * address with the stored address. Logs the matching result.
 *
 * @param p Pointer to the packet to be evaluated.
 * @return True if the packet's destination IP address matches, false otherwise.
 */
bool DstIPAddress::match(Ptr<Packet> p) const {
    Ptr<Packet> p_copy = p->Copy();
    PppHeader pppHeader;
    p_copy->RemoveHeader(pppHeader);
    Ipv4Header header;
    p_copy->PeekHeader(header);
    bool matches = (header.GetDestination() == default_address);
    std::cout << "DstIPAddress::match: Destination IP=" << header.GetDestination() 
              << ", expected=" << default_address << ", match=" << (matches ? "true" : "false") << std::endl;
    return matches;
}

DstMask::DstMask(Ipv4Address addr, Ipv4Mask mask) : default_address(addr), default_mask(mask) {}

/**
 * @brief Checks if the packet's destination IP address matches the filter's address and mask.
 *
 * Removes the PPP header, peeks at the IPv4 header, applies the mask to the packet's destination
 * IP address, and compares it with the stored address. Logs the matching result.
 *
 * @param p Pointer to the packet to be evaluated.
 * @return True if the packet's destination IP address matches, false otherwise.
 */
bool DstMask::match(Ptr<Packet> p) const {
    Ptr<Packet> p_copy = p->Copy();
    PppHeader pppHeader;
    p_copy->RemoveHeader(pppHeader);
    Ipv4Header header;
    p_copy->PeekHeader(header);
    bool matches = default_mask.IsMatch(header.GetDestination(), default_address);
    std::cout << "DstMask::match: Destination IP=" << header.GetDestination() 
              << ", expected=" << default_address << ", mask=" << default_mask 
              << ", match=" << (matches ? "true" : "false") << std::endl;
    return matches;
}

DstPortNumber::DstPortNumber(uint32_t port) : default_port(port) {}

/**
 * @brief Checks if the packet's destination port number matches the filter's port.
 *
 * Removes the PPP and IPv4 headers, checks the protocol (UDP or TCP), and compares the packet's
 * destination port number with the stored port. Logs the matching result.
 *
 * @param p Pointer to the packet to be evaluated.
 * @return True if the packet's destination port number matches, false otherwise.
 */
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
        bool matches = (udpHeader.GetDestinationPort() == default_port);
        std::cout << "DstPortNumber::match: Protocol=UDP, Destination Port=" << udpHeader.GetDestinationPort() 
                  << ", expected=" << default_port << ", match=" << (matches ? "true" : "false") << std::endl;
        return matches;
    } else if (protocol == 6) { // TCP
        TcpHeader tcpHeader;
        p_copy->PeekHeader(tcpHeader);
        bool matches = (tcpHeader.GetDestinationPort() == default_port);
        std::cout << "DstPortNumber::match: Protocol=TCP, Destination Port=" << tcpHeader.GetDestinationPort() 
                  << ", expected=" << default_port << ", match=" << (matches ? "true" : "false") << std::endl;
        return matches;
    }
    std::cout << "DstPortNumber::match: Unknown protocol=" << static_cast<uint32_t>(protocol) << ", no match" << std::endl;
    return false;
}

ProtocolNumber::ProtocolNumber(uint32_t protocol) : default_protocol(protocol) {}

/**
 * @brief Checks if the packet's protocol number matches the filter's protocol.
 *
 * Removes the PPP header, peeks at the IPv4 header, and compares the packet's protocol number
 * with the stored protocol. Logs the matching result.
 *
 * @param p Pointer to the packet to be evaluated.
 * @return True if the packet's protocol number matches, false otherwise.
 */
bool ProtocolNumber::match(Ptr<Packet> p) const {
    Ptr<Packet> p_copy = p->Copy();
    PppHeader pppHeader;
    p_copy->RemoveHeader(pppHeader);
    Ipv4Header header;
    p_copy->PeekHeader(header);
    bool matches = (header.GetProtocol() == default_protocol);
    std::cout << "ProtocolNumber::match: Protocol=" << static_cast<uint32_t>(header.GetProtocol()) 
              << ", expected=" << default_protocol << ", match=" << (matches ? "true" : "false") << std::endl;
    return matches;
}

} // namespace ns3
