# VANET Routing Protocol Comparison

This project implements and evaluates different routing protocols for Vehicular Ad Hoc Networks (VANETs) using the NS-3 (Network Simulator 3) framework. The simulation compares the performance of AODV (Ad-hoc On-demand Distance Vector), DSDV (Destination-Sequenced Distance-Vector), and OLSR (Optimized Link State Routing) protocols under various network conditions.

## Project Overview

Vehicular Ad Hoc Networks (VANETs) are a form of mobile ad hoc networks (MANETs) designed to provide communications among nearby vehicles and between vehicles and roadside equipment. VANETs enable various applications such as safety warnings, traffic management, navigation, and infotainment services.

This project focuses on evaluating the performance of different routing protocols in VANET scenarios to determine which protocols perform best under various conditions.

## Routing Protocols

The simulation compares three widely used ad hoc routing protocols:

1. **AODV (Ad-hoc On-demand Distance Vector)**:
   - A reactive routing protocol that establishes routes only when needed
   - Uses sequence numbers to ensure loop-free routes
   - Well-suited for dynamic networks with frequent topology changes

2. **DSDV (Destination-Sequenced Distance-Vector)**:
   - A proactive table-driven routing protocol
   - Maintains routing tables with entries for all destinations
   - Uses sequence numbers to avoid routing loops
   - Modified in our implementation with more frequent updates (2s interval) and shorter settling time (6s)

3. **OLSR (Optimized Link State Routing)**:
   - A proactive link-state routing protocol optimized for mobile ad hoc networks
   - Uses Multipoint Relays (MPRs) to reduce overhead from flooding control messages
   - Configured with 1s hello interval and 3s TC interval for VANET scenarios

## Simulation Setup

### Network Parameters
- **Number of vehicles**: Configurable (default: 20)
- **Packet size**: 512 bytes
- **Simulation time**: 90 seconds
- **Road length**: 1000 meters
- **Node speed**: 20 m/s

### Network Configuration
- **WiFi Standard**: IEEE 802.11g
- **WiFi MAC**: Ad hoc mode
- **Data rate**: 6 Mbps (ErpOfdmRate6Mbps)
- **Propagation model**: Range propagation loss model with max range of 250m
- **IP addressing**: 10.1.1.0/24 subnet

### Mobility Models
Two mobility options are available:
1. **NS-3 Native Mobility**:
   - Grid-based initial positions
   - Constant velocity model with alternating directions
   - Vehicles move along a straight road with uniform speed

2. **SUMO Integration**:
   - Support for SUMO (Simulation of Urban MObility) generated traces
   - More realistic vehicle movements based on road networks
   - Various vehicle densities supported (20, 40, 60, 80 vehicles)

### Traffic Pattern
- UDP client-server communication
- Nodes are paired for communication
- Data transmission starts at 30s and continues till the end of simulation
- Each source sends 100 packets of 512 bytes at 0.5s intervals

## Performance Metrics

The simulation measures and compares the following performance metrics:

1. **Throughput**: The rate of successful message delivery over a communication channel (measured in Kbps)
2. **Packet Delivery Ratio (PDR)**: The ratio of packets successfully delivered to destination compared to the number of packets sent
3. **Average Delay**: The average time taken for packets to traverse from source to destination
4. **Packet Loss Ratio**: The percentage of packets lost during transmission

## Results

The simulation results are stored in:
- Text files in the `results/` directory (e.g., vanet-AODV-results.txt)
- Visualizations in both normal and logarithmic scales in the `results/` and `final_results/` directories
- Animation files for visualization using NetAnim in the `simulations/` directory

Key comparative analyses include:
- Individual protocol performance across different metrics
- Cross-protocol comparison of throughput, delay, and PDR
- Performance variation with different vehicle densities

## Project Structure

- **scripts/**
  - `vanet-routing-compare.cc`: Main simulation script
  
- **results/**
  - Performance metric text files and visualizations for each protocol
  
- **final_results/**
  - Comparative analysis graphs
  
- **simulations/**
  - Animation files for NetAnim
  
- **sumo/**
  - SUMO configuration and mobility trace files

## Running the Simulation

To run the simulation with default parameters:

```bash
./waf --run "scripts/vanet-routing-compare"
```

To run with custom parameters:

```bash
./waf --run "scripts/vanet-routing-compare --numVehicles=30 --protocol=1 --simTime=120"
```

Available command-line parameters:
- `numVehicles`: Number of vehicles (minimum 2)
- `simTime`: Simulation time in seconds
- `packetSize`: Size of UDP packets
- `protocol`: Routing protocol (1=AODV, 2=DSDV, 3=OLSR, 0=All)
- `animation`: Enable NetAnim output (0=off, 1=on)
- `useSumo`: Use SUMO mobility traces (0=off, 1=on)
- `traceFile`: SUMO mobility trace file path

## Design Choices and Optimizations

1. **Error Handling**:
   - Input parameter validation
   - Exception handling for simulation errors
   - Graceful degradation when animation can't be initialized

2. **Mobility Model**:
   - Alternating vehicle directions for more diverse traffic patterns
   - Support for both synthetic and SUMO-based realistic mobility

3. **Protocol Configurations**:
   - DSDV: Increased update frequency for better reactivity in dynamic VANET environments
   - OLSR: Optimized hello and TC intervals for vehicular networks
   - All protocols configured to handle high mobility scenarios

4. **WiFi Parameters**:
   - Range limited to 250m to better model realistic vehicle communication ranges
   - Constant rate manager for predictable performance

5. **Performance Monitoring**:
   - Per-flow statistics for detailed analysis
   - Aggregate statistics for overall performance comparison
   - Comprehensive logging to both console and files

## Future Work

- Integration with more realistic channel models
- Evaluation of geographic routing protocols (GPSR, GeoDTN)
- Impact of obstacle models on protocol performance
- Scalability testing with larger vehicle densities
- Implementation of VANET-specific metrics (connectivity duration, route stability)
