#include "MQTTClient.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include "FS.h"
#include "SH1106Wire.h"
#include "SSD1306Wire.h"
#include "OLEDDisplayUi.h"

#define VERSION "0.3a"
#define HOSTNAME "Weather Node"
#define CONFIG "/conf.txt"

boolean IS_METRIC = true;

const int WEBSERVER_PORT = 80;
char* www_username = "admin";
char* www_password = "password";
char* MQTTIPADDRESS;
char* MQTTUSERNAME;
char* MQTTPASSWORD;
char* MQTTCLIENTNAME;
char* MQTTPORT;

String OTA_Password = "password";

const int buttonPin = D3;
int externalLight = LED_BUILTIN;
long intervalDisplay = 10000;

long previousMillisDisplay;

// Display Settings
const int I2C_DISPLAY_ADDRESS = 0x3c; // I2C Address of your Display (usually 0x3c or 0x3d)
const int SDA_PIN = D2;
const int SCL_PIN = D5;
const boolean INVERT_DISPLAY = true; // true = pins at top | false = pins at the bottom
//#define DISPLAY_SH1106       // Uncomment this line to use the SH1106 display -- SSD1306 is used by default and is most common

// Initialize the oled display for I2C_DISPLAY_ADDRESS
// SDA_PIN and SCL_PIN
#if defined(DISPLAY_SH1106)
SH1106Wire display(I2C_DISPLAY_ADDRESS, SDA_PIN, SCL_PIN);
#else
SSD1306Wire display(I2C_DISPLAY_ADDRESS, SDA_PIN, SCL_PIN); // this is the default
#endif

OLEDDisplayUi   ui( &display );

// Set the number of Frames supported
const int numberOfFrames = 3;
FrameCallback frames[numberOfFrames];
FrameCallback clockFrame[2];

ESP8266WebServer server(WEBSERVER_PORT);

void handleSystemReset();
void handleWifiReset();
int8_t getWifiQuality();
void readSettings();
void writeSettings();
void handleUpdateConfig();
void handleNotFound();
void handleRoot();
void handleConfigure();
void handleConfigureNoPassword();

MQTTClient client(
  "",             // MQTT Broker server ip
  1883,           // The MQTT port, default to 1883. this line can be omitted
  "user",         // MQTT Username
  "password",     // MQTT Password
  "test"          // Client name that uniquely identify your device
);

void setup() {
  Serial.begin(115200);
  SPIFFS.begin();
  delay(10);

  Serial.println();
  pinMode(externalLight, OUTPUT);
  pinMode(buttonPin, INPUT);

  readSettings();

  //initialize display
  display.init();
  if (INVERT_DISPLAY) {
    display.flipScreenVertically(); // connections at top of OLED display
  }

  display.clear();
  display.display();

  WiFiManager wifiManager;

  String hostname(HOSTNAME);
  hostname += String(ESP.getChipId(), HEX);
  if (!wifiManager.autoConnect((const char *)hostname.c_str())) {// new addition
    delay(3000);
    WiFi.disconnect(true);
    ESP.reset();
    delay(5000);
  }

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.setHostname((const char *)hostname.c_str());
  if (OTA_Password != "") {
    ArduinoOTA.setPassword(((const char *)OTA_Password.c_str()));
  }
  ArduinoOTA.begin();

  Serial.println("WEBSERVER_ENABLED");
  server.on("/Home", HTTP_GET, handleRoot);
  server.on("/Configure", handleConfigure);
  server.on("/updateConfig", handleUpdateConfigure);
  server.on("/FactoryReset", handleSystemReset);
  server.on("/WifiReset", handleWifiReset);
  server.onNotFound(handleRoot);
  server.begin();
  Serial.println("Server Started");
  String webAddress = "http://" + WiFi.localIP().toString() + ":" + String(WEBSERVER_PORT) + "/";
  Serial.println("Use this URL : " + webAddress);

  // Optionnal functionnalities of MQTTClient :
  client.enableDebuggingMessages(); // Enable debugging messages sent to serial output
  client.enableHTTPWebUpdater(); // Enable the web updater. User and password default to values of MQTTUsername and MQTTPassword. These can be overrited with enableHTTPWebUpdater("user", "password").
  client.enableLastWillMessage("TestClient/lastwill", "I am going offline");  // You can activate the retain flag by setting the third parameter to true
}

