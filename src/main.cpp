#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <esp_wifi.h>
#include <esp_wifi_ap_get_sta_list.h>
#include "Website.h"

#include "secrets.h"

const IPAddress hotspotIp(192, 168, 4, 1);
const IPAddress hotspotMask(255, 255, 255, 0);
const IPAddress firstClientIp(192, 168, 4, 2);
const IPAddress clientDns(8, 8, 4, 4);

WebServer server(80);

void sendConnectedDevices() {
  wifi_sta_list_t clients = {};
  server.sendHeader("Cache-Control", "no-store");
  if (esp_wifi_ap_get_sta_list(&clients) != ESP_OK) {
    server.send(503, "application/json", "{\"error\":\"Could not read connected devices\"}");
    return;
  }

  wifi_sta_mac_ip_list_t addresses = {};
  const bool haveAddresses =
      esp_wifi_ap_get_sta_list_with_ip(&clients, &addresses) == ESP_OK;
  String json = "{\"devices\":[";
  for (int i = 0; i < clients.num; ++i) {
    if (i > 0) json += ',';
    const auto &client = clients.sta[i];
    char mac[18];
    snprintf(mac, sizeof(mac), "%02X:%02X:%02X:%02X:%02X:%02X",
             client.mac[0], client.mac[1], client.mac[2],
             client.mac[3], client.mac[4], client.mac[5]);
    String ip;
    if (haveAddresses) {
      for (int j = 0; j < addresses.num; ++j) {
        if (memcmp(client.mac, addresses.sta[j].mac, 6) == 0 &&
            addresses.sta[j].ip.addr != 0) {
          ip = IPAddress(addresses.sta[j].ip.addr).toString();
          break;
        }
      }
    }
    json += "{\"mac\":\"";
    json += mac;
    json += "\",\"ip\":\"";
    json += ip;
    json += "\"}";
  }
  json += "]}";
  server.send(200, "application/json", json);
}

void onRouterEvent(arduino_event_id_t event) {
  switch (event) {
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      Serial.print("Router assigned IP: ");
      Serial.println(WiFi.localIP());
      Serial.println(WiFi.AP.enableNAPT(true)
                       ? "Internet sharing enabled"
                       : "Could not enable internet sharing");
      break;
    case ARDUINO_EVENT_WIFI_STA_LOST_IP:
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
      WiFi.AP.enableNAPT(false);
      Serial.println("Router connection lost. Reconnecting...");
      break;
    default:
      break;
  }
}

void setup() {
  Serial.begin(115200);
  if (!WiFi.AP.begin() ||
      !WiFi.AP.config(hotspotIp, hotspotIp, hotspotMask, firstClientIp, clientDns) ||
      !WiFi.AP.create(hotspotSsid, hotspotPassword)) {
    Serial.println("Could not start ESP32-Test Wi-Fi");
    return;
  }

  server.on("/", HTTP_GET, []() {
    server.send_P(200, "text/html", websiteHtml);
  });
  server.on("/api/devices", HTTP_GET, sendConnectedDevices);

  server.begin();
  Serial.print("Open http://");
  Serial.println(WiFi.softAPIP());

  if (strcmp(routerSsid, "YOUR_ROUTER_WIFI_NAME") == 0) {
    Serial.println("Set routerSsid and routerPassword to enable internet sharing.");
    return;
  }

  Network.onEvent(onRouterEvent);
  WiFi.setAutoReconnect(true);
  Serial.println("Connecting to your router...");
  WiFi.begin(routerSsid, routerPassword);
}

void loop() {
  server.handleClient();
}
