#include <Wire.h> 
#include "Adafruit_SGP30.h" // SGP30 Library
#include <SensirionI2cScd4x.h> // SCD41 Library
#include "PMS.h" // PMS Library
#include <WiFi.h> // Wifi Library
#include <HTTPClient.h> // HTTP Client Library
#include <time.h> // Timing Library
#include <PubSubClient.h> // MQTT Publishing Library

// External variables from sensor files
extern float sgp30_CO2;
extern float sgp30_TVOC;

extern uint16_t scd41_CO2;
extern float scd41_Temperature;
extern float scd41_Humidity;

extern uint16_t pm1_0;
extern uint16_t pm2_5;
extern uint16_t pm10_0;

// Initialize MQTT client
WiFiClient espClient; 
PubSubClient mqttClient(espClient);

// ------------- Define Data Structures of Data Packets sent from child ESP-32s ----------- //

struct timeComponents {
  int year;
  int month;
  int day;
  int hour;
  int minute;
  int second;
  int dayOfWeek;
};

timeComponents result;

void getTime() {
  struct tm timeStruct;

  if(!getLocalTime(&timeStruct)){
    Serial.println("Failed to obtain time");
    result.year = -1;
    result.month = -1;
    result.day = -1;
    result.hour = -1;
    result.minute = -1;
    result.second = -1;
    result.dayOfWeek = -1;
  }

  result.year = timeStruct.tm_year + 1900;
  result.month = timeStruct.tm_mon + 1;
  result.day = timeStruct.tm_mday;
  result.hour = timeStruct.tm_hour;
  result.minute = timeStruct.tm_min;
  result.second = timeStruct.tm_sec;
  result.dayOfWeek = timeStruct.tm_wday;
}

void updateNTPTime() {
  const long gmtOffset_sec = 0;
  const int daylightOffset_sec = 0; 
  

    configTime(-4 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print("NTP time update... ");
}

// Global sensor objects
Adafruit_SGP30 sgp;
SensirionI2cScd4x scd4x;
PMS pms(Serial2);

// Timing variables
unsigned long lastSensorRead = 0;
unsigned long lastGoogleSheetsTime = 0;
unsigned long lastNTPUpdateTime = 0;

int lastProcessedMinute = -1;
unsigned long lastMQTTTime = 0;

// Setup functions
void setupWiFi();
void getTime();
void setupSGP30();
void setupSCD41();
void setupPlantower();
void readSGP30Data();
void readSCD41Data();
void readPlantowerData();
void sendToGoogleSheets();
void setupMQTT();
void handleMQTT();
void publishAllData();

void setup() {
  Serial.begin(115200);
  Serial.println("Air Quality Monitor Starting...");
  
  // Initialize I2C
  Wire.begin(21, 22);
  Wire.setClock(100000);
  Serial.println("I2C initialized");
  
  // Initialize WiFi & Time
  setupWiFi();
  getTime();
  
  // Initialize MQTT communication protocol

  setupMQTT();
  
  // Initialize sensors
  setupSGP30();
  setupSCD41();
  setupPlantower();
  
  Serial.println("========================================");
  Serial.println("All sensors initialized - starting readings");
  Serial.println("========================================");
}

void loop() {

  handleMQTT();

  if (millis() - lastNTPUpdateTime >= 3600000) {
    updateNTPTime();
    lastNTPUpdateTime = millis();
  }

  getTime();
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
  //if (result.second == 0 && result.minute != lastProcessedMinute && result.year != -1) {
  //  sendToGoogleSheets();
  //  lastProcessedMinute = result.minute; 
  //}

  if (currentTime - lastMQTTTime >= 30000) {
    publishAllData();
    lastMQTTTime = currentTime;
  }
  
  delay(1000);
}