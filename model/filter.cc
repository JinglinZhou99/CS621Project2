#include "filter.h"
#include "ns3/log.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE("Filter");

NS_OBJECT_ENSURE_REGISTERED(Filter);

TypeId Filter::GetTypeId(void) {
    static TypeId tid = TypeId("ns3::Filter")
        .SetParent<Object>()
        .SetGroupName("Network")
        .AddConstructor<Filter>();
    return tid;
}

Filter::Filter() {
    NS_LOG_FUNCTION(this);
}

Filter::~Filter() {
    NS_LOG_FUNCTION(this);
    elements.clear();
}

bool Filter::Match(Ptr<Packet> p) const {
    NS_LOG_FUNCTION(this << p);

    // A packet matches if it satisfies all FilterElement conditions
    for (const auto& element : elements) {
        if (!element->Match(p)) {
            return false;
        }
    }
    return true;
}

void Filter::AddElement(Ptr<FilterElement> element) {
    elements.push_back(element);
}

} // namespace ns3
