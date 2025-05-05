import matplotlib.pyplot as plt

def plot_throughput(filename, title):
    times = []
    flow_ids = []
    throughputs = []
    
    with open(filename, 'r') as f:
        for line in f:
            t, fid, tp = map(float, line.split())
            times.append(t)
            flow_ids.append(int(fid))
            throughputs.append(tp)
    
    plt.figure(figsize=(10, 6))
    unique_fids = sorted(set(flow_ids))
    for fid in unique_fids:
        t = [times[i] for i in range(len(times)) if flow_ids[i] == fid]
        tp = [throughputs[i] for i in range(len(throughputs)) if flow_ids[i] == fid]
        plt.plot(t, tp, label=f'Flow {fid}')
    
    plt.title(title)
    plt.xlabel('Time (s)')
    plt.ylabel('Throughput (Mbps)')
    plt.legend()
    plt.grid(True)
    plt.savefig(f'{filename[:-4]}.png')

if __name__ == '__main__':
    plot_throughput('spq-throughput.txt', 'SPQ Throughput vs. Time')
    plot_throughput('drr-throughput.txt', 'DRR Throughput vs. Time')