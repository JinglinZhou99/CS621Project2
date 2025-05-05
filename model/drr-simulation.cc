#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"
#include "drr.h"
#include <fstream>

using namespace ns3;

void CalculateThroughput(Ptr<FlowMonitor> monitor, std::ofstream& outFile, Time interval) {
    monitor->CheckForLostPackets();
    auto flowStats = monitor->GetFlowStats();
    for (auto it = flowStats.begin(); it != flowStats.end(); ++it) {
        double throughput = (it->second.rxBytes * 8.0) / (it->second.timeLastRxPacket.GetSeconds() - it->second.timeFirstTxPacket.GetSeconds()) / 1e6;
        outFile << Simulator::Now().GetSeconds() << " " << it->first << " " << throughput << std::endl;
    }
    Simulator::Schedule(interval, &CalculateThroughput, monitor, std::ref(outFile), interval);
}

int main(int argc, char* argv[]) {
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

    // Install DRR on router
    Ptr<DRR> drr = CreateObject<DRR>();
    drr->ReadConfigFile("drr-config.txt"); // Config file is in model/
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

    // Install applications
    uint16_t port1 = 5000;
    uint16_t port2 = 5001;
    uint16_t port3 = 5002;

    // Application 1 (quantum=3000)
    BulkSendHelper bulk1("ns3::TcpSocketFactory", InetSocketAddress(if12.GetAddress(1), port1));
    bulk1.SetAttribute("MaxBytes", UintegerValue(0));
    ApplicationContainer client1 = bulk1.Install(nodes.Get(0));
    client1.Start(Seconds(0.0));
    client1.Stop(Seconds(30.0));

    PacketSinkHelper sink1("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port1));
    ApplicationContainer server1 = sink1.Install(nodes.Get(2));
    server1.Start(Seconds(0.0));
    server1.Stop(Seconds(30.0));

    // Application 2 (quantum=2000)
    BulkSendHelper bulk2("ns3::TcpSocketFactory", InetSocketAddress(if12.GetAddress(1), port2));
    bulk2.SetAttribute("MaxBytes", UintegerValue(0));
    ApplicationContainer client2 = bulk2.Install(nodes.Get(0));
    client2.Start(Seconds(0.0));
    client2.Stop(Seconds(30.0));

    PacketSinkHelper sink2("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port2));
    ApplicationContainer server2 = sink2.Install(nodes.Get(2));
    server2.Start(Seconds(0.0));
    server2.Stop(Seconds(30.0));

    // Application 3 (quantum=1000)
    BulkSendHelper bulk3("ns3::TcpSocketFactory", InetSocketAddress(if12.GetAddress(1), port3));
    bulk3.SetAttribute("MaxBytes", UintegerValue(0));
    ApplicationContainer client3 = bulk3.Install(nodes.Get(0));
    client3.Start(Seconds(0.0));
    client3.Stop(Seconds(30.0));

    PacketSinkHelper sink3("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port3));
    ApplicationContainer server3 = sink3.Install(nodes.Get(2));
    server3.Start(Seconds(0.0));
    server3.Stop(Seconds(30.0));

    // Install FlowMonitor
    FlowMonitorHelper flowmon;
    Ptr<FlowMonitor> monitor = flowmon.InstallAll();

    // Open output file for throughput
    std::ofstream outFile("drr-throughput.txt");
    if (!outFile.is_open()) {
        std::cerr << "Failed to open drr-throughput.txt" << std::endl;
        return 1;
    }
    Simulator::Schedule(Seconds(1.0), &CalculateThroughput, monitor, std::ref(outFile), Seconds(1.0));

    // Run simulation
    Simulator::Stop(Seconds(30.0));
    Simulator::Run();
    Simulator::Destroy();

    outFile.close();
    return 0;
}