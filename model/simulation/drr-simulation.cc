#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"
#include "drr.h"
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
    std::string configFile = "drr-config.txt";
    if (argc > 1) {
        configFile = argv[1];
    }

    // Create nodes
    NodeContainer nodes;
    nodes.Create(3);

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

    // Install DRR on router
    Ptr<DRR> drr = CreateObject<DRR>();
    if (!drr->ReadConfigFile(configFile)) {
        std::cerr << "Failed to read DRR config file: " << configFile << std::endl;
        return 1;
    }
    Ptr<PointToPointNetDevice> routerDev = DynamicCast<PointToPointNetDevice>(dev12.Get(0));
    if (!routerDev) {
        std::cerr << "Failed to cast NetDevice to PointToPointNetDevice" << std::endl;
        return 1;
    }
    routerDev->SetQueue(drr);

    // Assign IP addresses
    Ipv4AddressHelper ipv4;
    ipv4.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer if01 = ipv4.Assign(dev01);
    ipv4.SetBase("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer if12 = ipv4.Assign(dev12);

    // Enable global routing
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    // Install applications
    uint16_t port1 = 6000; // Weight 100
    uint16_t port2 = 7000; // Weight 200
    uint16_t port3 = 9000; // Weight 300

    // Application 1 (port 6000, weight 100)
    OnOffHelper onOff1("ns3::UdpSocketFactory", InetSocketAddress(if12.GetAddress(1), port1));
    onOff1.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
    onOff1.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
    onOff1.SetAttribute("DataRate", StringValue("2Mbps"));
    onOff1.SetAttribute("PacketSize", UintegerValue(1024));
    ApplicationContainer client1 = onOff1.Install(nodes.Get(0));
    client1.Start(Seconds(1.0));
    client1.Stop(Seconds(30.0));

    PacketSinkHelper sink1("ns3::UdpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port1));
    ApplicationContainer server1 = sink1.Install(nodes.Get(2));
    server1.Start(Seconds(0.0));
    server1.Stop(Seconds(30.0));

    // Application 2 (port 7000, weight 200)
    OnOffHelper onOff2("ns3::UdpSocketFactory", InetSocketAddress(if12.GetAddress(1), port2));
    onOff2.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
    onOff2.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
    onOff2.SetAttribute("DataRate", StringValue("2Mbps"));
    onOff2.SetAttribute("PacketSize", UintegerValue(1024));
    ApplicationContainer client2 = onOff2.Install(nodes.Get(0));
    client2.Start(Seconds(1.0));
    client2.Stop(Seconds(30.0));

    PacketSinkHelper sink2("ns3::UdpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port2));
    ApplicationContainer server2 = sink2.Install(nodes.Get(2));
    server2.Start(Seconds(0.0));
    server2.Stop(Seconds(30.0));

    // Application 3 (port 9000, weight 300)
    OnOffHelper onOff3("ns3::UdpSocketFactory", InetSocketAddress(if12.GetAddress(1), port3));
    onOff3.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
    onOff3.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
    onOff3.SetAttribute("DataRate", StringValue("2Mbps"));
    onOff3.SetAttribute("PacketSize", UintegerValue(1024));
    ApplicationContainer client3 = onOff3.Install(nodes.Get(0));
    client3.Start(Seconds(1.0));
    client3.Stop(Seconds(30.0));

    PacketSinkHelper sink3("ns3::UdpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port3));
    ApplicationContainer server3 = sink3.Install(nodes.Get(2));
    server3.Start(Seconds(0.0));
    server3.Stop(Seconds(30.0));

    // Trace packet transmissions and receptions
    client1.Get(0)->TraceConnectWithoutContext("Tx", MakeCallback(&PacketSentCallback));
    client2.Get(0)->TraceConnectWithoutContext("Tx", MakeCallback(&PacketSentCallback));
    client3.Get(0)->TraceConnectWithoutContext("Tx", MakeCallback(&PacketSentCallback));
    server1.Get(0)->TraceConnectWithoutContext("Rx", MakeCallback(&PacketReceivedCallback));
    server2.Get(0)->TraceConnectWithoutContext("Rx", MakeCallback(&PacketReceivedCallback));
    server3.Get(0)->TraceConnectWithoutContext("Rx", MakeCallback(&PacketReceivedCallback));

    // Enable PCAP tracing
    p2p.EnablePcap("predrr", dev01.Get(0));
    p2p.EnablePcap("postdrr", dev12.Get(1));

    // Install FlowMonitor
    FlowMonitorHelper flowmonHelper;
    Ptr<FlowMonitor> monitor = flowmonHelper.InstallAll();

    // Run simulation
    Simulator::Stop(Seconds(30.0));
    Simulator::Run();
    Simulator::Destroy();

    return 0;
}
