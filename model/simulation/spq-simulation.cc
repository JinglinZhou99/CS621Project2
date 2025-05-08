#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"
#include "spq.h"
#include <fstream>

using namespace ns3;

void PacketSentCallback(Ptr<const Packet> packet) {
    std::cout << "Packet sent at time " << Simulator::Now().GetSeconds() << "s, size=" << packet->GetSize() << " bytes" << std::endl;
}

void PacketReceivedCallback(Ptr<const Packet> packet, const Address &address) {
    std::cout << "Packet received at time " << Simulator::Now().GetSeconds() << "s, size=" << packet->GetSize() 
              << " bytes, from address " << address << std::endl;
}

int main(int argc, char* argv[]) {
    // Parse command-line arguments for config file
    std::string configFile = "spq-config.txt";
    if (argc > 1) {
        configFile = argv[1];
    }

    // Create nodes
    NodeContainer nodes;
    nodes.Create(3); // Two end-hosts and one router

    // Create point-to-point links
    PointToPointHelper p2p;
    p2p.SetDeviceAttribute("DataRate", StringValue("4Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue("2ms"));
    NetDeviceContainer dev01 = p2p.Install(nodes.Get(0), nodes.Get(1)); // Host1 to Router

    p2p.SetDeviceAttribute("DataRate", StringValue("1Mbps"));
    NetDeviceContainer dev12 = p2p.Install(nodes.Get(1), nodes.Get(2)); // Router to Host2

    // Install Internet stack
    InternetStackHelper stack;
    stack.Install(nodes);

    // Install SPQ on router
    Ptr<SPQ> spq = CreateObject<SPQ>();
    if (!spq->ReadConfigFile(configFile)) {
        std::cerr << "Failed to read SPQ config file: " << configFile << std::endl;
        return 1;
    }
    Ptr<PointToPointNetDevice> routerDev = DynamicCast<PointToPointNetDevice>(dev12.Get(0));
    if (!routerDev) {
        std::cerr << "Failed to cast NetDevice to PointToPointNetDevice" << std::endl;
        return 1;
    }
    routerDev->SetQueue(spq);

    // Assign IP addresses
    Ipv4AddressHelper ipv4;
    ipv4.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer if01 = ipv4.Assign(dev01);
    ipv4.SetBase("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer if12 = ipv4.Assign(dev12);

    // Enable global routing
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    // Install applications
    uint16_t portLow = 7000;
    uint16_t portHigh = 9000;

    // Application Low Priority (port 7000, starts at t=1)
    OnOffHelper onOffLow("ns3::UdpSocketFactory", InetSocketAddress(if12.GetAddress(1), portLow));
    onOffLow.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
    onOffLow.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
    onOffLow.SetAttribute("DataRate", StringValue("4Mbps")); // Send at a rate higher than link capacity to ensure saturation
    onOffLow.SetAttribute("PacketSize", UintegerValue(1024));
    ApplicationContainer clientLow = onOffLow.Install(nodes.Get(0));
    clientLow.Start(Seconds(1.0));
    clientLow.Stop(Seconds(30.0));

    PacketSinkHelper sinkLow("ns3::UdpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), portLow));
    ApplicationContainer serverLow = sinkLow.Install(nodes.Get(2));
    serverLow.Start(Seconds(0.0));
    serverLow.Stop(Seconds(30.0));

    // Application High Priority (port 9000, starts at t=14, stops at t=17)
    OnOffHelper onOffHigh("ns3::UdpSocketFactory", InetSocketAddress(if12.GetAddress(1), portHigh));
    onOffHigh.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
    onOffHigh.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
    onOffHigh.SetAttribute("DataRate", StringValue("4Mbps")); // Send at a rate higher than link capacity to ensure saturation
    onOffHigh.SetAttribute("PacketSize", UintegerValue(1024));
    ApplicationContainer clientHigh = onOffHigh.Install(nodes.Get(0));
    clientHigh.Start(Seconds(14.0));
    clientHigh.Stop(Seconds(17.0));

    PacketSinkHelper sinkHigh("ns3::UdpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), portHigh));
    ApplicationContainer serverHigh = sinkHigh.Install(nodes.Get(2));
    serverHigh.Start(Seconds(0.0));
    serverHigh.Stop(Seconds(30.0));

    // Trace packet transmissions and receptions
    clientLow.Get(0)->TraceConnectWithoutContext("Tx", MakeCallback(&PacketSentCallback));
    clientHigh.Get(0)->TraceConnectWithoutContext("Tx", MakeCallback(&PacketSentCallback));
    serverLow.Get(0)->TraceConnectWithoutContext("Rx", MakeCallback(&PacketReceivedCallback));
    serverHigh.Get(0)->TraceConnectWithoutContext("Rx", MakeCallback(&PacketReceivedCallback));

    // Enable PCAP tracing
    p2p.EnablePcap("prespq", dev01.Get(0)); // Packets entering the router (Host1 to Router)
    p2p.EnablePcap("postspq", dev12.Get(1)); // Packets leaving the router (Router to Host2)

    // Install FlowMonitor
    FlowMonitorHelper flowmonHelper;
    Ptr<FlowMonitor> monitor = flowmonHelper.InstallAll();

    // Run simulation
    Simulator::Stop(Seconds(30.0));
    Simulator::Run();
    Simulator::Destroy();

    return 0;
}
