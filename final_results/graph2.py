import pandas as pd
import matplotlib.pyplot as plt
import os

# CSV files to process
csv_files = ["aodv.csv", "olsr.csv", "dsdv.csv"]

# Metrics to plot (label, column name in CSV)
metrics = [
    ("Packet Delivery Ratio (%)", "Packet Delivery Ratio"),
    ("Average Delay (ms)", "Average Delay"),
    ("Throughput (Kbps)", "Throughput"),
]

# Create output folder
os.makedirs("plots", exist_ok=True)

# Dictionary to store dataframes keyed by protocol name
data = {}

# Load and clean data
for filename in csv_files:
    protocol = filename.split(".")[0]
    df = pd.read_csv(filename)
    
    # Clean "Packet Delivery Ratio" column if needed
    if df["Packet Delivery Ratio"].dtype == object:
        df["Packet Delivery Ratio"] = df["Packet Delivery Ratio"].str.rstrip('%').astype(float)
    
    data[protocol] = df

# Plot each metric comparing the 3 protocols
for label, col in metrics:
    plt.figure(figsize=(10, 6))
    
    for protocol, df in data.items():
        plt.plot(df["Flow"], df[col], marker='o', label=protocol.upper())

    plt.yscale("log")
    plt.xlabel("Number of Vehicles")
    plt.ylabel(label)
    plt.title(f"{label} vs Number of Vehicles (Log Scale)")
    plt.grid(True, which="both", linestyle='--', linewidth=0.5)
    plt.legend()
    plt.tight_layout()
    plt.savefig(f"plots/comparison_{col.replace(' ', '_')}_log_scale.png")
    plt.close()

print("✅ Comparison plots saved in 'plots/' folder.")
