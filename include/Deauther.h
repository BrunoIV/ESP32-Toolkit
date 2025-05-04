#ifndef DEAUTHER_H
#define DEAUTHER_H

#include "WifiNetwork.h"

class Deauther {
public:
    static void start(WifiNetwork network);
    static void stop();
    
private:
    static IRAM_ATTR void sniffer(void* buf, wifi_promiscuous_pkt_type_t type);
};

#endif
