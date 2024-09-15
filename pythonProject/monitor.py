import psutil
import platform
from datetime import datetime

def precise_seconds_from_timedelta(td):
    # Helper function to return total seconds from timedelta as a float
    return td.total_seconds()

def monitor_process(process_name):
    process_data = []
    process_found = False
    start_time = None

    try:
        print(f"Waiting for process '{process_name}' to start...")

        while True:
            processes = [p for p in psutil.process_iter(['name', 'cpu_percent', 'memory_info', 'create_time'])
                         if p.info['name'] == process_name]

            if processes and not process_found:
                process_found = True
                start_time = datetime.now()  # Set start time when process is first found
                print(f"Process '{process_name}' found. Monitoring started...")

            if process_found:
                if not processes:  # If the process list is empty, the process has exited
                    print(f"Process '{process_name}' has exited.")
                    if start_time:
                        total_running_time = datetime.now() - start_time
                        total_seconds = precise_seconds_from_timedelta(total_running_time)
                        print(f"Total Running Time: {total_seconds:.2f} seconds")
                    break

                for proc in processes:
                    try:
                        cpu_usage = proc.cpu_percent(interval=1)
                        memory_usage = proc.memory_info().rss / (1024 * 1024)

                        # Collect data instead of calculating on the fly
                        process_data.append((cpu_usage, memory_usage))

                    except psutil.NoSuchProcess:
                        continue

        # Perform calculations after monitoring ends
        if process_data:
            max_cpu_usage = max(data[0] for data in process_data)
            max_memory_usage = max(data[1] for data in process_data)
            average_cpu_usage = sum(data[0] for data in process_data) / len(process_data)

            print(f"Max CPU Usage: {max_cpu_usage}%")
            print(f"Max Memory Usage: {max_memory_usage} MB")
            print(f"Average CPU Usage: {average_cpu_usage}%")

    except KeyboardInterrupt:
        print("Monitoring manually stopped.")

if __name__ == "__main__":
    system_os = platform.system()
    if system_os == "Windows":
        process_name = "project_test.exe"  # Adjust for Windows executable extension
    else:
        process_name = "project_test"  # No extension for Linux or MacOS

    monitor_process(process_name)