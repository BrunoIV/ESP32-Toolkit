#pragma once
#include <Arduino.h>

struct NetworkInfo {
  String ip;
  String mask;
  String gateway;
  String dns;
  String mac;
  String ssid;
  int signal;   // RSSI

  NetworkInfo() :
    ip(""),
    mask(""),
    gateway(""),
    dns(""),
    mac(""),
    ssid(""),
    signal(0) {}

  float getDistance() const {
    int txPower = -59; // RSSI at 1m. Depends of device
    if (signal == 0) return -1.0;

    float ratio = signal * 1.0 / txPower;
    if (ratio < 1.0) {
      return pow(ratio, 10);
    } else {
      return 0.89976 * pow(ratio, 7.7095) + 0.111;
    }
  }
};