void onConnectionEstablished()
{
  // Subscribe to "temperature" and display received message to Serial
  client.subscribe("temperature", [](String & payload) {
    Serial.println(payload);
  });

  // Subscribe to "humidity" and display received message to Serial
  client.subscribe("humidity", [](String & payload) {
    Serial.println(payload);
  });

  // Subscribe to "pressure" and display received message to Serial
  client.subscribe("pressure", [](String & payload) {
    Serial.println(payload);
  });

  // Subscribe to "lightIntensity" and display received message to Serial
  client.subscribe("lightIntensity", [](String & payload) {
    Serial.println(payload);
  });

  client.publish("temperature", "test");
  client.publish("humidity", "test");
  client.publish("pressure", "test");
  client.publish("lightIntensity", "test");
}

void loop() {
  server.handleClient();
  ArduinoOTA.handle();
  client.loop();

  delay(1);

  unsigned long currentMillis = millis();
  if (digitalRead(buttonPin) == HIGH) {
    previousMillisDisplay = currentMillis;
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.setFont(ArialMT_Plain_10);
    display.drawString(64, 10, "Web Interface On");
    display.drawString(64, 20, "You May Connect to IP");
    display.setFont(ArialMT_Plain_16);
    display.drawString(64, 30, WiFi.localIP().toString());
    display.drawString(64, 46, "Port: " + String(WEBSERVER_PORT));
    display.display();
  }

  if (currentMillis - previousMillisDisplay > intervalDisplay) {
    display.clear();
  }
}

void handleSystemReset() {
  if (!server.authenticate(www_username, www_password)) {
    return server.requestAuthentication();
  }
  Serial.println("Reset System Configuration");
  if (SPIFFS.remove(CONFIG)) {
    handleRoot();
    ESP.restart();
  }
}

void handleWifiReset() {
  if (!server.authenticate(www_username, www_password)) {
    return server.requestAuthentication();
  }
  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  handleRoot();
  WiFiManager wifiManager;
  wifiManager.resetSettings();
  ESP.restart();
}

// converts the dBm to a range between 0 and 100%
int8_t getWifiQuality() {
  int32_t dbm = WiFi.RSSI();
  if (dbm <= -100) {
    return 0;
  } else if (dbm >= -50) {
    return 100;
  } else {
    return 2 * (dbm + 100);
  }
}

void writeSettings() {
  // Save decoded message to SPIFFS file for playback on power up.
  File f = SPIFFS.open(CONFIG, "w");
  if (!f) {
    Serial.println("File open failed!");
  } else {
    Serial.println("Saving settings now...");
    f.println("www_username=" + String(www_username));
    f.println("www_password=" + String(www_password));
    f.println("OTA_Password=" + String(OTA_Password));
    f.println("MQTT_ip=" + String(MQTT_ip));
    f.println("MQTT_username=" + String(MQTT_username));
    f.println("MQTT_password=" + String(MQTT_password));
    f.println("MQTT_clientname=" + String(MQTT_clientname));
    f.println("MQTT_port=" + String(MQTT_port));
  }
  f.close();
  readSettings();
}

void handleUpdateConfigure() {
  if (!server.authenticate(www_username, www_password)) {
    return server.requestAuthentication();
  }

  String temp = server.arg("userid");
  temp.toCharArray(www_username, sizeof(temp));
  temp = server.arg("stationpassword");
  temp.toCharArray(www_password, sizeof(temp));
  OTA_Password = server.arg("otapassword");
  WeatherApiKey = server.arg("openWeatherMapApiKey");
  CityID = server.arg("city1").toInt();
  IS_METRIC = server.hasArg("metric");

  writeSettings();
  handleConfigureNoPassword();
}

