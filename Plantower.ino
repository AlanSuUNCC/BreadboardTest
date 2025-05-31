// Plantower.ino - Plantower PM2.5 sensor functions

// Variables to store Plantower readings
uint16_t pm1_0;
uint16_t pm2_5;
uint16_t pm10_0;
int plantowerScanIndex = 0;

// PMS state management
unsigned long lastPmsWakeup = 0;
const unsigned long PMS_WAKE_PERIOD = 30000;  // Wake for 30 seconds
const unsigned long PMS_SLEEP_PERIOD = 60000; // Sleep for 60 seconds

enum PmsState {
  PMS_WAKE_UP,
  PMS_REQUEST_READ,
  PMS_READ_DATA,
  PMS_GO_TO_SLEEP
};

PmsState currentPmsState = PMS_WAKE_UP;
PMS::DATA pmsData;

void setupPlantower() {
  Serial.print("Initializing Plantower: ");
  
  // Initialize Serial2 for Plantower communication
  Serial2.begin(9600, SERIAL_8N1, 16, 17); // RX=16, TX=17
  
  // Set to passive mode
  pms.passiveMode();
  
  Serial.println("✅ Plantower initialized (Passive Mode)");
  Serial.println("Plantower will cycle: 30s awake, 60s sleep");
  
  lastPmsWakeup = millis();
}

void readPlantowerData() {
  unsigned long currentMillis = millis();
  
  switch (currentPmsState) {
    case PMS_WAKE_UP:
      if (currentMillis - lastPmsWakeup >= PMS_SLEEP_PERIOD) {
        Serial.println("Plantower: 🌅 Waking up sensor (30s warm-up)...");
        pms.wakeUp();
        lastPmsWakeup = currentMillis;
        currentPmsState = PMS_REQUEST_READ;
      }
      break;

    case PMS_REQUEST_READ:
      if (currentMillis - lastPmsWakeup >= PMS_WAKE_PERIOD) {
        Serial.println("Plantower: 📊 Requesting data...");
        pms.requestRead();
        currentPmsState = PMS_READ_DATA;
      }
      break;

    case PMS_READ_DATA:
      plantowerScanIndex++;
      
      if (pms.readUntil(pmsData, 3000)) {
        // Store readings
        pm1_0 = pmsData.PM_AE_UG_1_0;
        pm2_5 = pmsData.PM_AE_UG_2_5;
        pm10_0 = pmsData.PM_AE_UG_10_0;
        
        // Display readings
        Serial.print("Plantower #");
        Serial.print(plantowerScanIndex);
        Serial.print(": PM1.0 = ");
        Serial.print(pm1_0);
        Serial.print(" μg/m³, PM2.5 = ");
        Serial.print(pm2_5);
        Serial.print(" μg/m³, PM10 = ");
        Serial.print(pm10_0);
        Serial.println(" μg/m³ ✅");
      } else {
        Serial.print("Plantower #");
        Serial.print(plantowerScanIndex);
        Serial.println(": ❌ No data received");
      }
      
      currentPmsState = PMS_GO_TO_SLEEP;
      break;

    case PMS_GO_TO_SLEEP:
      Serial.println("Plantower: 💤 Going to sleep (60s)...");
      pms.sleep();
      lastPmsWakeup = currentMillis;
      currentPmsState = PMS_WAKE_UP;
      break;
  }
}