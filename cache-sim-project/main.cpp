#include "cache_simulator.h"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <limits>
#include <stdexcept>

// Function to trim leading/trailing whitespace
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r\f\v");
    if (std::string::npos == first) { return str; }
    size_t last = str.find_last_not_of(" \t\n\r\f\v");
    return str.substr(first, (last - first + 1));
}

size_t getSizeInput(const std::string& prompt) {
    long long value = -1;
    while (true) {
        std::cout << prompt;
        std::string line;
        if (!std::getline(std::cin, line)) { throw std::runtime_error("Input stream error."); }
        try {
            size_t processed_chars = 0;
            value = std::stoll(line, &processed_chars);
            if (processed_chars == line.length() && value > 0) {
                if (value > 0 && (value & (value - 1)) != 0) {
                     std::cout << "Warning: Input " << value << " is not a power of 2. This is allowed, but less common for hardware caches." << std::endl;
                }
                break;
            } else { std::cerr << "Error: Please enter a positive integer value." << std::endl; }
        } catch (const std::exception&) { std::cerr << "Error: Invalid number format or value out of range." << std::endl; }
    }
    return static_cast<size_t>(value);
}

// Get hit latency
unsigned int getUnsignedIntInput(const std::string& prompt) {
    long long value = -1;
     while (true) {
        std::cout << prompt;
        std::string line;
        if (!std::getline(std::cin, line)) { throw std::runtime_error("Input stream error."); }
         try {
            size_t processed_chars = 0;
            value = std::stoll(line, &processed_chars);
             if (processed_chars == line.length() && value > 0 && value <= std::numeric_limits<unsigned int>::max()) { break; }
             else { std::cerr << "Error: Please enter a positive integer value within range." << std::endl; }
        } catch (const std::exception&) { std::cerr << "Error: Invalid number format or value out of range." << std::endl; }
    }
    return static_cast<unsigned int>(value);
}

ReplacementPolicy getPolicyChoice() {
    int choice = 0;
    while (true) {
        std::cout << "Choose Replacement Policy (1 for LRU, 2 for FIFO): ";
        std::string line;
        if (!std::getline(std::cin, line)) { throw std::runtime_error("Input stream error."); }
         try {
            size_t processed_chars = 0;
            choice = std::stoi(line, &processed_chars);
             if (processed_chars == line.length() && (choice == 1 || choice == 2)) { break; }
             else { std::cerr << "Error: Invalid choice. Please enter 1 or 2." << std::endl; }
        } catch (const std::exception&) { std::cerr << "Error: Please enter 1 or 2." << std::endl; }
    }
    return (choice == 1) ? ReplacementPolicy::LRU : ReplacementPolicy::FIFO;
}

std::vector<unsigned long long> getAddressTraceFromInput() {
    std::vector<unsigned long long> addresses;
    std::string line;
    std::cout << "\nEnter memory access trace as comma-separated hexadecimal or decimal addresses (e.g., 0x1000, 4096, 0x1004):" << std::endl;
    if (!std::getline(std::cin, line)) {
         std::cerr << "Warning: Could not read address trace input." << std::endl;
         return addresses;
    }
    std::stringstream ss(line);
    std::string segment;
    while (std::getline(ss, segment, ',')) {
        std::string trimmed_segment = trim(segment);
        if (trimmed_segment.empty()) continue;
        try {
            size_t processed_chars = 0;
            unsigned long long addr;
            if (trimmed_segment.rfind("0x", 0) == 0 || trimmed_segment.rfind("0X", 0) == 0) {
                 addr = std::stoull(trimmed_segment, &processed_chars, 16);
            } else {
                 addr = std::stoull(trimmed_segment, &processed_chars, 10);
            }
            if (processed_chars == trimmed_segment.length()) { addresses.push_back(addr); }
            else { std::cerr << "Warning: Invalid characters in address segment: \"" << trimmed_segment << "\"" << std::endl; }
        } catch (const std::exception&) { std::cerr << "Warning: Invalid or out-of-range number in segment: \"" << trimmed_segment << "\"" << std::endl; }
    }
    std::cout << "Parsed " << addresses.size() << " valid addresses from input." << std::endl;
    return addresses;
}

