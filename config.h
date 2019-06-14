/***************************************************
 Espegi - Sonoff (esp8266 based) control with Blynk and web interface
 Created by: regi18   (regi18.ml   |   regi18.github.io)
 Version: Version: 3.5.0
 Github: 

 CONFIGURATION FILE FOR Espegi.ino
 **************************************************/

/* Comment this out if you don't want a DHT sensor */
#define DHT_SENSOR

/* Blynk key */
char auth[] = "";


/* Thinkspeak key (only if DHT_SENSOR is uncommented) */
#ifdef DHT_SENSOR
    const char* TS_SERVER = "api.thingspeak.com";
    String TS_API_KEY = "";
#endif


/* Wi-Fi Details */
char ssid[] = "";
char wifiPassword[] = "";
char HOSTNAME[] = "ESPEGI";

String ota_username = "admin";
String ota_password = "admin";


/* ESP8266 PINS */
#define RELAY_PIN 12
#define LED_PIN 13
#define BUTTON_PIN 0
#define BUTTON_DEBOUNCE_DELAY 270       // Debounce delay (in ms) for the button

#ifdef DHT_SENSOR
    #define DHT_PIN 14
    #define DHT_TYPE DHT22
#endif


/* BLYNK VIRTUAL PINS */
#define RELAY_VPIN V0
#ifdef DHT_SENSOR
    #define TEMP_VPIN V1
    #define HUM_VPIN V2
#endif
