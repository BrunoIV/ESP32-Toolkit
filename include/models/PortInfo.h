#pragma once

struct PortInfo {
    int port;
    const char* name;
};

extern const PortInfo ports[];
extern const int NUM_PORTS;