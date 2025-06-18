// MQTT server configuration
const char* mqtt_server = "192.168.86.41"; // MQTT server IP address
const int mqtt_port = 1883;

void setupMQTT() {
  mqttClient.setServer(mqtt_server, mqtt_port);
  Serial.println("MQTT client configured");
  Serial.print("MQTT server: ");
  Serial.println(mqtt_server);
}

void handleMQTT() {
  if (!mqttClient.connected()) {
    reconnectMQTT();
  }
  mqttClient.loop();
}

// Function to handle MQTT client reconnecting, tries for 3 attempts (configurable)
void reconnectMQTT() {
  int attempts = 0;
  while (!mqttClient.connected() && attempts < 3) {
    Serial.print("Connecting to MQTT...");
    String clientId = "AirQualityESP32-" + String(random(0xffff), HEX);
    
    if (mqttClient.connect(clientId.c_str())) {
      Serial.println(" connected!");
      Serial.print("Client ID: ");
      Serial.println(clientId);
    } else {
      Serial.print(" failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" retrying in 5 seconds");
      delay(5000);
      attempts++;
    }
  }
}

void publishAllData() {
  if (!mqttClient.connected()) { 
    Serial.println("MQTT not connected! Skipping publish");  // Skip publishing if not connected and return
    return;
  }
  
  char buffer[10];
  
  Serial.println("MQTT: Publishing sensor data...");
  
  // Publish timestamp
  dtostrf(millis(), 0, 0, buffer);
  mqttClient.publish("airquality/timestamp", buffer);
  
  // SGP30 data
  dtostrf(sgp30_CO2, 5, 0, buffer);
  mqttClient.publish("airquality/sgp30_co2_ppm", buffer);
  
  dtostrf(sgp30_TVOC, 5, 0, buffer);
  mqttClient.publish("airquality/voc_ppb", buffer);
  
  // SCD41 data
  itoa(scd41_CO2, buffer, 10);
  mqttClient.publish("airquality/scd41_co2_ppm", buffer);
  
  dtostrf(scd41_Temperature, 5, 2, buffer);
  mqttClient.publish("airquality/temperature_c", buffer);
  
  dtostrf(scd41_Humidity, 5, 2, buffer);
  mqttClient.publish("airquality/relative_humidity", buffer);
  
  // Plantower PM data
  itoa(pm1_0, buffer, 10);
  mqttClient.publish("airquality/pm_10", buffer);
  
  itoa(pm2_5, buffer, 10);
  mqttClient.publish("airquality/pm_25", buffer);
  
  itoa(pm10_0, buffer, 10);
  mqttClient.publish("airquality/pm_100", buffer);
  
  Serial.println("MQTT: All data published successfully");
}