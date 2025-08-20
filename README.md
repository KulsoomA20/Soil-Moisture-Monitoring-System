#Soil Moisture Monitoring & Control (ESP32 + Node.js + MQTT)
#This project monitors soil moisture with an ESP32 and uses a Raspberry Pi (Node.js server) as the backend.
Communication happens via MQTT.

#Components:
#*ESP32 (Arduino/PlatformIO)
    #Reads soil moisture sensor (analog).
    #Publishes data to MQTT (soil_moisture).
    #Subscribes to LED control topic (ledcontrol).
    #Hosts a simple web UI showing soil status.
#*Raspberry Pi- Node.js Server (Express + MQTT)
    #Connects to the same MQTT broker.
    #Subscribes to soil data.
    #REST API:
        #GET /api/sensors → get soil moisture & status.
        #POST /api/ledcontrol → send LED ON/OFF commands.

#Setup:
#*ESP32 (PlatformIO in VS Code)
    #Update Wi-Fi + MQTT credentials in main.cpp.
    #Install required libs in platformio.ini
    #Upload to ESP32 → check Serial Monitor for IP.
#*Raspberry Pi (Node.js server)
    #Runs at: http://<raspberrypi-ip>:3000
