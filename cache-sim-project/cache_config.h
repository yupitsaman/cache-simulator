#ifndef CACHE_CONFIG_H
#define CACHE_CONFIG_H

#include <cstddef> 

enum class ReplacementPolicy {
    LRU,
    FIFO
};

// Structure to hold configuration for a single cache level
struct CacheLevelConfig {
    size_t size_bytes = 0;
    size_t block_size_bytes = 0;
    size_t associativity = 0;
    ReplacementPolicy policy = ReplacementPolicy::LRU;
    unsigned int hit_latency_cycles = 0;
};

// Structure to hold configuration for the entire simulator
struct CacheSimulatorConfig {
    CacheLevelConfig l1_config;
    CacheLevelConfig l2_config;
    CacheLevelConfig l3_config;
    unsigned int memory_latency_cycles = 0;
};

#endif 