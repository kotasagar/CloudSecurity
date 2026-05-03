import subprocess
import re
import matplotlib.pyplot as plt
import os

sizes = [10, 50, 100, 500, 1000, 2000, 4000]
setup_times = []
cloud_times = []

# Regex patterns to extract timings
setup_pattern = re.compile(r"Setup Time:\s+(\d+)\s+ms")
cloud_pattern = re.compile(r"Cloud Computation Time:\s+(\d+)\s+ms")

executable = "./build/HybridCloudSecurity"

print("Starting benchmark for FHE Hybrid Architecture...")

for size in sizes:
    print(f"Running benchmark for dataset size N = {size}...")
    try:
        # Run the WSL executable
        result = subprocess.run(
            ["wsl", "-e", "bash", "-c", f"cd '/mnt/c/Users/KOTA SAGAR/Music/Cloud/HybridCloudSecurity' && {executable} {size}"],
            capture_output=True, text=True, check=True
        )
        
        output = result.stdout
        
        setup_match = setup_pattern.search(output)
        cloud_match = cloud_pattern.search(output)
        
        if setup_match and cloud_match:
            setup_time = int(setup_match.group(1))
            cloud_time = int(cloud_match.group(1))
            
            setup_times.append(setup_time)
            cloud_times.append(cloud_time)
            print(f"  -> Setup: {setup_time} ms, Cloud Compute: {cloud_time} ms")
        else:
            print("  -> Error: Could not parse timings.")
            setup_times.append(0)
            cloud_times.append(0)
            
    except subprocess.CalledProcessError as e:
        print(f"Error running executable: {e}")
        setup_times.append(0)
        cloud_times.append(0)

# Generate Graph
plt.figure(figsize=(10, 6))
plt.plot(sizes, cloud_times, marker='o', linestyle='-', color='b', linewidth=2, label='FHE Cloud Latency')

plt.title('Cloud Execution Latency vs Dataset Size', fontsize=14, fontweight='bold')
plt.xlabel('Dataset Size (N slots)', fontsize=12)
plt.ylabel('Latency (ms)', fontsize=12)
plt.grid(True, linestyle='--', alpha=0.7)
plt.legend()
plt.tight_layout()

# Save the graph
graph_path = os.path.join(os.path.dirname(__file__), "latency_vs_size.png")
plt.savefig(graph_path, dpi=300)
print(f"\nGraph successfully saved to: {graph_path}")
