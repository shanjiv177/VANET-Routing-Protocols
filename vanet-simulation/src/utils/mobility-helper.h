#ifndef MOBILITY_HELPER_H
#define MOBILITY_HELPER_H

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/sumo-module.h"

namespace ns3 {

void SetupMobility(Ptr<Node> node, const std::string& traceFile);

} // namespace ns3

#endif // MOBILITY_HELPER_H