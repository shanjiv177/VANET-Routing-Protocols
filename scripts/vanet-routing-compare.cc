/*
 * VANET Routing Protocol Comparison (AODV and DSDV)
 *
 * This script implements a vehicular ad-hoc network scenario to compare
 * the performance of different routing protocols in NS-3.
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/aodv-module.h"
#include "ns3/dsdv-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/netanim-module.h"
#include "ns3/config-store-module.h"
#include "ns3/olsr-module.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("VANETRoutingComparison");

// Global configuration parameters
uint32_t numVehicles = 20;   // Reduced number of vehicles for stability
uint32_t packetSize = 512;   // Smaller UDP packet size
uint32_t numPackets = 100;   // Fewer packets to send
uint32_t port = 9;           // Port number
double simTime = 90.0;       // Shorter simulation time
double dataStart = 30.0;     // Start time of data transmission
double dataEnd = 120.0;      // End time of data transmission
double roadLength = 1000.0;  // Road length in meters
double nodeSpeed = 20.0;     // Node speed in m/s
bool enableAnimation = true; // Whether to enable NetAnim output

bool useSumo = false;                        // Whether to use SUMO mobility traces
std::string traceFile = "sumo-mobility.tcl"; // SUMO-generated mobility trace file

enum RoutingProtocol
{
  AODV,
  DSDV,
  OLSR
};

// Function to configure the routing protocol
void ConfigureRoutingProtocol(NodeContainer &nodes, RoutingProtocol protocol)
{
  InternetStackHelper internet;
  switch (protocol)
  {
  case AODV:
  {
    AodvHelper aodv;
    internet.SetRoutingHelper(aodv);
    internet.Install(nodes);
    std::cout << "Using AODV routing protocol" << std::endl;
    break;
  }
  case DSDV:
  {
    DsdvHelper dsdv;
    dsdv.Set("PeriodicUpdateInterval", TimeValue(Seconds(2.0))); // More frequent updates
    dsdv.Set("SettlingTime", TimeValue(Seconds(6.0)));           // Shorter settling time
    internet.SetRoutingHelper(dsdv);
    internet.Install(nodes);
    std::cout << "Using DSDV routing protocol" << std::endl;
    break;
  }
  case OLSR:
  {
    OlsrHelper olsr;
    // Configure OLSR parameters for VANET scenario
    olsr.Set("HelloInterval", TimeValue(Seconds(1.0)));
    olsr.Set("TcInterval", TimeValue(Seconds(3.0)));
    olsr.Set("Willingness", EnumValue(7));
    internet.SetRoutingHelper(olsr);
    internet.Install(nodes);
    std::cout << "Using OLSR routing protocol" << std::endl;
    break;
  }
  default:
    NS_FATAL_ERROR("Invalid routing protocol");
  }
}

// Configure mobility model for vehicles
void ConfigureVehicleMobility(NodeContainer &nodes)
{
  if (useSumo)
  {
    // Use SUMO-generated trace file
    Ns2MobilityHelper mobility = Ns2MobilityHelper(traceFile);
    mobility.Install();
    std::cout << "Using SUMO mobility traces from file: " << traceFile << std::endl;
  }
  else
  {
    // Use existing NS-3 native mobility
    MobilityHelper mobility;

    if (nodes.GetN() < 2)
    {
      NS_FATAL_ERROR("Need at least 2 vehicles for the simulation");
    }

    // Use grid position allocator for initial positions
    uint32_t gridWidth = std::max(1u, numVehicles / 2);
    mobility.SetPositionAllocator("ns3::GridPositionAllocator",
                                  "MinX", DoubleValue(0.0),
                                  "MinY", DoubleValue(0.0),
                                  "DeltaX", DoubleValue(roadLength / gridWidth),
                                  "DeltaY", DoubleValue(5.0),
                                  "GridWidth", UintegerValue(gridWidth),
                                  "LayoutType", StringValue("RowFirst"));

    // Using constant velocity for more stability
    mobility.SetMobilityModel("ns3::ConstantVelocityMobilityModel");
    mobility.Install(nodes);

    // Set velocity for each node
    for (uint32_t i = 0; i < nodes.GetN(); i++)
    {
      Ptr<ConstantVelocityMobilityModel> model = nodes.Get(i)->GetObject<ConstantVelocityMobilityModel>();
      if (model)
      {
        // Alternate vehicle directions for more diverse movement
        double direction = (i % 2 == 0) ? 1.0 : -1.0;
        model->SetVelocity(Vector(nodeSpeed * direction, 0, 0));
      }
    }
  }
}

// Function to install WiFi devices on the nodes
NetDeviceContainer InstallWifiDevices(NodeContainer &nodes)
{
  WifiHelper wifi;
  wifi.SetStandard(WIFI_STANDARD_80211g);

  YansWifiPhyHelper wifiPhy;
  YansWifiChannelHelper wifiChannel;
  wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
  wifiChannel.AddPropagationLoss("ns3::RangePropagationLossModel",
                                 "MaxRange", DoubleValue(250.0)); // Limit range to avoid PHY issues

  wifiPhy.SetChannel(wifiChannel.Create());

  // Set transmit power and gain values
  wifiPhy.Set("TxPowerStart", DoubleValue(20.0));
  wifiPhy.Set("TxPowerEnd", DoubleValue(20.0));
  wifiPhy.Set("TxGain", DoubleValue(1.0));
  wifiPhy.Set("RxGain", DoubleValue(1.0));

  // Use constant rate manager with a reliable rate
  wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager",
                               "DataMode", StringValue("ErpOfdmRate6Mbps"),
                               "ControlMode", StringValue("ErpOfdmRate6Mbps"));

  WifiMacHelper wifiMac;
  wifiMac.SetType("ns3::AdhocWifiMac");

  NetDeviceContainer devices = wifi.Install(wifiPhy, wifiMac, nodes);
  return devices;
}

// Configure IP addressing
Ipv4InterfaceContainer ConfigureAddressing(NodeContainer &nodes, NetDeviceContainer &devices)
{
  Ipv4AddressHelper ipv4;
  ipv4.SetBase("10.1.1.0", "255.255.255.0");
  return ipv4.Assign(devices);
}

// Setup traffic application - UDP client/server
void SetupTrafficApplication(NodeContainer &nodes, Ipv4InterfaceContainer &interfaces)
{
  if (nodes.GetN() < 2)
  {
    NS_FATAL_ERROR("Need at least 2 nodes for client-server communication");
  }

  for (uint32_t i = 0; i < numVehicles / 2; i++)
  {
    uint32_t sourceNode = i;
    uint32_t destNode = (i + numVehicles / 2) % numVehicles; // Pair nodes
    UdpClientHelper client(interfaces.GetAddress(destNode), port + i);
    client.SetAttribute("MaxPackets", UintegerValue(numPackets));
    client.SetAttribute("Interval", TimeValue(Seconds(0.5)));
    client.SetAttribute("PacketSize", UintegerValue(packetSize));
    ApplicationContainer clientApps = client.Install(nodes.Get(sourceNode));
    clientApps.Start(Seconds(dataStart));
    clientApps.Stop(Seconds(dataEnd));

    UdpServerHelper server(port + i);
    ApplicationContainer serverApps = server.Install(nodes.Get(destNode));
    serverApps.Start(Seconds(dataStart));
    serverApps.Stop(Seconds(dataEnd));
  }
}

// Monitor performance metrics using FlowMonitor
void MonitorPerformance(Ptr<FlowMonitor> &flowMonitor, FlowMonitorHelper &flowHelper, std::string protocol)
{
  if (!flowMonitor)
  {
    std::cerr << "Error: Flow monitor not properly initialized" << std::endl;
    return;
  }

  // Print per flow statistics
  flowMonitor->CheckForLostPackets();

  // Create a file to store the results
  std::string filename = "vanet-" + protocol + "-results.txt";
  std::ofstream outFile(filename.c_str(), std::ios::out);
  if (!outFile.is_open())
  {
    std::cerr << "Error: Could not open output file " << filename << std::endl;
    return;
  }

  // Calculate metrics
  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(flowHelper.GetClassifier());
  if (!classifier)
  {
    std::cerr << "Error: Could not retrieve flow classifier" << std::endl;
    outFile.close();
    return;
  }

  std::map<FlowId, FlowMonitor::FlowStats> stats = flowMonitor->GetFlowStats();

  double totalThroughput = 0.0;
  double totalDelay = 0.0;
  uint32_t totalRxPackets = 0;
  uint32_t totalTxPackets = 0;
  uint32_t totalLostPackets = 0;

  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin(); i != stats.end(); ++i)
  {
    Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow(i->first);

    // Skip if no packets were received
    if (i->second.rxPackets == 0)
    {
      continue;
    }

    // Retrieve flow duration
    double flowDuration = i->second.timeLastRxPacket.GetSeconds() - i->second.timeFirstTxPacket.GetSeconds();
    if (flowDuration <= 0)
    {
      flowDuration = 0.001; // Avoid division by zero
    }

    // Calculate throughput in Kbps
    double throughput = (i->second.rxBytes * 8.0) / (flowDuration) / 1000;

    // Store results in file
    outFile << "Flow " << i->first << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")\n";
    outFile << "  Tx Packets: " << i->second.txPackets << "\n";
    outFile << "  Rx Packets: " << i->second.rxPackets << "\n";
    outFile << "  Lost Packets: " << i->second.lostPackets << "\n";

    if (i->second.txPackets > 0)
    {
      outFile << "  Packet Delivery Ratio: " << (double)i->second.rxPackets / i->second.txPackets * 100 << "%\n";
    }
    else
    {
      outFile << "  Packet Delivery Ratio: N/A\n";
    }

    if (i->second.rxPackets > 0)
    {
      outFile << "  Average Delay: " << i->second.delaySum.GetSeconds() / i->second.rxPackets * 1000 << " ms\n";
    }
    else
    {
      outFile << "  Average Delay: N/A\n";
    }

    outFile << "  Throughput: " << throughput << " Kbps\n\n";

    // Update totals
    totalThroughput += throughput;
    totalDelay += i->second.delaySum.GetSeconds();
    totalRxPackets += i->second.rxPackets;
    totalTxPackets += i->second.txPackets;
    totalLostPackets += i->second.lostPackets;
  }

  // Print aggregate statistics
  outFile << "===== Aggregate Statistics =====\n";
  outFile << "Total Throughput: " << totalThroughput << " Kbps\n";
  if (totalRxPackets > 0)
  {
    outFile << "Average Delay: " << totalDelay / totalRxPackets * 1000 << " ms\n";
  }
  else
  {
    outFile << "Average Delay: N/A (no packets received)\n";
  }

  if (totalTxPackets > 0)
  {
    outFile << "Packet Delivery Ratio: " << (double)totalRxPackets / totalTxPackets * 100 << "%\n";
    outFile << "Packet Loss Ratio: " << (double)totalLostPackets / totalTxPackets * 100 << "%\n";
  }
  else
  {
    outFile << "Packet Delivery Ratio: N/A (no packets transmitted)\n";
    outFile << "Packet Loss Ratio: N/A (no packets transmitted)\n";
  }

  // Print to console as well
  std::cout << "Results for " << protocol << " have been saved to " << filename << std::endl;
  std::cout << "  Total Throughput: " << totalThroughput << " Kbps" << std::endl;
  if (totalRxPackets > 0)
  {
    std::cout << "  Average Delay: " << totalDelay / totalRxPackets * 1000 << " ms" << std::endl;
  }
  else
  {
    std::cout << "  Average Delay: N/A (no packets received)" << std::endl;
  }

  if (totalTxPackets > 0)
  {
    std::cout << "  Packet Delivery Ratio: " << (double)totalRxPackets / totalTxPackets * 100 << "%" << std::endl;
  }
  else
  {
    std::cout << "  Packet Delivery Ratio: N/A (no packets transmitted)" << std::endl;
  }

  outFile.close();
}

void PrintNodePositions(NodeContainer &nodes)
{
  std::cout << "Node positions at " << Simulator::Now().GetSeconds() << "s:" << std::endl;
  for (uint32_t i = 0; i < std::min(5u, nodes.GetN()); i++)
  {
    Ptr<MobilityModel> mobility = nodes.Get(i)->GetObject<MobilityModel>();
    if (mobility)
    {
      Vector pos = mobility->GetPosition();
      std::cout << "Node " << i << ": (" << pos.x << ", " << pos.y << ")" << std::endl;
    }
  }
}

void RunSimulation(RoutingProtocol protocol)
{
  // Set simulation seed for reproducibility
  RngSeedManager::SetSeed(12345);
  RngSeedManager::SetRun(1);

  // Create nodes
  NodeContainer nodes;
  nodes.Create(numVehicles);

  if (nodes.GetN() < 2)
  {
    NS_FATAL_ERROR("Need at least 2 vehicles for the simulation");
  }

  // Configure mobility
  ConfigureVehicleMobility(nodes);

  // Install routing protocol
  ConfigureRoutingProtocol(nodes, protocol);

  // Install WiFi devices
  NetDeviceContainer devices = InstallWifiDevices(nodes);

  // Configure IP addressing
  Ipv4InterfaceContainer interfaces = ConfigureAddressing(nodes, devices);

  if (useSumo)
  {
    Simulator::Schedule(Seconds(1.0), &PrintNodePositions, nodes);
  }

  // Setup traffic application
  SetupTrafficApplication(nodes, interfaces);

  // Setup flow monitor
  FlowMonitorHelper flowHelper;
  Ptr<FlowMonitor> flowMonitor = flowHelper.InstallAll();

  // Setup animation if enabled
  AnimationInterface *anim = nullptr; // Use raw pointer, not Ptr<>
  std::string protocolName;
  std::string animFile;
  switch (protocol)
  {
  case AODV:
    protocolName = "AODV";
    animFile = "vanet-animation-aodv.xml";
    break;
  case DSDV:
    protocolName = "DSDV";
    animFile = "vanet-animation-dsdv.xml";
    break;
  case OLSR:
    protocolName = "OLSR";
    animFile = "vanet-animation-olsr.xml";
    break;
  default:
    protocolName = "Unknown";
  }

  if (enableAnimation)
  {
    try
    {
      anim = new AnimationInterface(animFile); // Direct constructor call

      if (anim)
      {
        for (uint32_t i = 0; i < nodes.GetN(); i++)
        {
          anim->UpdateNodeDescription(nodes.Get(i), "V" + std::to_string(i));
          anim->UpdateNodeColor(nodes.Get(i), 255, 0, 0);
        }
        std::cout << "Animation enabled, output file: " << animFile << std::endl;
      }
    }
    catch (const std::exception &e)
    {
      std::cerr << "Warning: Could not initialize animation: " << e.what() << std::endl;
      std::cerr << "Simulation will continue without animation" << std::endl;
    }
  }

  // Run the simulation
  std::cout << "Running simulation with " << protocolName << " routing protocol..." << std::endl;

  Simulator::Stop(Seconds(simTime));

  try
  {
    Simulator::Run();

    // Generate performance statistics
    MonitorPerformance(flowMonitor, flowHelper, protocolName);
  }
  catch (const std::exception &e)
  {
    std::cerr << "Error during simulation run: " << e.what() << std::endl;
  }

  if (anim)
  {
    delete anim; // Explicitly delete the animation interface
    anim = nullptr;
  }

  Simulator::Destroy();
}

int main(int argc, char *argv[])
{
  // Configure command line parameters
  CommandLine cmd(__FILE__);
  int protocolChoice = 0;
  cmd.AddValue("numVehicles", "Number of vehicles (minimum 2)", numVehicles);
  cmd.AddValue("simTime", "Simulation time in seconds", simTime);
  cmd.AddValue("packetSize", "Size of UDP packets", packetSize);
  cmd.AddValue("protocol", "Routing protocol (1=AODV, 2=DSDV, 3=OLSR 0=All)", protocolChoice);
  cmd.AddValue("animation", "Enable NetAnim output (0=off, 1=on)", enableAnimation);
  cmd.AddValue("useSumo", "Use SUMO mobility traces (0=off, 1=on)", useSumo);
  cmd.AddValue("traceFile", "SUMO mobility trace file path", traceFile);
  cmd.Parse(argc, argv);

  // Validate parameters
  if (numVehicles < 2)
  {
    std::cerr << "Error: Need at least 2 vehicles for the simulation" << std::endl;
    return 1;
  }

  if (simTime <= 0)
  {
    std::cerr << "Error: Simulation time must be positive" << std::endl;
    return 1;
  }

  if (dataStart >= dataEnd || dataEnd > simTime)
  {
    std::cerr << "Warning: Data timing might be invalid. Adjusting..." << std::endl;
    dataStart = std::min(dataStart, simTime * 0.1);
    dataEnd = std::min(dataEnd, simTime * 0.9);
  }

  // Enable logging for debugging if needed
  // LogComponentEnable("VANETRoutingComparison", LOG_LEVEL_INFO);
  // LogComponentEnable("UdpClient", LOG_LEVEL_INFO);
  // LogComponentEnable("UdpServer", LOG_LEVEL_INFO);

  try
  {
    if (protocolChoice == 1)
    {
      RunSimulation(AODV);
    }
    else if (protocolChoice == 2)
    {
      RunSimulation(DSDV);
    }
    else if (protocolChoice == 3)
    {
      RunSimulation(OLSR);
    }
    else
    {
      // Run all supported protocols
      std::cout << "=== Running simulation with AODV ===" << std::endl;
      RunSimulation(AODV);

      // Reset between simulations
      std::cout << "\n"
                << std::endl;

      std::cout << "=== Running simulation with DSDV ===" << std::endl;
      RunSimulation(DSDV);

      std::cout << "\n"
                << std::endl;

      std::cout << "=== Running simulation with OLSR ===" << std::endl;
      RunSimulation(OLSR);
    }

    return 0;
  }
  catch (const std::exception &e)
  {
    std::cerr << "Exception during simulation: " << e.what() << std::endl;
    return 1;
  }
}