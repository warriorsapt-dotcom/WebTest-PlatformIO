# ESP32 Web Test

PlatformIO firmware for an ESP32 Wi-Fi hotspot with internet sharing and a web page listing connected devices.

The device list shows IP and MAC addresses, refreshes every five seconds, and supports manual refresh. It lists clients connected to the ESP32 hotspot.

## Setup

1. Install PlatformIO and open this project.
2. Copy `src/secrets.example.h` to `src/secrets.h`.
3. Set your router credentials and hotspot name/password in `src/secrets.h`. Use a hotspot password of at least eight characters. This file is ignored by Git.
4. Build with `pio run` and upload with `pio run --target upload`.
5. Connect to the configured ESP32 hotspot and open `http://192.168.4.1`.

Leaving the router SSID as `YOUR_ROUTER_WIFI_NAME` runs the hotspot without internet sharing. Serial output is available at 115200 baud (`pio device monitor`).

## Files

- `src/main.cpp`: hotspot setup, internet sharing, HTTP server, and `/api/devices` endpoint.
- `src/Website.h`: embedded website.
- `platformio.ini`: ESP32 Dev Module build configuration.

Build output and local editor settings are excluded from Git. Device behavior must be verified on ESP32 hardware.
