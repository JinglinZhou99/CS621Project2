#ifndef DESTINATION_IP_ADDRESS_H
#define DESTINATION_IP_ADDRESS_H

#include "filter-element.h"
#include "ns3/ipv4-address.h"

namespace ns3 {

class DestinationIPAddress : public FilterElement {
public:
    static TypeId GetTypeId(void);

    DestinationIPAddress();
    DestinationIPAddress(Ipv4Address address);
    virtual ~DestinationIPAddress();

    bool Match(Ptr<Packet> p) const override;

private:
    Ipv4Address m_address; // Destination IP address to match
};

} // namespace ns3

#endif /* DESTINATION_IP_ADDRESS_H */
