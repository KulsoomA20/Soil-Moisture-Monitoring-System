# Soil Moisture Monitoring & Control (ESP32 + Node.js + MQTT)<br>
This project monitors soil moisture with an ESP32 and uses a Raspberry Pi (Node.js server) as the backend.<br>
Communication happens via MQTT.<br>

# Components:<br>
# ESP32 (Arduino/PlatformIO)<br>
Reads soil moisture sensor (analog).<br>
Publishes data to MQTT (soil_moisture).<br>
Subscribes to LED control topic (ledcontrol).<br>
Hosts a simple web UI showing soil status.<br>
# Raspberry Pi- Node.js Server (Express + MQTT)<br>
Connects to the same MQTT broker.<br>
Subscribes to soil data.<br>
REST API:<br>
GET /api/sensors → get soil moisture & status.<br>
POST /api/ledcontrol → send LED ON/OFF commands.<br>

# Setup:<br>
# ESP32 (PlatformIO in VS Code)<br>
Update Wi-Fi + MQTT credentials in main.cpp.<br>
Install required libs in platformio.ini<br>
Upload to ESP32 → check Serial Monitor for IP.<br>
# Raspberry Pi (Node.js server)<br>
Runs at: http://<raspberrypi-ip>:3000<br>
