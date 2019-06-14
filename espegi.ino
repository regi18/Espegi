 /***************************************************
 Espegi - Sonoff (esp8266 based) control with Blynk and web interface
 Created by: regi18
 Version: Version: 3.5.0
 Github: 

 regi18.ml   |   regi18.github.io
 **************************************************/
 
#include "config.h"
#include "webpage.h"
#include <BlynkSimpleEsp8266.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#ifdef DHT_SENSOR
  #include <DHT.h>
#endif

int state = 0;                                        // Variable to store the relay state
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = BUTTON_DEBOUNCE_DELAY;  // The minimum time (ms) between button clicks
#ifdef DHT_SENSOR
  float temp = 0, hum = 0;                            // Variables for the DHT sensor
#endif

ESP8266WebServer server(80);

#ifdef DHT_SENSOR
  DHT dht(DHT_PIN, DHT_TYPE);
  WiFiClient client;
#endif

BlynkTimer timer;


// Prototipes
void toggleRelay();
void handleRoot();
void handleToggleRelay();
void handleRestart();
void handleUpdatePage();
void updateFirmware();
void checkButton();
void getDhtData();
void sendDataTS();


void setup()
{
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);

  #ifdef DHT_SENSOR
    pinMode(DHT_PIN, INPUT);
    dht.begin();
    timer.setInterval(15000L, getDhtData);
    timer.setInterval(1200000L, sendDataTS);
  #endif

  timer.setInterval(150L, checkButton);
  
  WiFi.hostname(HOSTNAME);
  Blynk.begin(auth, ssid, wifiPassword);

  digitalWrite(LED_PIN, 0);
  delay(700);
  digitalWrite(LED_PIN, 1);
  delay(700);
  digitalWrite(LED_PIN, 0);
  delay(700);
  digitalWrite(LED_PIN, 1);

  // Web Server Setup
  server.on("/", handleRoot);
  server.on("/toggle", handleToggleRelay);
  server.on("/restart", handleRestart);
  server.on("/forcereload", handleRoot);
  server.on("/togglecmd", handleToggleRelayCmd);
  #ifdef DHT_SENSOR
    server.on("/temperaturecmd", handleTemperatureCmd);
  #endif

  // Update page (where you can choose and upload a file)
  server.on("/update", HTTP_GET, [](){
      if(ota_username != emptyString && ota_password != emptyString && !server.authenticate(ota_username.c_str(), ota_password.c_str()))
        return server.requestAuthentication();
      handleUpdatePage();
  });

  server.on("/startupdate", HTTP_POST, []() {
      server.sendHeader("Connection", "close");
      server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
      ESP.restart();
  }, updateFirmware);

  server.begin();
}


void loop()
{
  if (Blynk.connected()) { Blynk.run(); }
  else { Blynk.connect(); }
  timer.run();
  server.handleClient();
}

BLYNK_CONNECTED() {
    Blynk.syncAll();
}



/* ----------------------------------- RELAY ------------------------------------- */



/* Receives values from Blynk */
BLYNK_WRITE(RELAY_VPIN)
{
  state = param.asInt();
  digitalWrite(RELAY_PIN, state);
}

/* Toggle Relay */
void toggleRelay() {
  state = !state;
  digitalWrite(RELAY_PIN, state);
  Blynk.virtualWrite(RELAY_VPIN, state);
}

/* Handle Physical Button */
void checkButton() {
  if (!digitalRead(BUTTON_PIN)) { 
      if ((millis() - lastDebounceTime) > debounceDelay) { toggleRelay(); } 
      lastDebounceTime = millis();
  }
}



/* ----------------------------------- WEBPAGE ------------------------------------- */



/* OTA Server */
void handleUpdatePage() {
  String s = UPDATE_PAGE_PART1;
  s += HOSTNAME;
  s += UPDATE_PAGE_PART2;
  server.send(200, "text/html", s);
}

