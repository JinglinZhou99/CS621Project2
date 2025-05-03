#ifndef SOURCE_IP_ADDRESS_H
#define SOURCE_IP_ADDRESS_H

#include "filter-element.h"
#include "ns3/ipv4-address.h"

namespace ns3 {

class SourceIPAddress : public FilterElement {
public:
    static TypeId GetTypeId(void);

    SourceIPAddress();
    SourceIPAddress(Ipv4Address address);
    virtual ~SourceIPAddress();

    bool Match(Ptr<Packet> p) const override;

private:
    Ipv4Address m_address; // Source IP address to match
};

} // namespace ns3

#endif /* SOURCE_IP_ADDRESS_H */
