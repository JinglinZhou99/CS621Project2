#ifndef DRR_H
#define DRR_H

#include "diffserv.h"
#include "traffic-class.h"
#include "ns3/ptr.h"
#include <vector>
#include <utility>

namespace ns3 {

class DRR : public DiffServ {
public:
    static TypeId GetTypeId(void);
    DRR();
    virtual ~DRR();

    virtual std::pair<uint32_t, Ptr<const Packet>> Schedule(void);
    virtual uint32_t Classify(Ptr<Packet> p);
    bool ReadConfigFile(std::string filename);
    virtual void ParseConfigLine(const std::string& line);

private:
    uint32_t currentQueue;
    std::vector<double> deficits;
};

} // namespace ns3

#endif // DRR_H
