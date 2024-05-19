import subprocess
import numpy as np
import pandas as pd
import time

def run_test(command, param):
    times = []
    mem_usages = []

    for _ in range(10):
        time_start = time.time()
        result = subprocess.run(f'/usr/bin/time -f "%e %M" {command} {param}', capture_output=True, text=True, shell=True)
        time_taken = time.time() - time_start
        time_taken = time_taken * 1000 # convert to ms
        output = result.stderr.strip().split()
        mem_usage = int(output[1])

        times.append(time_taken)
        mem_usages.append(mem_usage)

    avg_time = np.mean(times)
    jitter = np.std(times)
    avg_mem = np.mean(mem_usages)

    return avg_time, jitter, avg_mem

def main():
    process_counts = [10, 50, 100, 200, 500, 1000]
    thread_counts = [10, 50, 100, 200, 500,1000]

    process_results = []
    thread_results = []

    for count in process_counts:
        process_results.append(run_test('./process_switch', count))

    for count in thread_counts:
        thread_results.append(run_test('./thread_switch', count))

    data = {
        "Process Count": process_counts,
        "Process Avg Time (ms)": [result[0] for result in process_results],
        "Process Jitter (ms)": [result[1] for result in process_results],
        "Process Avg Memory (KB)": [result[2] for result in process_results],
        "Thread Count": thread_counts,
        "Thread Avg Time (ms)": [result[0] for result in thread_results],
        "Thread Jitter (ms)": [result[1] for result in thread_results],
        "Thread Avg Memory (KB)": [result[2] for result in thread_results],
    }

    df = pd.DataFrame(data)
    print(df)
    df.to_csv("performance_results.csv", index=False)

if __name__ == "__main__":
    main()
