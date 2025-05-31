// SGP30.ino - Adafruit SGP30 sensor functions

// Variables to store SGP30 readings
float sgp30_CO2;
float sgp30_TVOC;
int sgp30ScanIndex = 0;

void setupSGP30() {
  Serial.print("Initializing SGP30: ");
  
  if (!sgp.begin()) {
    Serial.println("SGP30 initialization failed!");
    return;
  }
  
  Serial.println("SGP30 initialized");
  
  // Initialize air quality measurement
  sgp.IAQinit();
  Serial.println("SGP30 air quality measurement started");
  
  // Display sensor info
  Serial.print("SGP30 Serial Number: ");
  Serial.print(sgp.serialnumber[0], HEX);
  Serial.print(sgp.serialnumber[1], HEX);
  Serial.println(sgp.serialnumber[2], HEX);
  
  Serial.println("Note: SGP30 needs 15 seconds for stable readings");
}

void readSGP30Data() {
  sgp30ScanIndex++;
  
  if (!sgp.IAQmeasure()) {
    Serial.println("SGP30: Measurement failed");
    return;
  }
  
  // Read the values
  sgp30_CO2 = sgp.eCO2;
  sgp30_TVOC = sgp.TVOC;
  
  // Display readings
  Serial.print("SGP30 #");
  Serial.print(sgp30ScanIndex);
  Serial.print(": eCO2 = ");
  Serial.print(sgp30_CO2, 0);
  Serial.print(" ppm, TVOC = ");
  Serial.print(sgp30_TVOC, 0);
  Serial.print(" ppb");
  
  // Indicate if readings are baseline or variable
  if (sgp30_CO2 == 400 && sgp30_TVOC == 0) {
    Serial.println(" (baseline - warming up)");
  } else {
    Serial.println("success");
  }
  
  // Show baseline values occasionally
  if (sgp30ScanIndex % 20 == 0) {
    uint16_t TVOC_base, eCO2_base;
    if (sgp.getIAQBaseline(&eCO2_base, &TVOC_base)) {
      Serial.print("  Baseline values: eCO2=0x");
      Serial.print(eCO2_base, HEX);
      Serial.print(", TVOC=0x");
      Serial.println(TVOC_base, HEX);
    }
  }
}