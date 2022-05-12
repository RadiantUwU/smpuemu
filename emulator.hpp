#pragma once

#include <memory>

#include "configdef.hpp"

class Memory {
    char* mem;
public:
    Memory() {
        mem = (char*)malloc(Config::memlen * 1024);
    }
    char& operator[] (unsigned long a) {
        return mem[a];
    }
}