/* Index page */
void handleRoot() {
  String s = INDEX_START;
  s += state ? "ON" : "OFF";
  s += INDEX_PART1;

  #ifdef DHT_SENSOR
    s += INDEX_DHT_1;
    s += temp;
    s += INDEX_DHT_2;
    s += hum;
    s += INDEX_DHT_END;
  #endif

  s += INDEX_PART2;
  s += HOSTNAME;
  s += INDEX_END;
  server.send(200, "text/html", s);
}

/* Handle web interface when toggling actuator */
void handleToggleRelay() {
  toggleRelay();
  handleRoot();
}

/* Update firmware and checks for authentication */
void updateFirmware() {
  bool authenticated = (ota_username == emptyString || ota_password == emptyString || server.authenticate(ota_username.c_str(), ota_password.c_str()));
  if(!authenticated) { return; }

  const String updateMessage = UPDATE_MESSAGE;
  const String EndUpdateMessage = END_UPDATE_MESSAGE;

  HTTPUpload& upload = server.upload();

  if (upload.status == UPLOAD_FILE_START) { 
      uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
      if (!Update.begin(maxSketchSpace)) {
        server.send(200, "text/html", updateMessage + "Error:<br/>!Update.begin(maxSketchSpace)" + EndUpdateMessage);
      }
  } else if (authenticated && upload.status == UPLOAD_FILE_WRITE) {
   //flashing firmware to ESP
    if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
      server.send(200, "text/html", updateMessage + "Error:<br/>Buffers sizes differs" + EndUpdateMessage);
    }
  } else if (authenticated && upload.status == UPLOAD_FILE_END) {
    if (Update.end(true)) { //true to set the size to the current progress
      server.send(200, "text/html", updateMessage + "Success!<br/>Rebooting..." + EndUpdateMessage);
    } else {
      server.send(200, "text/html", updateMessage + "Error!"  + EndUpdateMessage);
    }
  } 
  else if(authenticated && upload.status == UPLOAD_FILE_ABORTED) {
    Update.end();
    server.send(200, "text/html", updateMessage + "Error:<br/>Update was aborted" + EndUpdateMessage);
  }
}

void handleRestart() {
  ESP.restart();
}

/* Relay toggling without HTML page */
void handleToggleRelayCmd() {
  toggleRelay();
  server.send(200, "text/plain", "OK");
}

/* Simple temperature retrieve */
#ifdef DHT_SENSOR
  void handleTemperatureCmd() {
    String s = String(temp);
    s += "°C, ";
    s += hum;
    s += "%";
    server.send(200, "text/html; charset=utf-8", s);
  }
#endif



/* ----------------------------------- DHT SENSOR ------------------------------------- */



#ifdef DHT_SENSOR

/* Get DHT data */
void getDhtData(void)
{
  hum = dht.readHumidity();              // Read humidity (percent)
  temp = dht.readTemperature(false);     // Read temperature as Celsius
  if (isnan(hum) || isnan(temp))         // Check if any reads failed and exit early (to try again).
  {
    return;
  }

  Blynk.virtualWrite(TEMP_VPIN, temp);
  Blynk.virtualWrite(HUM_VPIN, hum);
}

/* Send DHT data to thinkspeak */
void sendDataTS(void)
{
   if (client.connect(TS_SERVER, 80)) 
   { 
     String postStr = TS_API_KEY;
     postStr += "&field1=";
     postStr += String(temp);
     postStr += "&field2=";
     postStr += String(hum);
     postStr += "\r\n\r\n";
   
     client.print("POST /update HTTP/1.1\n");
     client.print("Host: api.thingspeak.com\n");
     client.print("Connection: close\n");
     client.print("X-THINGSPEAKAPIKEY: " + TS_API_KEY + "\n");
     client.print("Content-Type: application/x-www-form-urlencoded\n");
     client.print("Content-Length: ");
     client.print(postStr.length());
     client.print("\n\n");
     client.print(postStr);
     delay(1000); 
   }
   client.stop();
}

#endif
