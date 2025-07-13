#ifndef CACHE_SIMULATOR_H
#define CACHE_SIMULATOR_H

#include "cache_level.h"
#include <vector>

class CacheSimulator {
public:
    CacheSimulator(const CacheSimulatorConfig& config);
    ~CacheSimulator() = default;

    void accessMemory(unsigned long long address);
    void runSimulation(const std::vector<unsigned long long>& addresses);
    void printResults() const;

private:
    CacheSimulatorConfig config_;
    CacheLevel l3_cache_;
    CacheLevel l2_cache_;
    CacheLevel l1_cache_;

    unsigned long long total_accesses_ = 0;
    unsigned long long total_cycles_ = 0;
    unsigned long long global_access_counter_ = 0;
};

#endif 