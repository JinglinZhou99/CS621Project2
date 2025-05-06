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

void PacketReceivedCallback(Ptr<const Packet> packet, const Address &address) { // Updated to include Address
    std::cout << "Packet received at time " << Simulator::Now().GetSeconds() << "s, size=" << packet->GetSize() 
              << " bytes, from address " << address << std::endl;
}

void CalculateThroughput(Ptr<FlowMonitor> monitor, std::ofstream& outFile, Time interval) {
    monitor->CheckForLostPackets();
    auto flowStats = monitor->GetFlowStats();
    std::cout << "CalculateThroughput called at time: " << Simulator::Now().GetSeconds() << "s, flows: " << flowStats.size() << std::endl;
    for (auto it = flowStats.begin(); it != flowStats.end(); ++it) {
        double throughput = (it->second.rxBytes * 8.0) / (it->second.timeLastRxPacket.GetSeconds() - it->second.timeFirstTxPacket.GetSeconds()) / 1e6;
        std::cout << "Flow " << it->first << ": rxBytes=" << it->second.rxBytes << ", timeLastRx=" << it->second.timeLastRxPacket.GetSeconds() 
                  << ", timeFirstTx=" << it->second.timeFirstTxPacket.GetSeconds() << ", throughput=" << throughput << " Mbps" << std::endl;
        outFile << Simulator::Now().GetSeconds() << " " << it->first << " " << throughput << std::endl;
    }
    Simulator::Schedule(interval, &CalculateThroughput, monitor, std::ref(outFile), interval);
}

int main(int argc, char* argv[]) {
    // Parse command-line arguments for config file
    std::string configFile = "drr-config.txt"; // Default config file
    if (argc > 1) {
        configFile = argv[1]; // Use the config file path passed via command line
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

    // Trace packet transmissions and receptions
    client1.Get(0)->TraceConnectWithoutContext("Tx", MakeCallback(&PacketSentCallback));
    client2.Get(0)->TraceConnectWithoutContext("Tx", MakeCallback(&PacketSentCallback));
    client3.Get(0)->TraceConnectWithoutContext("Tx", MakeCallback(&PacketSentCallback));
    server1.Get(0)->TraceConnectWithoutContext("Rx", MakeCallback(&PacketReceivedCallback));
    server2.Get(0)->TraceConnectWithoutContext("Rx", MakeCallback(&PacketReceivedCallback));
    server3.Get(0)->TraceConnectWithoutContext("Rx", MakeCallback(&PacketReceivedCallback));

    // Enable PCAP tracing on the router device
    p2p.EnablePcap("drr-router", dev12.Get(0));

    // Install FlowMonitor
    FlowMonitorHelper flowmon;
    Ptr<FlowMonitor> monitor = flowmon.InstallAll();

    // Open output file for throughput
    std::ofstream outFile("drr-throughput.txt");
    if (!outFile.is_open()) {
        std::cerr << "Failed to open drr-throughput.txt" << std::endl;
        return 1;
    }
    Simulator::Schedule(Seconds(5.0), &CalculateThroughput, monitor, std::ref(outFile), Seconds(1.0));

    // Run simulation
    Simulator::Stop(Seconds(30.0));
    Simulator::Run();
    Simulator::Destroy();

    outFile.close();
    return 0;
}