int main() {
    try {
        CacheSimulatorConfig sim_config;
        ReplacementPolicy chosen_policy; 

        std::cout << "===== Multi-Level Cache Simulator Configuration =====\n" << std::endl;

        chosen_policy = getPolicyChoice(); 
        sim_config.memory_latency_cycles = getUnsignedIntInput("Enter Main Memory Access Latency (cycles): ");

        // --- L1 Configuration ---
        std::cout << "\n--- L1 Cache Configuration ---" << std::endl;
        sim_config.l1_config.size_bytes = getSizeInput("Enter L1 Size (bytes): ");
        sim_config.l1_config.block_size_bytes = getSizeInput("Enter L1 Block Size (bytes): ");
        size_t l1_max_assoc = (sim_config.l1_config.block_size_bytes > 0) ? (sim_config.l1_config.size_bytes / sim_config.l1_config.block_size_bytes) : 1;
        sim_config.l1_config.associativity = getSizeInput("Enter L1 Associativity (1=Direct, N=Set, " + std::to_string(l1_max_assoc > 0 ? l1_max_assoc : 1) + "=Full): ");
        sim_config.l1_config.policy = chosen_policy; // <-- FIX: Assign the stored choice
        sim_config.l1_config.hit_latency_cycles = getUnsignedIntInput("Enter L1 Hit Latency (cycles): ");


        // --- L2 Configuration ---
        std::cout << "\n--- L2 Cache Configuration ---" << std::endl;
        sim_config.l2_config.size_bytes = getSizeInput("Enter L2 Size (bytes): ");
        sim_config.l2_config.block_size_bytes = sim_config.l1_config.block_size_bytes;
        std::cout << "--> Using L1 block size (" << sim_config.l2_config.block_size_bytes << " bytes) for L2." << std::endl;
        size_t l2_max_assoc = (sim_config.l2_config.block_size_bytes > 0) ? (sim_config.l2_config.size_bytes / sim_config.l2_config.block_size_bytes) : 1;
        sim_config.l2_config.associativity = getSizeInput("Enter L2 Associativity (1=Direct, N=Set, " + std::to_string(l2_max_assoc > 0 ? l2_max_assoc : 1) + "=Full): ");
        sim_config.l2_config.policy = chosen_policy; // <-- FIX: Assign the stored choice
        sim_config.l2_config.hit_latency_cycles = getUnsignedIntInput("Enter L2 Hit Latency (cycles): ");


        // --- L3 Configuration ---
        std::cout << "\n--- L3 Cache Configuration ---" << std::endl;
        sim_config.l3_config.size_bytes = getSizeInput("Enter L3 Size (bytes): ");
        sim_config.l3_config.block_size_bytes = sim_config.l1_config.block_size_bytes;
        std::cout << "--> Using L1 block size (" << sim_config.l3_config.block_size_bytes << " bytes) for L3." << std::endl;
        size_t l3_max_assoc = (sim_config.l3_config.block_size_bytes > 0) ? (sim_config.l3_config.size_bytes / sim_config.l3_config.block_size_bytes) : 1;
        sim_config.l3_config.associativity = getSizeInput("Enter L3 Associativity (1=Direct, N=Set, " + std::to_string(l3_max_assoc > 0 ? l3_max_assoc : 1) + "=Full): ");
        sim_config.l3_config.policy = chosen_policy; // <-- FIX: Assign the stored choice
        sim_config.l3_config.hit_latency_cycles = getUnsignedIntInput("Enter L3 Hit Latency (cycles): ");


        CacheSimulator simulator(sim_config);
        std::vector<unsigned long long> addresses = getAddressTraceFromInput();

        if (!addresses.empty()) {
            simulator.runSimulation(addresses);
            simulator.printResults();
        } else {
            std::cout << "\nNo valid addresses entered. Simulation not run." << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "\nAn Error Occurred: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}