# VANET Simulation Project

## Overview
This project simulates Vehicular Ad-hoc Networks (VANETs) using NS3 and SUMO. It focuses on the communication between moving vehicles through various routing protocols, including AODV, OLSR, DSDV, and DSR. The simulation aims to analyze network behavior in dynamic vehicle movement scenarios integrated with realistic mobility patterns.

## Project Structure
```
vanet-simulation
├── src
│   ├── main.cc                  # Entry point for the simulation
│   ├── vanet-application.cc      # Implementation of the VANET application
│   ├── vanet-application.h       # Header file for the VANET application
│   ├── routing
│   │   ├── aodv-implementation.cc # AODV routing protocol implementation
│   │   ├── dsdv-implementation.cc # DSDV routing protocol implementation
│   │   ├── dsr-implementation.cc  # DSR routing protocol implementation
│   │   └── gpsr-implementation.cc # GPSR routing protocol implementation
│   └── utils
│       ├── mobility-helper.cc     # Utility functions for vehicle mobility
│       └── mobility-helper.h      # Header file for mobility utilities
├── sumo
│   ├── maps
│   │   └── city-scenario.net.xml  # Road network definition for SUMO
│   ├── routes
│   │   └── traffic.rou.xml        # Traffic routes for SUMO simulation
│   └── config
│       └── simulation.sumocfg      # SUMO simulation configuration
├── results
│   ├── metrics                    # Directory for performance metrics
│   └── visualizations             # Directory for visualization outputs
├── scripts
│   ├── run-simulation.sh          # Script to run the simulation
│   ├── analyze-results.py         # Script to analyze simulation results
│   └── sumo-to-ns3.py            # Script to convert SUMO traces to NS3 format
├── CMakeLists.txt                # Build configuration for CMake
├── wscript                        # Build configuration for Waf
└── README.md                     # Project documentation
```

## Setup Instructions
1. **Install Dependencies**: Ensure that NS3 and SUMO are installed on your system.
2. **Build the Project**: Use CMake or Waf to build the project.
   - For CMake: 
     ```
     mkdir build
     cd build
     cmake ..
     make
     ```
   - For Waf:
     ```
     ./waf configure
     ./waf build
     ```
3. **Run the Simulation**: Execute the `run-simulation.sh` script to start the simulation.
   ```
   ./scripts/run-simulation.sh
   ```

## Usage
- The simulation will generate performance metrics and visualizations in the `results` directory.
- Use `analyze-results.py` to process the generated metrics for insights into network performance.

## Goals
The primary goal of this project is to develop a comprehensive VANET simulation that demonstrates effective communication between dynamically moving vehicles, leveraging SUMO for realistic traffic flow and NS3 for network simulation. Future enhancements may include additional performance analysis tools and testing of alternative routing protocols.