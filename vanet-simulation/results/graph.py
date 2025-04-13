import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

aodv_data = pd.read_csv('aodv.txt')
dsdv_data = pd.read_csv('dsdv.txt')

aodv_data['Protocol'] = 'AODV'
dsdv_data['Protocol'] = 'DSDV'

# Combine both DataFrames
df = pd.concat([aodv_data, dsdv_data])

# Reorganize data to ensure we have proper trial data for both protocols
aodv_data_sorted = df[df['Protocol'] == 'AODV'].sort_values('Trial')
dsdv_data_sorted = df[df['Protocol'] == 'DSDV'].sort_values('Trial')

# Metrics to plot
metrics = [
    ('Total_Throughput_Kbps', 'Total Throughput (Kbps)'),
    ('Average_Delay_ms', 'Average Delay (ms)'),
    ('Packet_Delivery_Ratio', 'Packet Delivery Ratio (%)')
]

# Generate individual plots for each metric
for metric, title in metrics:
    plt.figure(figsize=(10, 6))
    
    # Plot AODV data
    plt.plot(aodv_data_sorted['Trial'], aodv_data_sorted[metric], 'b-o', label='AODV', linewidth=2, markersize=8)
    
    # Plot DSDV data
    plt.plot(dsdv_data_sorted['Trial'], dsdv_data_sorted[metric], 'r-s', label='DSDV', linewidth=2, markersize=8)
    
    # Set labels and title
    plt.xlabel('Trial', fontsize=12)
    plt.ylabel(title, fontsize=12)
    plt.title(f'Comparison of {title} between AODV and DSDV', fontsize=14)
    
    # Set x-ticks to only show trial numbers
    plt.xticks(np.unique(df['Trial']), fontsize=10)
    plt.yticks(fontsize=10)
    
    # Add grid for better readability
    plt.grid(True, linestyle='--', alpha=0.7)
    
    # Use logarithmic scale for throughput
    if metric == 'Total_Throughput_Kbps':
        plt.yscale('log')
        plt.ylabel(f"{title} (log scale)", fontsize=12)
        
        # Annotate values on the plot
        for _, row in aodv_data_sorted.iterrows():
            plt.annotate(f"{row[metric]:.2f}", (row['Trial'], row[metric]), 
                        textcoords="offset points", xytext=(0,10), ha='center', fontsize=9)
        for _, row in dsdv_data_sorted.iterrows():
            plt.annotate(f"{row[metric]:.2f}", (row['Trial'], row[metric]), 
                        textcoords="offset points", xytext=(0,-15), ha='center', fontsize=9)
    
    # Add legend
    plt.legend(fontsize=11)
    
    # Add minor gridlines for log scale
    if metric == 'Total_Throughput_Kbps':
        plt.grid(True, which='minor', linestyle=':', alpha=0.4)
    
    plt.tight_layout()
    plt.savefig(f'{metric}_comparison.png', dpi=300, bbox_inches='tight')
    plt.show()

# Create a version with all three plots together but arranged vertically
fig, axes = plt.subplots(3, 1, figsize=(10, 15))
plt.subplots_adjust(hspace=0.3)

for i, (metric, title) in enumerate(metrics):
    ax = axes[i]
    
    # Plot AODV data
    ax.plot(aodv_data_sorted['Trial'], aodv_data_sorted[metric], 'b-o', label='AODV', linewidth=2)
    
    # Plot DSDV data
    ax.plot(dsdv_data_sorted['Trial'], dsdv_data_sorted[metric], 'r-s', label='DSDV', linewidth=2)
    
    # Set labels and title
    ax.set_xlabel('Trial')
    ax.set_ylabel(title)
    ax.set_title(f'Comparison of {title} between AODV and DSDV')
    
    # Set x-ticks to only show trial numbers
    ax.set_xticks(np.unique(df['Trial']))
    
    # Add grid for better readability
    ax.grid(True, linestyle='--', alpha=0.7)
    
    # Add legend
    ax.legend()

    # Set logarithmic scale for throughput
    if metric == 'Total_Throughput_Kbps':
        ax.set_yscale('log')
        ax.set_ylabel(f"{title} (log scale)")
        
        # Add minor gridlines for log scale
        ax.grid(True, which='minor', linestyle=':', alpha=0.4)
        
        # Annotate values
        for _, row in aodv_data_sorted.iterrows():
            ax.annotate(f"{row[metric]:.2f}", (row['Trial'], row[metric]), 
                        textcoords="offset points", xytext=(0,10), ha='center')
        for _, row in dsdv_data_sorted.iterrows():
            ax.annotate(f"{row[metric]:.2f}", (row['Trial'], row[metric]), 
                        textcoords="offset points", xytext=(0,-15), ha='center')

plt.savefig('all_metrics_comparison.png', dpi=300, bbox_inches='tight')
plt.close()  # Close without showing to avoid displaying the combined plot
