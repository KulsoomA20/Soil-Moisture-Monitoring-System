#include <WiFi.h>
#include <PubSubClient.h>
#include <WebServer.h>

// Wi-Fi Credentials
const char* ssid = "your ssid";
const char* password = "your password";

// MQTT Broker and Topics
const char* mqtt_server = "your server";
const int mqtt_port = 1883;
const char* mqtt_topic_soil = "soil_moisture";
const char* mqtt_topic_led = "ledcontrol";
const char* mqtt_username = "mqtt_user";
const char* mqtt_password = "password";

// Pins
const int soilMoisturePin = 35; // Analog pin for soil moisture sensor
const int led1 = 32; // Wet indicator
const int led2 = 33; // Moist indicator
const int led3 = 18; // Dry indicator

WiFiClient espClient;
PubSubClient client(espClient);
WebServer server(80);

int soilMoistureValue = 0;
String moistureStatus = "Unknown";

// Function to connect to Wi-Fi
void setupWifi() {
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

// MQTT message callback
void callback(char* topic, byte* payload, unsigned int length) {
  String msgString = "";
  for (int i = 0; i < length; i++) {
    msgString += (char)payload[i];
  }

  Serial.print("Message received on topic: ");
  Serial.println(topic);
  Serial.print("Message: ");
  Serial.println(msgString);

  if (msgString == "onled") {
    digitalWrite(led3, HIGH);
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
    Serial.println("LED turned ON");
  } else if (msgString == "offled") {
    digitalWrite(led3, LOW);
    digitalWrite(led1, HIGH);
    digitalWrite(led2, LOW);
    Serial.println("LED turned OFF");
  }
}

// Function to connect to MQTT broker
void setupMQTT() {
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("ESP32Sensor", mqtt_username, mqtt_password)) {
      Serial.println("Connected to MQTT");
      client.subscribe(mqtt_topic_led);
      Serial.println("Subscribed to topic: " + String(mqtt_topic_led));
    } else {
      Serial.print("Failed, rc=");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

// Function to send soil moisture data to MQTT
void sendDataToMQTT() {
  String moisture = String(soilMoistureValue);
  client.publish(mqtt_topic_soil, moisture.c_str());
  Serial.println("Published moisture: " + moisture);
  Serial.println(moistureStatus);
}

// Generate HTML for the web interface
String generateHTML() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Soil Moisture Control Panel</title>
  <style>
    body {
      font-family: 'Roboto', sans-serif;
      margin: 0;
      padding: 0;
      background: url('https://t3.ftcdn.net/jpg/06/56/97/20/240_F_656972077_jAon9gaXE4GwdjMYcRvw6eGiXHlsd2xC.jpg') no-repeat center center fixed;
      background-size: cover;
      color: white;
    }
    h1 {
      text-align: center;
      margin-top: 20px;
      font-size: 36px;
      text-shadow: 2px 2px 8px rgba(0, 0, 0, 0.4);
    }
    p {
      text-align: center;
      font-size: 18px;
      color: #f1f1f1;
      text-shadow: 1px 1px 5px rgba(0, 0, 0, 0.3);
    }
    .status {
      font-size: 22px;
      font-weight: bold;
      text-align: center;
      margin-top: 20px;
    }
    .container {
      width: 80%;
      max-width: 800px;
      margin: auto;
      padding: 30px;
      background-color: rgba(0, 0, 0, 0.6);
      border-radius: 15px;
      box-shadow: 0 0 20px rgba(0, 0, 0, 0.5);
      backdrop-filter: blur(10px);
      position: relative;
      top: 200px;
    }
  </style>
  <link href='https://fonts.googleapis.com/css2?family=Roboto:wght@300;400;500;700&display=swap' rel='stylesheet'>
</head>
<body>
  <div class='container'>
    <h1>Soil Moisture Control Panel</h1>
    <p>Soil Moisture Value: )rawliteral" + String(soilMoistureValue) + R"rawliteral(</p>
    <p>Status: <span class='status'>)" + moistureStatus + R"rawliteral(</span></p>
  </div>
</body>
</html>
)rawliteral";
  return html;
}

// Web server handlers
void handleRoot() {
  server.send(200, "text/html", generateHTML());
}

void handleLedOn() {
  client.publish(mqtt_topic_led, "onled");
  server.send(200, "text/plain", "LED turned ON");
}

void handleLedOff() {
  client.publish(mqtt_topic_led, "offled");
  server.send(200, "text/plain", "LED turned OFF");
}

void setup() {
  Serial.begin(115200);
  setupWifi();
  setupMQTT();

  // Initialize pins
  pinMode(soilMoisturePin, INPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);

  // Web server routes
  server.on("/", handleRoot);
  server.on("/led/on", handleLedOn);
  server.on("/led/off", handleLedOff);
  server.begin();

  Serial.println("Web server started");
}

void loop() {
  if (!client.connected()) {
    setupMQTT();
  }
  client.loop();
  server.handleClient();

  // Read soil moisture
  soilMoistureValue = analogRead(soilMoisturePin);
  if (soilMoistureValue < 3000) {
    moistureStatus = "Wet";
    digitalWrite(led1, HIGH);
    digitalWrite(led2, LOW);
    digitalWrite(led3, LOW);
  } else if (soilMoistureValue < 3500) {
    moistureStatus = "Moist";
    digitalWrite(led1, LOW);
    digitalWrite(led2, HIGH);
    digitalWrite(led3, LOW);
  } else {
    moistureStatus = "Dry";
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
    digitalWrite(led3, HIGH);
  }

  sendDataToMQTT();
  delay(5000); // Delay for 5 seconds
}
