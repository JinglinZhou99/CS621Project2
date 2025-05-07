#include "filter.h"
#include "filter-element.h"
#include "ns3/packet.h"

namespace ns3 {

Filter::Filter() {
    // Default constructor, initializes an empty vector of elements
}

Filter::~Filter() {
    // Clean up elements
    for (FilterElement* element : elements) {
        delete element;
    }
    elements.clear();
}

bool Filter::match(Ptr<Packet> p) {
    for (FilterElement* element : elements) {
        if (!element->match(p)) {
            std::cout << "Filter::match: Packet rejected by element " << (&element - &elements[0]) << std::endl;
            return false;
        }
    }
    std::cout << "Filter::match: Packet accepted" << std::endl;
    return true;
}

void Filter::AddElement(FilterElement* e) {
    elements.push_back(e);
    std::cout << "Filter::AddElement: Added element, total elements=" << elements.size() << std::endl;
}

} // namespace ns3