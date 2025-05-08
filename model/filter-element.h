#ifndef FILTER_ELEMENT_H
#define FILTER_ELEMENT_H

#include "ns3/packet.h"
#include "ns3/object.h"
#include "ns3/ppp-header.h"
#include "ns3/ipv4-header.h"
#include "ns3/udp-header.h"
#include "ns3/tcp-header.h"

namespace ns3 {

class FilterElement : public Object {
public:
    static TypeId GetTypeId(void);
    virtual bool match(Ptr<Packet> p) const = 0;
};

class SrcIPAddress : public FilterElement {
public:
    static TypeId GetTypeId(void);
    SrcIPAddress(Ipv4Address addr);
    bool match(Ptr<Packet> p) const override;

private:
    Ipv4Address default_address;
};

class SrcMask : public FilterElement {
public:
    static TypeId GetTypeId(void);
    SrcMask(Ipv4Address addr, Ipv4Mask mask);
    bool match(Ptr<Packet> p) const override;

private:
    Ipv4Address default_address;
    Ipv4Mask default_mask;
};

class SrcPortNumber : public FilterElement {
public:
    static TypeId GetTypeId(void);
    SrcPortNumber(uint32_t port);
    bool match(Ptr<Packet> p) const override;

private:
    uint32_t default_port;
};

class DstIPAddress : public FilterElement {
public:
    static TypeId GetTypeId(void);
    DstIPAddress(Ipv4Address addr);
    bool match(Ptr<Packet> p) const override;

private:
    Ipv4Address default_address;
};

class DstMask : public FilterElement {
public:
    static TypeId GetTypeId(void);
    DstMask(Ipv4Address addr, Ipv4Mask mask);
    bool match(Ptr<Packet> p) const override;

private:
    Ipv4Address default_address;
    Ipv4Mask default_mask;
};

class DstPortNumber : public FilterElement {
public:
    static TypeId GetTypeId(void);
    DstPortNumber(uint32_t port);
    bool match(Ptr<Packet> p) const override;

private:
    uint32_t default_port;
};

class ProtocolNumber : public FilterElement {
public:
    static TypeId GetTypeId(void);
    ProtocolNumber(uint32_t protocol);
    bool match(Ptr<Packet> p) const override;

private:
    uint32_t default_protocol;
};

} // namespace ns3

#endif /* FILTER_ELEMENT_H */
