#ifndef FILTER_H
#define FILTER_H

#include "ns3/packet.h"
#include "ns3/object.h"
#include "filter-element.h"
#include <vector>

namespace ns3 {

class Filter : public Object {
public:
    Filter();
    bool match(Ptr<Packet> p);
    void AddElement(FilterElement* elem);

    std::vector<FilterElement*> elements;
};

} // namespace ns3

#endif /* FILTER_H */
