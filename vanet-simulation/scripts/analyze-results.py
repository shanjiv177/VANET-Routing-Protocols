import os
import pandas as pd
import matplotlib.pyplot as plt

def analyze_results(metrics_dir):
    # Check if the metrics directory exists
    if not os.path.exists(metrics_dir):
        print(f"Metrics directory '{metrics_dir}' does not exist.")
        return

    # Initialize lists to store performance metrics
    throughput = []
    end_to_end_delay = []
    packet_loss_rate = []

    # Iterate through all files in the metrics directory
    for filename in os.listdir(metrics_dir):
        if filename.endswith('.csv'):
            file_path = os.path.join(metrics_dir, filename)
            # Read the CSV file into a DataFrame
            df = pd.read_csv(file_path)

            # Append metrics to the lists
            throughput.append(df['Throughput'].mean())
            end_to_end_delay.append(df['EndToEndDelay'].mean())
            packet_loss_rate.append(df['PacketLossRate'].mean())

    # Create a summary DataFrame
    summary_df = pd.DataFrame({
        'Simulation Run': range(1, len(throughput) + 1),
        'Throughput (Mbps)': throughput,
        'End-to-End Delay (ms)': end_to_end_delay,
        'Packet Loss Rate (%)': packet_loss_rate
    })

    # Save the summary to a CSV file
    summary_file = os.path.join(metrics_dir, 'summary_metrics.csv')
    summary_df.to_csv(summary_file, index=False)
    print(f"Summary metrics saved to '{summary_file}'.")

    # Plot the results
    plt.figure(figsize=(12, 6))

    # Throughput plot
    plt.subplot(1, 3, 1)
    plt.plot(summary_df['Simulation Run'], summary_df['Throughput (Mbps)'], marker='o')
    plt.title('Throughput Over Simulation Runs')
    plt.xlabel('Simulation Run')
    plt.ylabel('Throughput (Mbps)')

    # End-to-End Delay plot
    plt.subplot(1, 3, 2)
    plt.plot(summary_df['Simulation Run'], summary_df['End-to-End Delay (ms)'], marker='o', color='orange')
    plt.title('End-to-End Delay Over Simulation Runs')
    plt.xlabel('Simulation Run')
    plt.ylabel('End-to-End Delay (ms)')

    # Packet Loss Rate plot
    plt.subplot(1, 3, 3)
    plt.plot(summary_df['Simulation Run'], summary_df['Packet Loss Rate (%)'], marker='o', color='red')
    plt.title('Packet Loss Rate Over Simulation Runs')
    plt.xlabel('Simulation Run')
    plt.ylabel('Packet Loss Rate (%)')

    plt.tight_layout()
    plt.savefig(os.path.join(metrics_dir, 'performance_metrics.png'))
    print(f"Performance metrics plot saved to '{metrics_dir}/performance_metrics.png'.")

if __name__ == "__main__":
    metrics_directory = '../results/metrics'  # Adjust the path as necessary
    analyze_results(metrics_directory)