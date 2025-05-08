#ifndef SPQ_H
#define SPQ_H

#include "diffserv.h"
#include "traffic-class.h"
#include "ns3/ptr.h"
#include <utility> // For std::pair

namespace ns3 {

class SPQ : public DiffServ {
public:
    static TypeId GetTypeId(void);
    SPQ();
    virtual ~SPQ();

    virtual std::pair<uint32_t, Ptr<const Packet>> Schedule(void);
    virtual uint32_t Classify(Ptr<Packet> p);
    bool ReadConfigFile(std::string filename);
    virtual void ParseConfigLine(const std::string& line);

private:
};

} // namespace ns3

#endif // SPQ_H