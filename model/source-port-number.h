#ifndef SOURCE_PORT_NUMBER_H
#define SOURCE_PORT_NUMBER_H

#include "filter-element.h"

namespace ns3 {

class SourcePortNumber : public FilterElement {
public:
    static TypeId GetTypeId(void);

    SourcePortNumber();
    SourcePortNumber(uint16_t port);
    virtual ~SourcePortNumber();

    bool Match(Ptr<Packet> p) const override;

private:
    uint16_t m_port; // Source port to match
};

} // namespace ns3

#endif /* SOURCE_PORT_NUMBER_H */