void readSettings() {
  if (SPIFFS.exists(CONFIG) == false) {
    Serial.println("Settings File does not yet exists.");
    writeSettings();
    return;
  }
  File fr = SPIFFS.open(CONFIG, "r");
  String line;
  while (fr.available()) {
    line = fr.readStringUntil('\n');

    if (line.indexOf("www_username=") >= 0) {
      String temp = line.substring(line.lastIndexOf("www_username=") + 13);
      temp.trim();
      temp.toCharArray(www_username, sizeof(temp));
      Serial.println("www_username=" + String(www_username));
    }
    if (line.indexOf("www_password=") >= 0) {
      String temp = line.substring(line.lastIndexOf("www_password=") + 13);
      temp.trim();
      temp.toCharArray(www_password, sizeof(temp));
      Serial.println("www_password=" + String(www_password));
    }
    if (line.indexOf("OTA_Password=") >= 0) {
      String temp = line.substring(line.lastIndexOf("OTA_Password=") + 13);
      temp.trim();
      temp.toCharArray(OTA_Password, sizeof(temp));
      Serial.println("OTA_Password=" + String(OTA_Password));
    }
    if (line.indexOf("MQTT_ip=") >= 0) {
      MQTTIPADDRESS = line.substring(line.lastIndexOf("MQTT_ip=") + 8);
      Serial.println("MQTT_ip=" + MQTTIPADDRESS);
    }
    if (line.indexOf("MQTT_username=") >= 0) {
      MQTTUSERNAME = line.substring(line.lastIndexOf("MQTT_username=") + 14);
      Serial.println("MQTT_username=" + MQTTUSERNAME);
    }
    if (line.indexOf("MQTT_password=") >= 0) {
      MQTTPASSWORD = line.substring(line.lastIndexOf("MQTT_password=") + 14);
      Serial.println("MQTT_password=" + MQTTPASSWORD);
    }
    if (line.indexOf("MQTT_clientname=") >= 0) {
      MQTTCLIENTNAME = line.substring(line.lastIndexOf("MQTT_clientname=") + 16);
      Serial.println("MQTT_clientname=" + MQTTCLIENTNAME);
    }
    if (line.indexOf("MQTT_port=") >= 0) {
      MQTTPORT = line.substring(line.lastIndexOf("MQTT_port=") + 10);
      Serial.println("MQTT_port=" + MQTTPORT);
    }
  }
  fr.close();

  setIPAddress(MQTTIPADDRESS);
  setUsername(MQTTUSERNAME);
  setPassword(MQTTPASSWORD);
  setClientName(MQTTCLIENTNAME);
  setPort(MQTTPORT);
}

void handleNotFound() {
  server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URL in the request
}

void handleRoot() {
  String form = parseHomePage();
  server.send(200, "text/html", form);  // Home webpage for the cloud
}

void handleConfigure() {
  if (!server.authenticate(www_username, www_password)) {
    return server.requestAuthentication();
  }
  String form = parseConfigurePage();
  form.replace("%USERID%", www_username);
  form.replace("%STATIONPASSWORD%", www_password);
  form.replace("%OTAPASSWORD%", OTA_Password);
  form.replace("%OTAPASSWORD%", OTA_Password);
  form.replace("%OTAPASSWORD%", OTA_Password);
  form.replace("%OTAPASSWORD%", OTA_Password);
  form.replace("%OTAPASSWORD%", OTA_Password);
  form.replace("%OTAPASSWORD%", OTA_Password);

  server.send(200, "text/html", form);  // Configure portal for the cloud
}


void handleConfigureNoPassword() {
  String form = parseConfigurePage();
  form.replace("%USERID%", www_username);
  form.replace("%STATIONPASSWORD%", www_password);
  form.replace("%OTAPASSWORD%", OTA_Password);

  server.send(200, "text/html", form);  // Configure portal for the cloud
}
