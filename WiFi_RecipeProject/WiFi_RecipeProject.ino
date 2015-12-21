#include <WiFi.h>
#include <WiFiClient.h>

#include <Temboo.h>
#include "TembooAccount.h" // Contains Temboo account information

WiFiClient client;

void setup() {
  
  Serial.begin(9600);
  pinMode(7, INPUT);
  int wifiStatus = WL_IDLE_STATUS;

  // Determine if the WiFi Shield is present
  Serial.print("\n\nShield:");
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("FAIL");

    // If there's no WiFi shield, stop here
    while(true);
  }

  Serial.println("OK");

  // Try to connect to the local WiFi network
  while(wifiStatus != WL_CONNECTED) {
    Serial.print("WiFi:");
    wifiStatus = WiFi.begin(WIFI_SSID, WPA_PASSWORD);

    if (wifiStatus == WL_CONNECTED) {
      Serial.println("OK");
    } else {
      Serial.println("FAIL");
    }
    delay(5000);
  }

  Serial.println("Setup complete.\n");
}

void loop() {
  if (!digitalRead(7)) {

    TembooChoreo GetSearchResultChoreo(client);
    TembooChoreo StatusesUpdateChoreo(client);
    
    // Invoke the Temboo client
    GetSearchResultChoreo.begin();
    // Invoke the Temboo client
    StatusesUpdateChoreo.begin();

    // Set Temboo account credentials
    GetSearchResultChoreo.setAccountName(TEMBOO_ACCOUNT);
    GetSearchResultChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
    GetSearchResultChoreo.setAppKey(TEMBOO_APP_KEY);
    
    // Set Temboo account credentials
    StatusesUpdateChoreo.setAccountName(TEMBOO_ACCOUNT);
    StatusesUpdateChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
    StatusesUpdateChoreo.setAppKey(TEMBOO_APP_KEY);

    // Set profile to use for execution
    GetSearchResultChoreo.setProfile("WolframAlphaAccount");
    // Set profile to use for execution
    StatusesUpdateChoreo.setProfile("twitternew");

    // Set Choreo inputs
    String InputValue = "tell me a  joke";
    GetSearchResultChoreo.addInput("Input", InputValue);

    // Identify the Choreo to run
    GetSearchResultChoreo.setChoreo("/Library/WolframAlpha/GetSearchResult");
    // Identify the Choreo to run
    StatusesUpdateChoreo.setChoreo("/Library/Twitter/Tweets/StatusesUpdate");
    
    // Run the Choreo; when results are available, print them to serial
    GetSearchResultChoreo.run();
    
    String name = "";
    while(GetSearchResultChoreo.available()) {
      name = GetSearchResultChoreo.readStringUntil('\x1F');
      name.trim();
    }
    int length = name.indexOf('\n');
    String joke = name.substring(0,length);
    Serial.println(name);
    Serial.println(joke);
    
    GetSearchResultChoreo.close();
    
    if(joke){
      StatusesUpdateChoreo.addInput("StatusUpdate", joke);

      // Run the Choreo; when results are available, print them to serial
      StatusesUpdateChoreo.run();
    
      while(StatusesUpdateChoreo.available()) {
        char c = StatusesUpdateChoreo.read();
        Serial.print(c);
      }
      
      
    }
    StatusesUpdateChoreo.close();
  }

  Serial.println("\nWaiting...\n");
  delay(30000); // wait 30 seconds between GetSearchResult calls
}
