#ifndef DRR_H
#define DRR_H

#include "diffserv.h"
#include <string>
#include <vector>

namespace ns3 {

class DRR : public DiffServ {
public:
    DRR();
    virtual ~DRR();
    static TypeId GetTypeId(void);
    void ReadConfigFile(std::string filename);

protected:
    virtual Ptr<const Packet> Schedule(void) const override;
    virtual uint32_t Classify(Ptr<Packet> p) override;

private:
    void ParseConfigLine(const std::string& line);
    mutable std::vector<uint32_t> deficits; // Deficit counters for each queue
    mutable uint32_t currentQueue; // Current queue index for round-robin
};

} // namespace ns3

#endif /* DRR_H */