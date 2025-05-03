#ifndef DESTINATION_PORT_NUMBER_H
#define DESTINATION_PORT_NUMBER_H

#include "filter-element.h"

namespace ns3 {

class DestinationPortNumber : public FilterElement {
public:
    static TypeId GetTypeId(void);

    DestinationPortNumber();
    DestinationPortNumber(uint16_t port);
    virtual ~DestinationPortNumber();

    bool Match(Ptr<Packet> p) const override;

private:
    uint16_t m_port; // Destination port to match
};

} // namespace ns3

#endif /* DESTINATION_PORT_NUMBER_H */
