#ifndef CACHE_LINE_H
#define CACHE_LINE_H

#include <cstddef> 

// Represents a single line in the cache
struct CacheLine {
    bool valid = false;
    unsigned long long tag = 0;
    unsigned long long last_used_timestamp = 0;

    CacheLine() = default;
};

#endif 