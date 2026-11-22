#pragma once

#include <pgmspace.h>

const char websiteHtml[] PROGMEM = R"HTML(
<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>ESP32 Web Test</title>
<style>
body { font-family: system-ui, sans-serif; max-width: 800px; margin: 40px auto; padding: 0 20px; color: #172b3a; background: #f3f6fa; }
section { background: white; border: 1px solid #dce3eb; border-radius: 12px; padding: 20px; margin-top: 28px; }
h2 { margin-top: 0; }
button { background: #1763bd; color: white; border: 0; border-radius: 6px; padding: 10px 16px; cursor: pointer; }
button:disabled { opacity: .6; cursor: wait; }
.table-wrap { overflow-x: auto; }
table { width: 100%; border-collapse: collapse; text-align: left; }
th, td { padding: 12px 8px; border-bottom: 1px solid #e5eaf0; white-space: nowrap; }
#device-status { color: #465568; }
</style>
</head>
<body>
<h1>Website by Warriorsapt</h1>
<h1>ESP32 web server is running</h1>
<p>This page was served directly from your ESP32.</p>
<section aria-labelledby="devices-heading">
<h2 id="devices-heading">Connected devices (<span id="device-count">&ndash;</span>)</h2>
<p>Devices connected to this ESP32's Wi-Fi hotspot. Updates every 5 seconds.</p>
<button id="refresh-devices" type="button">Refresh now</button>
<p id="device-status" role="status">Loading connected devices...</p>
<div class="table-wrap">
<table aria-label="Connected hotspot devices" hidden id="devices-table">
<thead><tr><th scope="col">Device</th><th scope="col">IP address</th><th scope="col">MAC address</th></tr></thead>
<tbody id="device-rows"></tbody>
</table>
</div>
</section>
<script>
const refreshButton = document.getElementById('refresh-devices');
const statusText = document.getElementById('device-status');
const deviceTable = document.getElementById('devices-table');
const deviceRows = document.getElementById('device-rows');
const deviceCount = document.getElementById('device-count');
let loading = false;
let refreshTimer;
async function refreshDevices() {
  if (loading) return;
  clearTimeout(refreshTimer);
  loading = true;
  refreshButton.disabled = true;
  const controller = new AbortController();
  const timeout = setTimeout(() => controller.abort(), 8000);
  try {
    const response = await fetch('/api/devices', { cache: 'no-store', signal: controller.signal });
    if (!response.ok) throw new Error('Unable to load devices');
    const { devices } = await response.json();
    deviceRows.replaceChildren();
    devices.forEach((device, index) => {
      const row = document.createElement('tr');
      [String(index + 1), device.ip || 'Not available yet', device.mac].forEach(value => {
        const cell = document.createElement('td');
        cell.textContent = value;
        row.appendChild(cell);
      });
      deviceRows.appendChild(row);
    });
    deviceCount.textContent = devices.length;
    deviceTable.hidden = devices.length === 0;
    statusText.textContent = devices.length
      ? 'Last updated: ' + new Date().toLocaleTimeString()
      : 'No devices are connected to the hotspot.';
  } catch (error) {
    deviceCount.textContent = '\u2013';
    deviceRows.replaceChildren();
    deviceTable.hidden = true;
    statusText.textContent = 'Could not load devices. Check your connection to the ESP32. Retrying...';
  } finally {
    clearTimeout(timeout);
    loading = false;
    refreshButton.disabled = false;
    refreshTimer = setTimeout(refreshDevices, 5000);
  }
}
refreshButton.addEventListener('click', refreshDevices);
refreshDevices();
</script>
</body>
</html>
)HTML";
