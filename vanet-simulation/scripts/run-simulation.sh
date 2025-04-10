#!/bin/bash

# Navigate to the SUMO directory and start the SUMO simulation
cd ../sumo
sumo-gui -c config/simulation.sumocfg &

# Wait for a few seconds to ensure SUMO starts properly
sleep 5

# Navigate back to the project root and run the NS3 simulation
cd ../src
./waf --run main

# Optionally, you can add commands to analyze results after the simulation
# python ../scripts/analyze-results.py

# End of script