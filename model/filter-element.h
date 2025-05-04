#ifndef FILTER_ELEMENT_H
#define FILTER_ELEMENT_H

#include "ns3/packet.h"
#include "ns3/object.h"
#include "ns3/ppp-header.h"
#include "ns3/ipv4-header.h"
#include "ns3/udp-header.h"
#include "ns3/tcp-header.h"

namespace ns3 {

class FilterElement {
public:
    // Pure virtual method to check if the packet matches the condition
    virtual bool match(Ptr<Packet> p) const = 0;
};

class SrcIPAddress : public FilterElement {
private:
    Ipv4Address default_address;
public:
    SrcIPAddress(Ipv4Address addr);
    bool match(Ptr<Packet> p) const override;
}

/** SrcMask subclass */
class SrcMask : public FilterElement {
private:
    Ipv4Address default_address;
    Ipv4Mask default_mask;

public:
    SrcMask(Ipv4Address addr, Ipv4Mask mask);

    bool match(Ptr<Packet> p) const override;
};

/** SrcPortNumber subclass */
class SrcPortNumber : public FilterElement
{
    private:
        uint32_t default_port;

    public:
        SrcPortNumber(uint32_t port);

        bool match(Ptr<Packet> p) const override;
};

/** DstIPAddress subclass */
class DstIPAddress : public FilterElement
{
    private:
        Ipv4Address default_address;

    public:
        DstIPAddress(Ipv4Address addr);

        bool match(Ptr<Packet> p) const override;
};

/** DstMask subclass */
class DstMask : public FilterElement
{
    private:
        Ipv4Address default_address;
        Ipv4Mask default_mask;

    public:
        DstMask(Ipv4Address addr, Ipv4Mask mask);

        bool match(Ptr<Packet> p) const override;
};

/** DstPortNumber subclass */
class DstPortNumber : public FilterElement
{
    private:
        uint32_t default_port;

    public:
        DstPortNumber(uint32_t port);

        bool match(Ptr<Packet> p) const override;
};

/** ProtocolNumber subclass */
class ProtocolNumber : public FilterElement
{
    private:
        uint32_t default_protocol;

    public:
        ProtocolNumber(uint32_t protocol);

        bool match(Ptr<Packet> p) const override;
};

} // namespace ns3

#endif /* FILTER_ELEMENT_H */
