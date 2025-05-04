#include "filter.h"

namespace ns3 {

Filter::Filter() {}

bool Filter::match(Ptr<Packet> p) {

    // A packet matches if it satisfies all FilterElement conditions
    for (FilterElement *elem : elements) {
        if (!elem->match(p)) {
            return false;
        }
    }
    return true;
}

void Filter::AddElement(FilterElement *elem) {
    elements.push_back(elem);
}

} // namespace ns3
