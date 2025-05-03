#ifndef PROTOCOL_NUMBER_H
#define PROTOCOL_NUMBER_H

#include "filter-element.h"

namespace ns3 {

class ProtocolNumber : public FilterElement {
public:
    static TypeId GetTypeId(void);

    ProtocolNumber();
    ProtocolNumber(uint8_t protocol);
    virtual ~ProtocolNumber();

    bool Match(Ptr<Packet> p) const override;

private:
    uint8_t m_protocol; // Protocol number to match (e.g., 6 for TCP, 17 for UDP)
};

} // namespace ns3

#endif /* PROTOCOL_NUMBER_H */
