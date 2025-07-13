#ifndef CACHE_LEVEL_H
#define CACHE_LEVEL_H

#include "cache_config.h"
#include "cache_line.h"
#include <vector>
#include <cstddef>

class CacheLevel {
public:
    CacheLevel(const CacheLevelConfig& config, CacheLevel* next = nullptr, unsigned int level_num = 0, unsigned int memory_latency = 0);
    ~CacheLevel() = default;

    bool access(unsigned long long address, unsigned long long& access_counter, unsigned int& latency_cycles);

    size_t getHits() const;
    size_t getMisses() const;
    size_t getAccesses() const;
    double getHitRate() const;
    double getMissRate() const;
    const CacheLevelConfig& getConfig() const;
    unsigned int getLevelNum() const;

    void printStats() const;

private:
    CacheLevelConfig config_;
    std::vector<std::vector<CacheLine>> sets_;
    CacheLevel* next_level_;
    unsigned int level_num_;
    unsigned int memory_latency_cycles_;

    size_t num_blocks_;
    size_t num_sets_;
    size_t index_bits_;
    size_t offset_bits_;
    size_t tag_bits_;

    size_t hits_ = 0;
    size_t misses_ = 0;

    void calculateDerivedParameters();
    unsigned long long extractTag(unsigned long long address) const;
    size_t extractIndex(unsigned long long address) const;
    size_t findVictim(size_t set_index, unsigned long long access_counter);
    void updateReplacementPolicy(size_t set_index, size_t line_index, unsigned long long access_counter);
    bool isPowerOfTwo(size_t n);
};

#endif 