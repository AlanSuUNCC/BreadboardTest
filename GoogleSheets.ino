// GoogleSheets.ino - Google Sheets upload functions

const char* googleScriptUrl = "https://script.google.com/macros/s/AKfycbw8mrsM1mXwv6C1AER3deVW70S7gUAP9a9jzGX6b86CRIusbxYKCud7e394s8ebwtCStw/exec";

void sendToGoogleSheets() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Google Sheets: WiFi not connected, skipping upload");
    return;
  }

  Serial.println("Google Sheets: Uploading data...");
  
  // Create URL with sensor data parameters
  String url = String(googleScriptUrl);
  url += "?pm1=" + String(pm1_0);
  url += "&pm25=" + String(pm2_5);
  url += "&pm10=" + String(pm10_0);
  url += "&sgp30_co2=" + String(sgp30_CO2);
  url += "&sgp30_tvoc=" + String(sgp30_TVOC);
  url += "&scd41_co2=" + String(scd41_CO2);
  url += "&scd41_temp=" + String(scd41_Temperature);
  url += "&scd41_hum=" + String(scd41_Humidity);
  
  // Create HTTP client
  HTTPClient http;
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  
  // Begin the connection
  http.begin(url);
  
  // Send GET request
  int httpResponseCode = http.GET();
  
  // Handle response
  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.print("Google Sheets: Upload successful (Code: ");
    Serial.print(httpResponseCode);
    Serial.println(")");
    
    // Print first 50 characters of response
    if (response.length() > 50) {
      Serial.println("   Response: " + response.substring(0, 50) + "...");
    } else {
      Serial.println("   Response: " + response);
    }
  } else {
    Serial.print("Google Sheets: Upload failed (Error: ");
    Serial.print(httpResponseCode);
    Serial.println(")");
  }
  
  http.end();
}