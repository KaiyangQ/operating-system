# Operating System Concepts

Exploration of core OS concepts — multithreading, process monitoring, and resource tracking — implemented in C++ and Python.

## Overview

This project covers practical OS topics through two parallel implementations: a Windows C++ suite that uses native system APIs for real-time process instrumentation, and a cross-platform Python monitor built on `psutil`.

## Contents

### C++ (`test/`)

| File | Description |
|---|---|
| `main.cpp` | Windows process monitor. A background thread continuously samples CPU usage (via PDH) and memory consumption (via PSAPI) while the main thread runs a CPU-bound prime computation, demonstrating concurrent resource tracking. |
| `primes.cpp` / `primes.h` | Multithreaded prime finder. Spawns 4 worker threads; each prints discovered primes along with its thread ID and process ID, illustrating thread creation and identification with C++17 `<thread>`. |

**Key APIs used:** `PDH` (Performance Data Helper) for CPU counters, `PSAPI` for working-set memory queries, `std::thread` / `std::mutex` for concurrency.

### Python (`pythonProject/`)

| File | Description |
|---|---|
| `monitor.py` | Cross-platform process monitor using `psutil`. Polls CPU percentage and RSS memory at regular intervals and reports maximum and average statistics on exit. |

## Concepts Covered

- **Multithreading** — thread creation, synchronization, and identification (thread ID / PID)
- **Process monitoring** — sampling CPU utilization and memory footprint at runtime
- **Resource tracking** — aggregating max/average stats over a monitoring window
- **Windows system APIs** — PDH performance counters, PSAPI memory info

## Tech Stack

| Layer | Technology |
|---|---|
| Language (systems) | C++17 |
| Threading | `std::thread`, `std::mutex` |
| Windows APIs | PDH, PSAPI |
| Language (scripting) | Python 3 |
| Process monitoring | `psutil` |

## Requirements

**C++**
- Windows (PDH and PSAPI are Windows-only)
- C++17-capable compiler (MSVC, MinGW-w64)

**Python**
```
pip install psutil
```
