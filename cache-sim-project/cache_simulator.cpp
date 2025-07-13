#include "cache_simulator.h"
#include <iostream>
#include <iomanip>

CacheSimulator::CacheSimulator(const CacheSimulatorConfig& config) :
    config_(config),
    l3_cache_(config_.l3_config, nullptr, 3, config_.memory_latency_cycles),
    l2_cache_(config_.l2_config, &l3_cache_, 2),
    l1_cache_(config_.l1_config, &l2_cache_, 1)
{
    std::cout << "\nCache Simulator Initialized." << std::endl;
    std::cout << "Main Memory Access Latency: " << config_.memory_latency_cycles << " cycles" << std::endl;
}

void CacheSimulator::accessMemory(unsigned long long address) {
    total_accesses_++;
    unsigned int access_latency = 0;
    l1_cache_.access(address, global_access_counter_, access_latency);
    total_cycles_ += access_latency;
}

void CacheSimulator::runSimulation(const std::vector<unsigned long long>& addresses) {
    std::cout << "\nStarting Simulation for " << addresses.size() << " addresses..." << std::endl;
    if (addresses.empty()) {
        std::cout << "Warning: No addresses provided for simulation." << std::endl;
        return;
    }
    for (unsigned long long addr : addresses) {
        accessMemory(addr);
    }
    std::cout << "Simulation Finished." << std::endl;
}

void CacheSimulator::printResults() const {
    std::cout << "\n=============== Simulation Results ===============" << std::endl;
    std::cout << "Total Memory Accesses Requested: " << total_accesses_ << std::endl;
    std::cout << "Total Simulation Cycles:         " << total_cycles_ << std::endl;

    double avg_access_time = 0.0;
    if (total_accesses_ > 0) {
        avg_access_time = static_cast<double>(total_cycles_) / total_accesses_;
        std::cout << std::fixed << std::setprecision(4);
        std::cout << "Average Memory Access Time (AMAT): " << avg_access_time << " cycles" << std::endl;
         std::cout << std::defaultfloat << std::setprecision(6);
    } else {
         std::cout << "Average Memory Access Time (AMAT): N/A (No accesses)" << std::endl;
    }

    std::cout << std::endl;
    l1_cache_.printStats();
    std::cout << std::endl;
    l2_cache_.printStats();
    std::cout << std::endl;
    l3_cache_.printStats();
    std::cout << "==================================================" << std::endl;

    if (total_accesses_ > 0) {
        double l1_miss_rate = l1_cache_.getMissRate();
        double l2_miss_rate = l2_cache_.getMissRate();
        double l3_miss_rate = l3_cache_.getMissRate();
        double l3_miss_penalty = static_cast<double>(config_.memory_latency_cycles);
        double amat_l3 = l3_cache_.getConfig().hit_latency_cycles + (l3_miss_rate * l3_miss_penalty);
        double l2_miss_penalty = amat_l3;
        double amat_l2 = l2_cache_.getConfig().hit_latency_cycles + (l2_miss_rate * l2_miss_penalty);
        double l1_miss_penalty = amat_l2;
        double amat_l1_formula = l1_cache_.getConfig().hit_latency_cycles + (l1_miss_rate * l1_miss_penalty);

        std::cout << "\nAMAT Calculation (Formula-based):" << std::endl;
        std::cout << std::fixed << std::setprecision(4);
        std::cout << "  AMAT L3 = L3_HitT + (L3_MR * MemLat) = " << l3_cache_.getConfig().hit_latency_cycles << " + (" << l3_miss_rate << " * " << l3_miss_penalty << ") = " << amat_l3 << std::endl;
        std::cout << "  AMAT L2 = L2_HitT + (L2_MR * AMAT_L3) = " << l2_cache_.getConfig().hit_latency_cycles << " + (" << l2_miss_rate << " * " << amat_l3 << ") = " << amat_l2 << std::endl;
        std::cout << "  AMAT L1 = L1_HitT + (L1_MR * AMAT_L2) = " << l1_cache_.getConfig().hit_latency_cycles << " + (" << l1_miss_rate << " * " << amat_l2 << ") = " << amat_l1_formula << std::endl;
        std::cout << "(Formula AMAT should closely match the simulated AMAT: " << avg_access_time << ")" << std::endl;
        std::cout << std::defaultfloat << std::setprecision(6);
        std::cout << "==================================================" << std::endl;
    }
}