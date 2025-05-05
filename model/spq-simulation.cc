#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"
#include "spq.h"
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

    // Install SPQ on router
    Ptr<SPQ> spq = CreateObject<SPQ>();
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

    // Install applications
    uint16_t portA = 5000;
    uint16_t portB = 5001;

    // Application B (low priority, starts at t=0)
    BulkSendHelper bulkB("ns3::TcpSocketFactory", InetSocketAddress(if12.GetAddress(1), portB));
    bulkB.SetAttribute("MaxBytes", UintegerValue(0));
    ApplicationContainer clientB = bulkB.Install(nodes.Get(0));
    clientB.Start(Seconds(0.0));
    clientB.Stop(Seconds(30.0));

    PacketSinkHelper sinkB("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), portB));
    ApplicationContainer serverB = sinkB.Install(nodes.Get(2));
    serverB.Start(Seconds(0.0));
    serverB.Stop(Seconds(30.0));

    // Application A (high priority, starts at t=12)
    BulkSendHelper bulkA("ns3::TcpSocketFactory", InetSocketAddress(if12.GetAddress(1), portA));
    bulkA.SetAttribute("MaxBytes", UintegerValue(0));
    ApplicationContainer clientA = bulkA.Install(nodes.Get(0));
    clientA.Start(Seconds(12.0));
    clientA.Stop(Seconds(24.0));

    PacketSinkHelper sinkA("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), portA));
    ApplicationContainer serverA = sinkA.Install(nodes.Get(2));
    serverA.Start(Seconds(0.0));
    serverA.Stop(Seconds(30.0));

    // Install FlowMonitor
    FlowMonitorHelper flowmon;
    Ptr<FlowMonitor> monitor = flowmon.InstallAll();

    // Open output file for throughput
    std::ofstream outFile("spq-throughput.txt");
    if (!outFile.is_open()) {
        std::cerr << "Failed to open spq-throughput.txt" << std::endl;
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