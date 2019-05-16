#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include <PubSubClient.h>

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPUpdateServer.h>

#define WebServer ESP8266WebServer
#define ESPmDNS ESP8266mDNS
#define ESPHTTPUpdateServer ESP8266HTTPUpdateServer

#define MAX_TOPIC_SUBSCRIPTION_LIST_SIZE 10
#define MAX_DELAYED_EXECUTION_LIST_SIZE 10
#define CONNECTION_RETRY_DELAY 10 * 1000

void onConnectionEstablished(); // MUST be implemented in your sketch. Called once everythings is connected (Wifi, mqtt).

typedef void(*ConnectionEstablishedCallback) ();
typedef void(*MessageReceivedCallback) ( String &message);
typedef void(*DelayedExecutionCallback) ();

class MQTTClient
{
  private:
    // Wifi related
    bool mWifiConnected;
    unsigned long mLastWifiConnectionAttemptMillis;
    unsigned long mLastWifiConnectionSuccessMillis;
    char* mWifiSsid;
    char* mWifiPassword;
    WiFiClient mWifiClient;

    // MQTT related
    bool mMqttConnected;
    unsigned long mLastMqttConnectionMillis;
    char* mMqttServerIp;
    short mMqttServerPort;
    char* mMqttUsername;
    char* mMqttPassword;
    char* mMqttClientName;
    bool mMqttCleanSession;
    char* mMqttLastWillTopic;
    char* mMqttLastWillMessage;
    bool mMqttLastWillRetain;

    PubSubClient mMqttClient;

    struct TopicSubscriptionRecord {
      String topic;
      MessageReceivedCallback callback;
    };
    TopicSubscriptionRecord mTopicSubscriptionList[MAX_TOPIC_SUBSCRIPTION_LIST_SIZE];
    byte mTopicSubscriptionListSize;

    // HTTP update server related
    char* mUpdateServerAddress;
    char* mUpdateServerUsername;
    char* mUpdateServerPassword;
    WebServer* mHttpServer;
    ESPHTTPUpdateServer* mHttpUpdater;

    // Delayed execution related
    struct DelayedExecutionRecord {
      unsigned long targetMillis;
      DelayedExecutionCallback callback;
    };
    DelayedExecutionRecord mDelayedExecutionList[MAX_DELAYED_EXECUTION_LIST_SIZE];
    byte mDelayedExecutionListSize = 0;

    // General behaviour related
    ConnectionEstablishedCallback mConnectionEstablishedCallback;
    bool mEnableSerialLogs;
    bool mShowLegacyructorWarning;


  public:
    // Wifi + MQTT with no MQTT authentification
    MQTTClient(
      char* wifiSsid,
      char* wifiPassword,
      char* mqttServerIp,
      char* mqttClientName = "ESP8266",
      short mqttServerPort = 1883);

    // Wifi + MQTT with MQTT authentification
    MQTTClient(
      char* wifiSsid,
      char* wifiPassword,
      char* mqttServerIp,
      char* mqttUsername,
      char* mqttPassword,
      char* mqttClientName = "ESP8266",
      short mqttServerPort = 1883);

    // Only MQTT handling (no wifi), with MQTT authentification
    MQTTClient(
      char* mqttServerIp,
      short mqttServerPort,
      char* mqttUsername,
      char* mqttPassword,
      char* mqttClientName = "ESP8266");

    // Only MQTT handling without MQTT authentification
    MQTTClient(
      char* mqttServerIp,
      short mqttServerPort,
      char* mqttClientName = "ESP8266");

    // Legacy ructor for version 1.3 - WILL BE DELETED SOON OR LATER
    MQTTClient(
      char* wifiSsid,
      char* wifiPassword,
      ConnectionEstablishedCallback connectionEstablishedCallback,
      char* mqttServerIp,
      short mqttServerPort = 1883,
      char* mqttUsername = NULL,
      char* mqttPassword = NULL,
      char* mqttClientName = "ESP8266",
      bool enableWebUpdater = true,
      bool enableSerialLogs = true);

    // Legacy ructor for version <= 1.2 - WILL BE DELETED SOON OR LATER
    MQTTClient(
      char* wifiSsid,
      char* wifiPassword,
      char* mqttServerIp,
      short mqttServerPort,
      char* mqttUsername,
      char* mqttPassword,
      char* mqttClientName,
      ConnectionEstablishedCallback connectionEstablishedCallback,
      bool enableWebUpdater = true,
      bool enableSerialLogs = true);

    ~MQTTClient();

    // Optionnal functionnalities
    void enableDebuggingMessages( bool enabled = true); // Allow to display usefull debugging messages. Can be set to false to disable them during program execution
    void enableHTTPWebUpdater( char* username,  char* password,  char* address = "/"); // Activate the web updater, must be set before the first loop() call.
    void enableHTTPWebUpdater( char* address = "/"); // Will set user and password equal to mMqttUsername and mMqttPassword
    void enableMQTTPersistence(); // Tell the broker to establish a persistant connection. Disabled by default. Must be called before the fisrt loop() execution
    void enableLastWillMessage( char* topic,  char* message,  bool retain = false); // Must be set before the first loop() call.

    // Main loop, to call at each sketch loop()
    void loop();

    // MQTT related
    void publish(const String &topic, const String &payload, bool retain = false);
    void subscribe(const String &topic, MessageReceivedCallback messageReceivedCallback);
    void unsubscribe(const String &topic);   //Unsubscribes from the topic, if it exists, and removes it from the CallbackList.

    void setUsername(char* username);
    void setPassword(char* password);
    void setIPAddress(char* serverIP);
    void setClientName(char* clientName);
    void setPort(short MQTTPort);

    // Other
    void executeDelayed( long delay, DelayedExecutionCallback callback);

    inline bool isConnected()  {
      return mWifiConnected && mMqttConnected;
    };
    inline void setOnConnectionEstablishedCallback(ConnectionEstablishedCallback callback) {
      mConnectionEstablishedCallback = callback;
    }; // Default to onConnectionEstablished, you might want to override this for special cases like two MQTT connections in the same sketch

  private:
    void connectToWifi();
    void connectToMqttBroker();
    void mqttMessageReceivedCallback(char* topic, byte* payload, unsigned int length);
};

#endif
