#ifndef FILTER_H
#define FILTER_H

#include "ns3/packet.h"
#include "ns3/object.h"
#include "filter-element.h"
#include <vector>

namespace ns3 {

class Filter : public Object {
public:
    static TypeId GetTypeId(void);

    Filter();
    virtual ~Filter();

    // Check if the packet matches all FilterElement conditions
    bool Match(Ptr<Packet> p) const;

    // Add a FilterElement to the collection
    void AddElement(Ptr<FilterElement> element);

private:
    std::vector<Ptr<FilterElement>> elements; // Collection of FilterElement conditions
};

} // namespace ns3

#endif /* FILTER_H */
