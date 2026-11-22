#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <esp_wifi.h>
#include <esp_wifi_ap_get_sta_list.h>
#include <esp_timer.h>
#include <atomic>
#include "Website.h"

#include "secrets.h"

const IPAddress hotspotIp(192, 168, 4, 1);
const IPAddress hotspotMask(255, 255, 255, 0);
const IPAddress firstClientIp(192, 168, 4, 2);
const IPAddress clientDns(8, 8, 4, 4);

WebServer server(80);
std::atomic<bool> internetSharingEnabled{false};

String jsonString(const String &value) {
  String result = "\"";
  for (size_t i = 0; i < value.length(); ++i) {
    const unsigned char c = value[i];
    if (c == '"' || c == '\\') {
      result += '\\';
      result += static_cast<char>(c);
    } else if (c < 0x20) {
      char escaped[7];
      snprintf(escaped, sizeof(escaped), "\\u%04x", c);
      result += escaped;
    } else {
      result += static_cast<char>(c);
    }
  }
  return result + '"';
}

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
    json += "\",\"rssi\":";
    json += String(client.rssi);
    json += '}';
  }
  wifi_config_t apConfig = {};
  const bool haveConfig = esp_wifi_get_config(WIFI_IF_AP, &apConfig) == ESP_OK;
  const bool routerConnected = WiFi.status() == WL_CONNECTED;
  json += "],\"network\":{\"ssid\":" + jsonString(WiFi.softAPSSID());
  json += ",\"ip\":" + jsonString(WiFi.softAPIP().toString());
  json += ",\"mac\":" + jsonString(WiFi.softAPmacAddress());
  json += ",\"channel\":" + String(WiFi.channel());
  json += ",\"capacity\":" + (haveConfig ? String(apConfig.ap.max_connection) : String("null"));
  json += ",\"routerConnected\":" + String(routerConnected ? "true" : "false");
  json += ",\"routerSsid\":" + jsonString(routerConnected ? WiFi.SSID() : String());
  json += ",\"routerIp\":" + jsonString(routerConnected ? WiFi.localIP().toString() : String());
  json += ",\"routerRssi\":" + (routerConnected ? String(WiFi.RSSI()) : String("null"));
  json += ",\"sharingEnabled\":" + String(routerConnected && internetSharingEnabled.load() ? "true" : "false");
  json += ",\"uptimeSeconds\":" + String(static_cast<unsigned long>(esp_timer_get_time() / 1000000));
  json += ",\"freeHeap\":" + String(ESP.getFreeHeap());
  json += "}}";
  server.send(200, "application/json", json);
}

void onRouterEvent(arduino_event_id_t event) {
  switch (event) {
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      Serial.print("Router assigned IP: ");
      Serial.println(WiFi.localIP());
      internetSharingEnabled.store(WiFi.AP.enableNAPT(true));
      Serial.println(internetSharingEnabled.load()
                       ? "Internet sharing enabled"
                       : "Could not enable internet sharing");
      break;
    case ARDUINO_EVENT_WIFI_STA_LOST_IP:
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
      WiFi.AP.enableNAPT(false);
      internetSharingEnabled.store(false);
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
