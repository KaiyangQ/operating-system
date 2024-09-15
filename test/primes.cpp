#include "primes.h"
#include <iostream>
#include <vector>
#include <thread>
#include <unistd.h> // for getpid()

bool isPrime(int number) {
    if (number <= 1) return false;
    if (number <= 3) return true;
    if (number % 2 == 0 || number % 3 == 0) return false;
    for (int i = 5; i * i <= number; i += 6) {
        if (number % i == 0 || number % (i + 2) == 0) return false;
    }
    return true;
}

void findPrimesInRange(int start, int end) {
    for (int num = start; num <= end; ++num) {
        if (isPrime(num)) {
            std::cout << num << " is a prime number from Thread: " << std::this_thread::get_id() <<" pid: "<<getpid()<< std::endl;
        }
    }
}

void findPrimes(int start, int end) {
    constexpr int threadCount = 4; // Number of threads
    std::vector<std::thread> threads;

    // Calculate range size for each thread
    const int range = (end - start + 1) / threadCount;

    // Launching threads
    for (int i = 0; i < threadCount; ++i) {
        int rangeStart = start + i * range;
        int rangeEnd = (i == threadCount - 1) ? end : rangeStart + range - 1;
        threads.emplace_back(findPrimesInRange, rangeStart, rangeEnd);
    }

    // Waiting for all threads to finish
    for (auto& thread : threads) {
        thread.join();
    }
}
