#include "Scanner.h"
#include "models/PortInfo.h"
#include <WiFi.h>
#include <ESP32Ping.h>
#include <Utils.h>

static String ipToScan = "";
static String _ipStart = "";
static String _ipEnd = "";

void Scanner::scanCommonPorts(ScanCallback cb) {
    if(ipToScan != "") {
        for (int i = 0; i < NUM_PORTS; i++) {
            WiFiClient client;
            if (client.connect(ipToScan.c_str(), ports[i].port)) {
                cb("🟢 " + String(ports[i].port) + " (" + ports[i].name + ")");
                client.stop();
            } else {
                cb("🔴 " + String(ports[i].port) + " (" + ports[i].name + ")");
            }
            delay(50); //prevent flood
        }

        ipToScan = "";
    }

    if(_ipStart != "") {
        int a1, b1, c1, d1;
        int a2, b2, c2, d2;

        if (sscanf(_ipStart.c_str(), "%d.%d.%d.%d", &a1, &b1, &c1, &d1) != 4) return;
        if (sscanf(_ipEnd.c_str(), "%d.%d.%d.%d", &a2, &b2, &c2, &d2) != 4) return;

        uint32_t start = Utils::ipToInt(a1,b1,c1,d1);
        uint32_t end   = Utils::ipToInt(a2,b2,c2,d2);

        for (uint32_t ip = start; ip <= end; ip++) {

            int a,b,c,d;
            Utils::intToIp(ip, a,b,c,d);

            char ipStr[16];
            sprintf(ipStr, "%d.%d.%d.%d", a,b,c,d);

            char buf[16];
            sprintf(buf, "%d.%d.%d.%d", a, b, c, d);

            String result = Ping.ping(buf) ? "🟢" : "🔴";
            cb(result + " " + String(buf));
        }


        _ipStart = "";
        _ipEnd = "";
    }
}

void Scanner::startScanPort(String ip) {
    ipToScan = ip;
}

void Scanner::startScanIp(String ipStart, String ipEnd) {
    _ipStart = ipStart;
    _ipEnd = ipEnd;
}

