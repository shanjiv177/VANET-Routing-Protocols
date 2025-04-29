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

for filename in csv_files:
    protocol = filename.split(".")[0]
    df = pd.read_csv(filename)

    # Clean the "Packet Delivery Ratio" column (remove '%' and convert to float)
    df["Packet Delivery Ratio"] = df["Packet Delivery Ratio"].astype(float)

    # Plot each metric individually (normal y-axis)
    for label, col in metrics:
        plt.figure(figsize=(10, 6))
        plt.plot(df["Flow"], df[col], marker='o')
        plt.title(f"{label} per Flow ({protocol.upper()})")
        plt.xlabel("Flow")
        plt.ylabel(label)
        plt.grid(True)
        plt.tight_layout()
        plt.savefig(f"plots/{protocol}_{col.replace(' ', '_')}_normal.png")
        plt.close()

    # Combined plot with log scale
    plt.figure(figsize=(12, 8))
    for label, col in metrics:
        plt.plot(df["Flow"], df[col], marker='o', label=label)

    plt.yscale("log")
    plt.xlabel("Flow")
    plt.ylabel("Log Scale")
    plt.title(f"All Metrics per Flow (Log Scale) - {protocol.upper()}")
    plt.grid(True, which="both", linestyle='--', linewidth=0.5)
    plt.legend()
    plt.tight_layout()
    plt.savefig(f"plots/{protocol}_all_metrics_log_scale.png")
    plt.close()

print("✅ All plots saved in 'plots/' folder.")
