#include <algorithm>
#include <windows.h>
#include <psapi.h>
#include <iostream>
#include <thread>
#include <vector>
#include <numeric>

#include <pdh.h>

#include "primes.h"

#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "pdh.lib")

struct ProcessInfo {
    double cpuUsage;
    SIZE_T peakMemoryUsage;
    SIZE_T currentMemoryUsage;
};

static PDH_HQUERY cpuQuery;
static PDH_HCOUNTER cpuTotal;
std::vector<double> cpuUsages;
bool keepMonitoring = true;
DWORD pid;  // Process ID

void initCPUCounter() {
    PdhOpenQuery(nullptr, NULL, &cpuQuery);
    PdhAddEnglishCounter(cpuQuery, "\\Processor(_Total)\\% Processor Time", NULL, &cpuTotal);
    PdhCollectQueryData(cpuQuery);
}

double getCurrentCPUUsage() {
    PDH_FMT_COUNTERVALUE counterVal;
    PdhCollectQueryData(cpuQuery);
    PdhGetFormattedCounterValue(cpuTotal, PDH_FMT_DOUBLE, nullptr, &counterVal);
    return counterVal.doubleValue;
}

ProcessInfo getCPUAndMemoryUsage(HANDLE process) {
    FILETIME createTime, exitTime, kernelTime, userTime;
    SYSTEMTIME userSystemTime;
    ProcessInfo info{0, 0, 0};

    GetProcessTimes(process, &createTime, &exitTime, &kernelTime, &userTime);
    FileTimeToSystemTime(&userTime, &userSystemTime);

    ULARGE_INTEGER li;
    li.LowPart = userTime.dwLowDateTime;
    li.HighPart = userTime.dwHighDateTime;
    info.cpuUsage = li.QuadPart / 10000.0;

    PROCESS_MEMORY_COUNTERS pmc;
    GetProcessMemoryInfo(process, &pmc, sizeof(pmc));
    info.currentMemoryUsage = pmc.WorkingSetSize;
    info.peakMemoryUsage = pmc.PeakWorkingSetSize;

    return info;
}

void monitorSystem() {
    HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (process == nullptr) {
        std::cerr << "Failed to open process for PID: " << pid << std::endl;
        return;
    }

    initCPUCounter();
    while (keepMonitoring) {
        Sleep(1000);
        double cpuUsage = getCurrentCPUUsage();
        cpuUsages.push_back(cpuUsage);
        auto info = getCPUAndMemoryUsage(process);
        std::cout << "CPU Usage: " << cpuUsage << "%, Current Memory: "
                  << info.currentMemoryUsage / 1024 << " KB, Peak Memory: "
                  << info.peakMemoryUsage / 1024 << " KB" << std::endl;
    }
    CloseHandle(process);
}

int main() {
    pid = GetCurrentProcessId();  // Get the current process ID
    std::cout << "Monitoring process with PID: " << pid << std::endl;
    auto start = std::chrono::high_resolution_clock::now();

    std::thread monitorThread(monitorSystem);

    findPrimes(1, 100);

    keepMonitoring = false; // Stop monitoring
    monitorThread.join(); // Ensure the monitoring thread completes

    auto end = std::chrono::high_resolution_clock::now();

    double averageCpuUsage = std::accumulate(cpuUsages.begin(), cpuUsages.end(), 0.0) / cpuUsages.size();
    double peakCpuUsage = *std::max_element(cpuUsages.begin(), cpuUsages.end());

    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "Elapsed time: " << elapsed.count() << " ms" << std::endl;

    std::cout << "Average CPU Usage: " << averageCpuUsage << "%" << std::endl;
    std::cout << "Peak CPU Usage: " << peakCpuUsage << "%" << std::endl;

    return 0;
}
