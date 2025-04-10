#include "mobility-helper.h"
#include "ns3/sumo-helper.h"
#include "ns3/mobility-helper.h"
#include "ns3/node-container.h"
#include "ns3/ptr.h"
#include "ns3/simulator.h"

using namespace ns3;

void SetupMobility(NodeContainer& vehicles, const std::string& sumoConfigFile) {
    // Create a SUMO helper to manage the SUMO simulation
    SumoHelper sumoHelper;
    sumoHelper.SetSumoConfigFile(sumoConfigFile);

    // Set up mobility for the vehicles using SUMO
    MobilityHelper mobility;
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(vehicles);

    // Start the SUMO simulation
    sumoHelper.Start();
}

void StopMobility() {
    // Stop the SUMO simulation
    Simulator::Stop();
}