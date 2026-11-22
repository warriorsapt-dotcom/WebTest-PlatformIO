#pragma once

#include <pgmspace.h>

const char websiteHtml[] PROGMEM = R"HTML(
<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Wi-Fi Overview | Warriorsapt</title>
<style>
:root { color-scheme: light; font-family: system-ui, sans-serif; color: #182638; background: #f3f5f9; }
* { box-sizing: border-box; }
body { margin: 0; font-size: 16px; }
.topbar { background: #142238; color: #fff; padding: 18px max(24px, calc((100vw - 1200px)/2)); display: flex; justify-content: space-between; gap: 20px; }
.topbar span { color: #c3d1e3; font-size: .875rem; }
main { max-width: 1248px; margin: auto; padding: 32px 24px; }
.header, .panel-head, .toolbar { display: flex; justify-content: space-between; align-items: center; gap: 16px; flex-wrap: wrap; }
h1 { font-size: 2rem; margin: 0 0 8px; letter-spacing: -.04em; }
h2 { font-size: 1.1rem; margin: 0 0 8px; }
p { line-height: 1.6; margin: 6px 0; }
.muted, .label { color: #526279; font-size: .875rem; }
button, input, select { font: inherit; border-radius: 6px; padding: 10px 14px; border: 1px solid #b8c5d6; background: white; color: #182638; }
button { cursor: pointer; }
button.primary { background: #155bd7; color: #fff; border-color: #155bd7; }
button:disabled { opacity: .55; cursor: wait; }
:focus-visible { outline: 3px solid #5299ff; outline-offset: 3px; }
#status { margin: 18px 0; padding: 12px 16px; background: #e7eef9; border-radius: 6px; font-size: .875rem; }
#status.error { background: #fff0da; color: #754800; }
.cards { display: grid; grid-template-columns: repeat(4, 1fr); gap: 16px; }
.card, .panel { background: #fff; border: 1px solid #dce3ed; border-radius: 10px; padding: 22px; }
.card { border-top: 3px solid #155bd7; }
.metric { font-size: 2rem; font-weight: 650; margin: 10px 0; letter-spacing: -.035em; }
.grid { display: grid; grid-template-columns: 1.4fr 1fr; gap: 20px; margin: 20px 0; }
.panel { min-width: 0; }
#history-chart { width: 100%; height: 180px; display: block; margin-top: 12px; }
.chart-labels { display: flex; justify-content: space-between; gap: 12px; }
.quality { width: 100%; display: flex; align-items: center; gap: 12px; margin-top: 12px; border-color: #e1e7f0; text-align: left; }
.quality[aria-pressed="true"] { outline: 2px solid #155bd7; }
.quality span:first-child { width: 70px; }
.track { height: 8px; background: #edf1f7; border-radius: 6px; flex: 1; overflow: hidden; }
.fill { display: block; height: 100%; width: 0; transition: width .2s; }
.good { background: #dff3e9; color: #146442; }
.average { background: #fff0cd; color: #795100; }
.poor { background: #fde5e4; color: #a02f29; }
.unknown { background: #e9edf3; color: #526279; }
.fill.good { background: #23875e; } .fill.average { background: #cb8b13; } .fill.poor { background: #cb514a; }
.badge { display: inline-block; border-radius: 20px; padding: 4px 10px; font-size: .875rem; }
dl { margin: 12px 0 0; }
dl div { display: flex; justify-content: space-between; gap: 16px; padding: 10px 0; border-bottom: 1px solid #edf0f5; }
dt { color: #526279; font-size: .875rem; } dd { margin: 0; text-align: right; overflow-wrap: anywhere; }
.table-wrap { overflow-x: auto; margin-top: 16px; }
table { width: 100%; border-collapse: collapse; text-align: left; }
th, td { padding: 14px 12px; border-bottom: 1px solid #e4eaf2; white-space: nowrap; }
th { background: #f7f9fc; font-size: .875rem; color: #526279; }
td { font-size: .9375rem; }
input { width: min(100%, 300px); }
.toolbar { margin-top: 16px; justify-content: flex-start; }
#empty { padding: 20px 0; }
footer { color: #526279; font-size: .875rem; margin-top: 24px; line-height: 1.6; }
@media (max-width: 850px) { .cards { grid-template-columns: repeat(2, 1fr); } .grid { grid-template-columns: 1fr; } }
@media (max-width: 480px) { main { padding: 24px 14px; } .cards { gap: 10px; } .card, .panel { padding: 16px; } .metric { font-size: 1.6rem; } .topbar { padding: 16px; } .topbar span { display: none; } }
</style>
</head>
<body>
<div class="topbar"><strong>Warriorsapt <span>/ Network monitor</span></strong><span>ESP32 Wi-Fi</span></div>
<main>
<div class="header"><div><h1>Wi-Fi overview</h1><p class="muted">Your hotspot, connected clients, and wireless signal at a glance.</p></div><button class="primary" id="refresh" type="button">Refresh now</button></div>
<p id="status" role="status">Connecting to the ESP32...</p>
<div class="cards">
<section class="card"><div class="label">Connected clients</div><div class="metric" id="client-count">&mdash;</div><div class="muted" id="capacity">Waiting for hotspot data</div></section>
<section class="card"><div class="label">Access points / SSIDs</div><div class="metric" id="ap-count">&mdash;</div><div class="muted" id="ssid-summary">Local ESP32 hotspot</div></section>
<section class="card"><div class="label">Router link</div><div class="metric" id="router-status">&mdash;</div><div class="muted" id="router-signal">Waiting for router data</div></section>
<section class="card"><div class="label">Device uptime</div><div class="metric" id="uptime">&mdash;</div><div class="muted" id="heap">Waiting for device data</div></section>
</div>
<div class="grid">
<section class="panel"><div class="panel-head"><div><h2>Connected clients over time</h2><p class="muted">Observed in this browser session</p></div><label class="muted">Window <select id="window"><option value="60">5 minutes</option><option value="180">15 minutes</option></select></label></div>
<svg id="history-chart" viewBox="0 0 600 180" role="img" aria-label="Waiting for client count samples"></svg>
<div class="chart-labels muted"><span id="history-start">Waiting for samples</span><span id="history-summary">No samples yet</span><span id="history-end"></span></div></section>
<section class="panel"><h2>Clients by signal quality</h2><p class="muted">RSSI measured at the ESP32. Select a group to filter.</p>
<button class="quality" data-quality="good" aria-pressed="false"><span>Good</span><span class="track"><span class="fill good" id="good-bar"></span></span><strong id="good-count">&mdash;</strong></button>
<button class="quality" data-quality="average" aria-pressed="false"><span>Average</span><span class="track"><span class="fill average" id="average-bar"></span></span><strong id="average-count">&mdash;</strong></button>
<button class="quality" data-quality="poor" aria-pressed="false"><span>Poor</span><span class="track"><span class="fill poor" id="poor-bar"></span></span><strong id="poor-count">&mdash;</strong></button>
<p class="muted" style="margin-top:14px">Good: &ge; -45 dBm &middot; Average: -69 to -46 dBm &middot; Poor: &le; -70 dBm</p></section>
</div>
<div class="grid">
<section class="panel"><h2>Access point details</h2><dl>
<div><dt>Network name (SSID)</dt><dd id="ap-ssid">&mdash;</dd></div><div><dt>Hotspot IP</dt><dd id="ap-ip">&mdash;</dd></div><div><dt>Access point MAC</dt><dd id="ap-mac">&mdash;</dd></div><div><dt>Radio / channel</dt><dd id="ap-channel">&mdash;</dd></div>
</dl></section>
<section class="panel"><h2>Upstream connection</h2><dl>
<div><dt>Router SSID</dt><dd id="router-ssid">&mdash;</dd></div><div><dt>Assigned IP</dt><dd id="router-ip">&mdash;</dd></div><div><dt>Internet sharing (NAPT)</dt><dd id="sharing">&mdash;</dd></div><div><dt>Internet reachability</dt><dd>Not tested</dd></div>
</dl></section>
</div>
<section class="panel"><div class="panel-head"><div><h2>Connected devices</h2><p class="muted">Clients currently associated with this hotspot</p></div><span id="shown" class="muted"></span></div>
<div class="toolbar"><input id="search" type="search" aria-label="Search by IP or MAC address" placeholder="Search IP or MAC address"><label class="muted">Signal <select id="quality-filter"><option value="all">All clients</option><option value="good">Good</option><option value="average">Average</option><option value="poor">Poor</option></select></label><label class="muted">Sort <select id="sort"><option value="weakest">Weakest signal first</option><option value="strongest">Strongest signal first</option><option value="ip">IP address</option></select></label></div>
<div class="table-wrap"><table id="devices-table" hidden><thead><tr><th scope="col">Device</th><th scope="col">IP address</th><th scope="col">MAC address</th><th scope="col">RSSI</th><th scope="col">Signal quality</th></tr></thead><tbody id="device-rows"></tbody></table></div><p id="empty" class="muted">Waiting for connected devices...</p>
</section>
<footer>Updates every 5 seconds. History keeps up to 15 minutes of samples in this tab and resets on reload or ESP32 restart. This ESP32 uses 2.4 GHz Wi-Fi; SNR, interference, throughput, and 5 GHz metrics are not collected.</footer>
</main>
<script>
const $ = id => document.getElementById(id);
const set = (id, value) => { $(id).textContent = value; };
const qualityOf = rssi => rssi >= -45 ? 'good' : rssi >= -69 ? 'average' : 'poor';
const title = value => value[0].toUpperCase() + value.slice(1);
let devices = [], samples = [], loading = false, refreshTimer, lastUptime = null, online = false;
function renderDevices() {
  const query = $('search').value.trim().toLowerCase();
  const filter = $('quality-filter').value;
  document.querySelectorAll('[data-quality]').forEach(button => button.setAttribute('aria-pressed', String(button.dataset.quality === filter)));
  const visible = devices.filter(d => (filter === 'all' || qualityOf(d.rssi) === filter) && (d.ip + ' ' + d.mac).toLowerCase().includes(query));
  const order = $('sort').value;
  visible.sort((a, b) => order === 'ip' ? a.ip.localeCompare(b.ip, undefined, { numeric: true }) : order === 'strongest' ? b.rssi - a.rssi : a.rssi - b.rssi);
  $('device-rows').replaceChildren();
  visible.forEach((device, i) => {
    const row = document.createElement('tr');
    [String(i + 1), device.ip || 'Not available yet', device.mac, device.rssi + ' dBm', title(qualityOf(device.rssi))].forEach((value, j) => {
      const cell = document.createElement('td');
      if (j === 4) { const badge = document.createElement('span'); badge.className = 'badge ' + qualityOf(device.rssi); badge.textContent = value; cell.appendChild(badge); }
      else cell.textContent = value;
      row.appendChild(cell);
    });
    $('device-rows').appendChild(row);
  });
  $('devices-table').hidden = !visible.length;
  $('empty').hidden = visible.length > 0;
  set('empty', !online ? 'Live device data is unavailable. Retrying...' : devices.length ? 'No devices match these filters.' : 'No devices are connected to the hotspot.');
  set('shown', online ? visible.length + ' of ' + devices.length + ' clients' : 'Unavailable');
}
function chartNode(tag, attributes) {
  const node = document.createElementNS('http://www.w3.org/2000/svg', tag);
  Object.entries(attributes).forEach(([key, value]) => node.setAttribute(key, value));
  return node;
}
function renderHistory() {
  const chart = $('history-chart'); chart.replaceChildren();
  const end = Date.now(), start = end - Number($('window').value) * 5000;
  const points = samples.filter(s => s.time >= start);
  const max = Math.max(1, ...points.map(s => s.count));
  for (let i = 0; i <= 2; i++) {
    const y = 20 + i * 65;
    chart.appendChild(chartNode('line', { x1: 36, x2: 588, y1: y, y2: y, stroke: '#e4eaf2' }));
    const label = chartNode('text', { x: 4, y: y + 5, fill: '#526279', 'font-size': 14 }); label.textContent = (max * (2 - i) / 2).toFixed(1).replace('.0', ''); chart.appendChild(label);
  }
  let previous = null;
  points.forEach(s => {
    const x = 36 + (s.time - start) / (end - start) * 552, y = 150 - s.count / max * 130;
    if (previous && s.time - previous.time < 15000) chart.appendChild(chartNode('line', { x1: previous.x, y1: previous.y, x2: x, y2: y, stroke: '#155bd7', 'stroke-width': 2 }));
    const dot = chartNode('circle', { cx: x, cy: y, r: 3, fill: '#155bd7' });
    const tooltip = chartNode('title', {}); tooltip.textContent = new Date(s.time).toLocaleTimeString() + ': ' + s.count + ' clients'; dot.appendChild(tooltip); chart.appendChild(dot);
    previous = { x, y, time: s.time };
  });
  set('history-start', new Date(start).toLocaleTimeString()); set('history-end', new Date(end).toLocaleTimeString());
  const summary = points.length ? 'Peak: ' + Math.max(...points.map(s => s.count)) + ' clients / ' + points.length + ' samples' : 'Waiting for samples';
  set('history-summary', summary); chart.setAttribute('aria-label', 'Connected client history. ' + summary);
}
function renderNetwork(n) {
  set('client-count', devices.length); set('capacity', n.capacity == null ? '2.4 GHz clients' : devices.length + ' of ' + n.capacity + ' client slots used');
  set('ap-count', '1 / 1'); set('ssid-summary', n.ssid); set('router-status', n.routerConnected ? 'Connected' : 'Offline');
  set('router-signal', n.routerConnected ? n.routerRssi + ' dBm / ' + title(qualityOf(n.routerRssi)) : 'Hotspot remains available');
  const seconds = n.uptimeSeconds;
  set('uptime', Math.floor(seconds / 3600) + 'h ' + Math.floor(seconds % 3600 / 60) + 'm');
  set('heap', Math.round(n.freeHeap / 1024) + ' KB free memory');
  set('ap-ssid', n.ssid); set('ap-ip', n.ip); set('ap-mac', n.mac); set('ap-channel', '2.4 GHz / channel ' + n.channel);
  set('router-ssid', n.routerSsid || 'Disconnected'); set('router-ip', n.routerIp || 'Not assigned'); set('sharing', n.sharingEnabled ? 'Enabled' : 'Disabled');
  ['good', 'average', 'poor'].forEach(q => { const count = devices.filter(d => qualityOf(d.rssi) === q).length; set(q + '-count', count); $(q + '-bar').style.width = (devices.length ? count / devices.length * 100 : 0) + '%'; });
}
function clearLiveData() {
  ['client-count','ap-count','router-status','uptime','ap-ssid','ap-ip','ap-mac','ap-channel','router-ssid','router-ip','sharing','good-count','average-count','poor-count'].forEach(id => set(id, '\u2014'));
  ['capacity','ssid-summary','router-signal','heap'].forEach(id => set(id, 'Live data unavailable'));
  ['good','average','poor'].forEach(q => { $(q + '-bar').style.width = '0%'; });
}
async function refresh() {
  if (loading) return;
  clearTimeout(refreshTimer); loading = true; $('refresh').disabled = true;
  const controller = new AbortController(), timeout = setTimeout(() => controller.abort(), 8000);
  try {
    const response = await fetch('/api/devices', { cache: 'no-store', signal: controller.signal });
    if (!response.ok) throw new Error('Request failed');
    const data = await response.json();
    if (!Array.isArray(data.devices) || !data.network) throw new Error('Invalid response');
    devices = data.devices; online = true;
    if (lastUptime !== null && data.network.uptimeSeconds < lastUptime) samples = [];
    lastUptime = data.network.uptimeSeconds;
    const now = Date.now(); samples.push({ time: now, count: devices.length }); samples = samples.filter(s => now - s.time <= 900000).slice(-180);
    renderNetwork(data.network); renderDevices(); renderHistory();
    $('status').className = ''; set('status', 'Live / Last updated ' + new Date().toLocaleTimeString() + ' / Refreshes every 5 seconds');
  } catch (error) {
    online = false; devices = []; clearLiveData(); renderDevices(); renderHistory();
    $('status').className = 'error'; set('status', 'Connection lost. Check your connection to the ESP32. Retrying automatically; chart shows previous observations.');
  } finally { clearTimeout(timeout); loading = false; $('refresh').disabled = false; refreshTimer = setTimeout(refresh, 5000); }
}
$('refresh').addEventListener('click', refresh);
['search', 'quality-filter', 'sort'].forEach(id => $(id).addEventListener(id === 'search' ? 'input' : 'change', renderDevices));
$('window').addEventListener('change', renderHistory);
document.querySelectorAll('[data-quality]').forEach(button => button.addEventListener('click', () => { $('quality-filter').value = $('quality-filter').value === button.dataset.quality ? 'all' : button.dataset.quality; renderDevices(); }));
refresh();
</script>
</body>
</html>
)HTML";
