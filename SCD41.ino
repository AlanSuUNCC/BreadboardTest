// SCD41 CO2/Temperature/Humidity sensor functions

// Variables to store SCD41 readings
uint16_t scd41_CO2;
float scd41_Temperature;
float scd41_Humidity;
int scd41ScanIndex = 0;

void setupSCD41() {
  Serial.print("Initializing SCD41: ");
  
  // Initialize SCD41 with I2C address
  scd4x.begin(Wire, 0x62);
  
  uint16_t error;
  
  // Stop any running measurement
  error = scd4x.stopPeriodicMeasurement();
  if (error) {
    Serial.print("Stop measurement warning: 0x");
    Serial.println(error, HEX);
  }
  delay(500);
  
  // Start periodic measurement
  error = scd4x.startPeriodicMeasurement();
  if (error) {
    Serial.print("Start measurement failed: 0x");
    Serial.println(error, HEX);
    return;
  }
  
  Serial.println("SCD41 initialized");
  
  // Get serial number
  uint64_t serialNumber;
  error = scd4x.getSerialNumber(serialNumber);
  if (!error) {
    Serial.print("SCD41 Serial Number: 0x");
    Serial.println(serialNumber, HEX);
  }
  
  Serial.println("Note: SCD41 needs ~5 seconds for first measurement");
}

void readSCD41Data() {
  scd41ScanIndex++;
  
  uint16_t error;
  bool isDataReady = false;
  
  // Check if data is ready
  error = scd4x.getDataReadyStatus(isDataReady);
  if (error) {
    Serial.print("SCD41 #");
    Serial.print(scd41ScanIndex);
    Serial.println("Data ready check failed");
    return;
  }
  
  if (!isDataReady) {
    Serial.print("SCD41 #");
    Serial.print(scd41ScanIndex);
    Serial.println("Data not ready yet");
    return;
  }
  
  // Read measurement
  float temperature, humidity;
  error = scd4x.readMeasurement(scd41_CO2, temperature, humidity);
  if (error) {
    Serial.print("SCD41 #");
    Serial.print(scd41ScanIndex);
    Serial.println("Read measurement failed");
    return;
  }
  
  if (scd41_CO2 == 0) {
    Serial.print("SCD41 #");
    Serial.print(scd41ScanIndex);
    Serial.println("Invalid measurement (CO2 = 0)");
    return;
  }
  
  // Store values
  scd41_Temperature = temperature;
  scd41_Humidity = humidity;
  
  // Display readings
  Serial.print("SCD41 #");
  Serial.print(scd41ScanIndex);
  Serial.print("CO2 = ");
  Serial.print(scd41_CO2);
  Serial.print(" ppm, Temp = ");
  Serial.print(scd41_Temperature, 1);
  Serial.print("C, Humidity = ");
  Serial.print(scd41_Humidity, 1);
  Serial.println("Success");
}