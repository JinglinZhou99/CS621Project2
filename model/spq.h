#ifndef SPQ_H
#define SPQ_H

#include "diffserv.h"
#include <string>

namespace ns3 {

class SPQ : public DiffServ {
public:
    SPQ();
    virtual ~SPQ();
    static TypeId GetTypeId(void);
    void ReadConfigFile(std::string filename);

protected:
    virtual Ptr<const Packet> Schedule(void) const override;
    virtual uint32_t Classify(Ptr<Packet> p) override;

private:
    void ParseConfigLine(const std::string& line);
};

} // namespace ns3

#endif /* SPQ_H */