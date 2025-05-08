#include "filter.h"
#include "filter-element.h"
#include "ns3/packet.h"

namespace ns3 {

Filter::Filter() {}

Filter::~Filter() {
    for (FilterElement* element : elements) {
        delete element;
    }
    elements.clear();
}

/**
 * @brief Checks if a packet matches all filter elements.
 *
 * Iterates through the list of filter elements and evaluates the packet against each one.
 * If any element rejects the packet, the method returns false. Logs the outcome of the match.
 *
 * @param p Pointer to the packet to be evaluated.
 * @return True if the packet matches all filter elements, false otherwise.
 */
bool Filter::match(Ptr<Packet> p) {
    for (size_t i = 0; i < elements.size(); ++i) {
        if (!elements[i]->match(p)) {
            std::cout << "Filter::match: Packet rejected by element " << i << std::endl;
            return false;
        }
    }
    std::cout << "Filter::match: Packet accepted" << std::endl;
    return true;
}

/**
 * @brief Adds a filter element to the filter.
 *
 * Appends the provided filter element to the list of elements and logs the updated element count.
 *
 * @param e Pointer to the filter element to be added.
 */
void Filter::AddElement(FilterElement* e) {
    elements.push_back(e);
    std::cout << "Filter::AddElement: Added element, total elements=" << elements.size() << std::endl;
}

} // namespace ns3
