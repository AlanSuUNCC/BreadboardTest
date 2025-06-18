// WiFi.ino - WiFi connection functions

// Replace with your actual WiFi credentials
const char* ssid = "CTPro";
const char* password = "j1ankangyukua1";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600;

void setupWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.print(ssid);
  Serial.print("...");
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  
  // Wait for connection with timeout
  int timeout = 0;
  while (WiFi.status() != WL_CONNECTED && timeout < 20) {
    delay(500);
    Serial.print(".");
    timeout++;
  }
  
  // Check if connected successfully
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.println("WiFi connected successfully!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println();
    Serial.println("WiFi connection failed!");
    Serial.println("Continuing without WiFi...");
  }
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}