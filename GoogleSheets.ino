const char* googleScriptUrl = "https://script.google.com/macros/s/AKfycbzv617sEFNVrmBtbawn26BqajKMMgHIGNzbpSSostyBBxczqxbJDl-PcSQEiwKMZDEn/exec";

void sendToGoogleSheets() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Google Sheets: WiFi not connected");
    return;
  }

  Serial.println("Google Sheets: Preparing upload...");
  
  // Clean the data - replace invalid values with 0
  float clean_sgp30_co2 = (sgp30_CO2 > 0 && sgp30_CO2 < 50000) ? sgp30_CO2 : 0;
  float clean_sgp30_tvoc = (sgp30_TVOC >= 0 && sgp30_TVOC < 10000) ? sgp30_TVOC : 0;
  int clean_scd41_co2 = (scd41_CO2 > 0 && scd41_CO2 < 10000) ? scd41_CO2 : 0;
  float clean_scd41_temp = (scd41_Temperature > -50 && scd41_Temperature < 100) ? scd41_Temperature : 0;
  float clean_scd41_hum = (scd41_Humidity >= 0 && scd41_Humidity <= 100) ? scd41_Humidity : 0;
  int clean_pm1 = (pm1_0 >= 0 && pm1_0 < 1000) ? pm1_0 : 0;
  int clean_pm25 = (pm2_5 >= 0 && pm2_5 < 1000) ? pm2_5 : 0;
  int clean_pm10 = (pm10_0 >= 0 && pm10_0 < 1000) ? pm10_0 : 0;
  
  // Create URL with cleaned parameters (no spaces!)
  String url = String(googleScriptUrl);
  url += "?sgp30_co2=" + String(clean_sgp30_co2, 0);
  url += "&sgp30_tvoc=" + String(clean_sgp30_tvoc, 0);
  url += "&scd41_co2=" + String(clean_scd41_co2);
  url += "&scd41_temp=" + String(clean_scd41_temp, 2);
  url += "&scd41_hum=" + String(clean_scd41_hum, 2);
  url += "&pm1=" + String(clean_pm1);
  url += "&pm25=" + String(clean_pm25);
  url += "&pm10=" + String(clean_pm10);
  
  // URL encode any remaining spaces (safety measure)
  url.replace(" ", "%20");
  
  Serial.println("Clean URL: " + url);
  
  // Create HTTP client
  HTTPClient http;
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  http.setTimeout(15000);
  
  // Send GET request
  http.begin(url);
  int httpResponseCode = http.GET();
  
  // Handle response
  if (httpResponseCode == 200) {
    String response = http.getString();
    Serial.println("Google Sheets: Upload successful!");
    
    // Show response if it's JSON
    if (response.startsWith("{")) {
      Serial.println("Response: " + response);
    }
  } else {
    Serial.print("Google Sheets: HTTP ");
    Serial.println(httpResponseCode);
  }
}