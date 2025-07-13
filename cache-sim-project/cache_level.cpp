#include "cache_level.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <stdexcept>
#include <limits>

CacheLevel::CacheLevel(const CacheLevelConfig& config, CacheLevel* next, unsigned int level_num, unsigned int memory_latency)
    : config_(config), next_level_(next), level_num_(level_num), memory_latency_cycles_(memory_latency),
      hits_(0), misses_(0)
{
    if (config_.size_bytes == 0 || config_.block_size_bytes == 0 || config_.associativity == 0) {
        throw std::invalid_argument("Cache size, block size, and associativity must be non-zero.");
    }
    if (config_.size_bytes % config_.block_size_bytes != 0) {
         throw std::invalid_argument("Cache size must be divisible by block size.");
    }
    if (!isPowerOfTwo(config_.block_size_bytes)) {
        std::cerr << "Warning: Block size (" << config_.block_size_bytes << ") is not a power of 2." << std::endl;
    }

    calculateDerivedParameters();

    if (num_blocks_ % config_.associativity != 0) {
        if (config_.associativity > num_blocks_) {
            std::cerr << "Warning: Associativity (" << config_.associativity << ") is greater than the number of blocks (" << num_blocks_ << "). Setting to fully associative (" << num_blocks_ << ")." << std::endl;
            config_.associativity = num_blocks_;
            calculateDerivedParameters();
        } else {
             throw std::invalid_argument("Number of blocks must be divisible by associativity (unless fully associative).");
        }
    }
    if (num_sets_ > 1 && !isPowerOfTwo(num_sets_)) {
         std::cerr << "Warning: Number of sets (" << num_sets_ << ") is not a power of 2. Indexing might be unusual." << std::endl;
    }
    sets_.resize(num_sets_);
    for (size_t i = 0; i < num_sets_; ++i) {
        sets_[i].resize(config_.associativity);
    }
    std::cout << "Initialized L" << level_num_ << " Cache: " << "Size=" << config_.size_bytes << "B, " << "BlockSize=" << config_.block_size_bytes << "B, " << "Assoc=" << config_.associativity << ", " << "Sets=" << num_sets_ << ", " << "Policy=" << (config_.policy == ReplacementPolicy::LRU ? "LRU" : "FIFO") << ", " << "Hit Latency=" << config_.hit_latency_cycles << " cycles" << std::endl;
    std::cout << "  L" << level_num_ << " Calculated: " << "Offset Bits=" << offset_bits_ << ", " << "Index Bits=" << index_bits_ << ", " << "Tag Bits=" << tag_bits_ << std::endl;
}

void CacheLevel::calculateDerivedParameters() {
    num_blocks_ = config_.size_bytes / config_.block_size_bytes;
    if (config_.associativity == 0) { throw std::runtime_error("Associativity cannot be zero during parameter calculation."); }
    if (config_.associativity >= num_blocks_) {
        config_.associativity = num_blocks_;
        num_sets_ = 1;
    } else {
        num_sets_ = num_blocks_ / config_.associativity;
    }
    offset_bits_ = (config_.block_size_bytes <= 1) ? 0 : static_cast<size_t>(std::log2(config_.block_size_bytes));
    index_bits_ = (num_sets_ <= 1) ? 0 : static_cast<size_t>(std::log2(num_sets_));
    size_t total_address_bits = 64;
    if (offset_bits_ + index_bits_ > total_address_bits) { throw std::runtime_error("Offset and Index bits exceed total address bits (64). Check configuration."); }
    tag_bits_ = total_address_bits - index_bits_ - offset_bits_;
}

unsigned long long CacheLevel::extractTag(unsigned long long address) const { return address >> (offset_bits_ + index_bits_); }

size_t CacheLevel::extractIndex(unsigned long long address) const {
    if (index_bits_ == 0) { return 0; }
    unsigned long long temp = address >> offset_bits_;
    unsigned long long index_mask = (1ULL << index_bits_) - 1;
    return static_cast<size_t>(temp & index_mask);
}

bool CacheLevel::access(unsigned long long address, unsigned long long& access_counter, unsigned int& latency_cycles) {
    unsigned long long tag = extractTag(address);
    size_t index = extractIndex(address);
    access_counter++;
    auto& current_set = sets_[index];
    for (size_t i = 0; i < config_.associativity; ++i) {
        if (current_set[i].valid && current_set[i].tag == tag) {
            hits_++;
            if (config_.policy == ReplacementPolicy::LRU) {
                updateReplacementPolicy(index, i, access_counter);
            }
            latency_cycles = config_.hit_latency_cycles;
            return true;
        }
    }
    misses_++;
    unsigned int downstream_latency = 0;
    if (next_level_) {
        next_level_->access(address, access_counter, downstream_latency);
        latency_cycles = config_.hit_latency_cycles + downstream_latency;
    } else {
        downstream_latency = memory_latency_cycles_;
        latency_cycles = config_.hit_latency_cycles + downstream_latency;
    }
    size_t victim_index = findVictim(index, access_counter);
    current_set[victim_index].valid = true;
    current_set[victim_index].tag = tag;
    updateReplacementPolicy(index, victim_index, access_counter);
    return false;
}

size_t CacheLevel::findVictim(size_t set_index, unsigned long long access_counter) {
    auto& current_set = sets_[set_index];
    for (size_t i = 0; i < config_.associativity; ++i) {
        if (!current_set[i].valid) { return i; }
    }
    size_t victim_index = 0;
    unsigned long long min_timestamp = std::numeric_limits<unsigned long long>::max();
    for (size_t i = 0; i < config_.associativity; ++i) {
        if (current_set[i].last_used_timestamp < min_timestamp) {
            min_timestamp = current_set[i].last_used_timestamp;
            victim_index = i;
        }
    }
    return victim_index;
}

void CacheLevel::updateReplacementPolicy(size_t set_index, size_t line_index, unsigned long long access_counter) {
    sets_[set_index][line_index].last_used_timestamp = access_counter;
}

size_t CacheLevel::getHits() const { return hits_; }
size_t CacheLevel::getMisses() const { return misses_; }
size_t CacheLevel::getAccesses() const { return hits_ + misses_; }
double CacheLevel::getHitRate() const { return (getAccesses() == 0) ? 0.0 : static_cast<double>(hits_) / getAccesses(); }
double CacheLevel::getMissRate() const { return (getAccesses() == 0) ? 0.0 : static_cast<double>(misses_) / getAccesses(); }
const CacheLevelConfig& CacheLevel::getConfig() const { return config_; }
unsigned int CacheLevel::getLevelNum() const { return level_num_; }
bool CacheLevel::isPowerOfTwo(size_t n) { return (n > 0) && ((n & (n - 1)) == 0); }

void CacheLevel::printStats() const {
    std::cout << "--- L" << level_num_ << " Cache Stats ---" << std::endl;
    std::cout << " Accesses: " << getAccesses() << std::endl;
    std::cout << " Hits:     " << hits_ << std::endl;
    std::cout << " Misses:   " << misses_ << std::endl;
    std::cout << std::fixed << std::setprecision(4);
    std::cout << " Hit Rate: " << getHitRate() * 100.0 << "%" << std::endl;
    std::cout << " Miss Rate:" << getMissRate() * 100.0 << "%" << std::endl;
    std::cout << std::defaultfloat << std::setprecision(6);
}