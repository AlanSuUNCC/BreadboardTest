#include <Wire.h>
#include "Adafruit_SGP30.h"
#include <SensirionI2cScd4x.h>
#include "PMS.h"
#include <WiFi.h>
#include <HTTPClient.h>

// External variables from sensor files
extern float sgp30_CO2;
extern float sgp30_TVOC;

extern uint16_t scd41_CO2;
extern float scd41_Temperature;
extern float scd41_Humidity;

extern uint16_t pm1_0;
extern uint16_t pm2_5;
extern uint16_t pm10_0;

// Global sensor objects
Adafruit_SGP30 sgp;
SensirionI2cScd4x scd4x;
PMS pms(Serial2);

// Timing variables
unsigned long lastSensorRead = 0;
unsigned long lastGoogleSheetsTime = 0;

// Setup
void setupWiFi();
void setupSGP30();
void setupSCD41();
void setupPlantower();
void readSGP30Data();
void readSCD41Data();
void readPlantowerData();
void sendToGoogleSheets();

void setup() {
  Serial.begin(115200);
  Serial.println("Air Quality Monitor Starting...");
  
  // Initialize I2C
  Wire.begin(21, 22);
  Wire.setClock(100000);
  Serial.println("I2C initialized");
  
  // Initialize WiFi
  setupWiFi();
  
  // Initialize sensors
  setupSGP30();
  setupSCD41();
  setupPlantower();
  
  Serial.println("========================================");
  Serial.println("All sensors initialized - starting readings");
  Serial.println("========================================");
}

void loop() {
  unsigned long currentTime = millis();
  
  // Read sensors every 5 seconds
  if (currentTime - lastSensorRead >= 5000) {
    Serial.println("\n--- SENSOR READINGS ---");
    
    readSGP30Data();
    readSCD41Data();
    readPlantowerData();
    
    lastSensorRead = currentTime;
  }
  
  // Send to Google Sheets every 60 seconds
  if (currentTime - lastGoogleSheetsTime >= 60000) {
    sendToGoogleSheets();
    lastGoogleSheetsTime = currentTime;
  }
  
  delay(1000);
}