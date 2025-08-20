const express = require('express');
const mqtt = require('mqtt');
const bodyParser = require('body-parser');
const cors = require('cors');

// MQTT Broker and Topics
const mqttBroker = ''; // MQTT broker address
const mqttUser = 'mqtt_user'; // MQTT username
const mqttPassword = 'password'; // MQTT password

const mqttTopicSoilMoisture = 'soil_moisture'; // Topic for soil moisture data
const mqttTopicLEDControl = 'ledcontrol'; // Topic for pump/LED control

// Variable to store soil moisture data
let soilMoisture = null;
let moistureStatus = "Unknown";

// Create MQTT client
const client = mqtt.connect(mqttBroker, {
  username: mqttUser,
  password: mqttPassword,
});

// MQTT Event Handlers
client.on('connect', () => {
  console.log('Connected to MQTT Broker');

  // Subscribe to the soil moisture topic
  client.subscribe(mqttTopicSoilMoisture, (err) => {
    if (err) {
      console.error('Failed to subscribe to topic:', err);
    } else {
      console.log(`Subscribed to topic: ${mqttTopicSoilMoisture}`);
    }
  });
});

client.on('error', (err) => {
  console.error('MQTT Connection Error:', err);
});

client.on('message', (topic, message) => {
  const payload = message.toString();
  console.log(`Received message: Topic = ${topic}, Payload = ${payload}`);

  // Update soil moisture data if the message is for the soil moisture topic
  if (topic === mqttTopicSoilMoisture) {
    const parsedValue = parseInt(payload, 10);
    if (isNaN(parsedValue)) {
      console.error('Invalid soil moisture value received:', payload);
      return;
    }

    soilMoisture = parsedValue;

    // Determine moisture status
    if (soilMoisture < 3000) {
      moistureStatus = "Wet";
    } else if (soilMoisture < 3500) {
      moistureStatus = "Moist";
    } else {
      moistureStatus = "Dry";
    }

    console.log(`Soil Moisture: ${soilMoisture}, Status: ${moistureStatus}`);
  }
});

// Express App Setup
const app = express();
const port = 3000;

// Middleware
app.use(bodyParser.json());
app.use(cors());

// Route to fetch sensor data
app.get('/api/sensors', (req, res) => {
  if (soilMoisture === null) {
    return res.status(404).json({ message: 'No soil moisture data available' });
  }

  res.json({
    soilMoisture: soilMoisture,
    status: moistureStatus,
    message: `Current moisture status is ${moistureStatus}`,
  });
});

// Route to control LED
app.post('/api/ledcontrol', (req, res) => {
  const { command } = req.body;

  if (!command || (command !== 'onled' && command !== 'offled')) {
    return res.status(400).json({ message: 'Invalid command. Use "onled" or "offled".' });
  }

  // Publish the command to the LED control topic
  client.publish(mqttTopicLEDControl, command, (err) => {
    if (err) {
      console.error('Failed to publish command:', err);
      return res.status(500).json({ message: 'Failed to send command' });
    }

    console.log(`Published command to topic ${mqttTopicLEDControl}: ${command}`);
    res.json({
      message: `LED command "${command}" executed successfully.`,
    });
  });
});

// Start the server
app.listen(port, '0.0.0.0', () => {
  console.log(`Node.js server running on http://0.0.0.0:${port}`);
});
