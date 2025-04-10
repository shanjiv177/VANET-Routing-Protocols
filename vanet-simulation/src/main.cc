#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/udp-socket-factory.h"
#include "ns3/flow-monitor-module.h"
#include "vanet-application.h"
#include "utils/mobility-helper.h"

using namespace ns3;

int main(int argc, char *argv[]) {
    // Set default parameters
    CommandLine cmd;
    cmd.Parse(argc, argv);

    // Create the NS3 simulation environment
    NodeContainer vehicles;
    vehicles.Create(50); // Create 50 vehicles

    // Set up mobility using SUMO
    MobilityHelper mobility;
    mobility.SetMobilityModel("ns3::MobilityModel", "Trace", StringValue("path/to/sumo/traces"));
    mobility.Install(vehicles);

    // Install Internet stack
    InternetStackHelper internet;
    internet.Install(vehicles);

    // Create and configure VANET applications
    for (uint32_t i = 0; i < vehicles.GetN(); ++i) {
        Ptr<VANETApplication> app = CreateObject<VANETApplication>();
        vehicles.Get(i)->AddApplication(app);
        app->SetStartTime(Seconds(1.0 + i * 0.1)); // Staggered start times
        app->SetStopTime(Seconds(100.0));
    }

    // Set up FlowMonitor to analyze performance
    FlowMonitorHelper flowMonitor;
    Ptr<FlowMonitor> monitor = flowMonitor.InstallAll();

    // Start the simulation
    Simulator::Run();

    // Analyze results
    monitor->SerializeToXmlFile("results/metrics/flow-monitor.xml", true, true);

    // Clean up and exit
    Simulator::Destroy();
    return 0;
}