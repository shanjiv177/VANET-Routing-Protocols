import xml.etree.ElementTree as ET
import sys

def convert_sumo_to_ns3(sumo_trace_file, ns3_output_file):
    # Parse the SUMO trace file
    tree = ET.parse(sumo_trace_file)
    root = tree.getroot()

    with open(ns3_output_file, 'w') as ns3_file:
        ns3_file.write("NodeId,Time,PosX,PosY\n")  # Header for NS3 format

        # Iterate through each vehicle in the SUMO trace
        for vehicle in root.findall('vehicle'):
            node_id = vehicle.get('id')
            for position in vehicle.findall('position'):
                time = position.get('time')
                pos_x = position.get('x')
                pos_y = position.get('y')
                ns3_file.write(f"{node_id},{time},{pos_x},{pos_y}\n")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python sumo-to-ns3.py <sumo_trace_file> <ns3_output_file>")
        sys.exit(1)

    sumo_trace_file = sys.argv[1]
    ns3_output_file = sys.argv[2]

    convert_sumo_to_ns3(sumo_trace_file, ns3_output_file)