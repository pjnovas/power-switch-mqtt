#include "config.h"

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Settings
const char *ssid = WIFI_SSID;
const char *password = WIFI_PASSWORD;
const char *clientId = WIFI_HOSTNAME;

const char *mqtt_server = MQTT_HOST;
const unsigned int mqtt_port = MQTT_PORT;

const char *mqtt_user = MQTT_USER;
const char *mqtt_pass = MQTT_PASS;
const char *mqtt_topic_command = MQTT_TOPIC_COMMAND;
const char *mqtt_topic_state = MQTT_TOPIC_STATE;

// Instances
WiFiClient espClient;
PubSubClient client(espClient);

// State
bool powerState = false;

void Log(String message)
{
#ifdef DEBUG
  Serial.println(message);
#endif
}

void onMessageReceive(char *topic, byte *payload, unsigned int length)
{
#ifdef DEBUG
  Serial.print("TOPIC: ");
  Serial.print(topic);
  Serial.print(" | VALUE: ");
  Serial.println((char *)payload);
#endif

  if (!strcmp(topic, mqtt_topic_command))
  {
    if (!strncmp((char *)payload, "ON", length))
    {
      digitalWrite(SWITCH_PIN, HIGH);
      powerState = true;
      client.publish(mqtt_topic_state, "ON", true);
      Serial.println("ON");
    }
    else if (!strncmp((char *)payload, "OFF", length))
    {
      digitalWrite(SWITCH_PIN, LOW);
      powerState = false;
      client.publish(mqtt_topic_state, "OFF", true);
      Serial.println("OFF");
    }
  }
}

void setup()
{
#ifdef DEBUG
  Serial.begin(115200);
  delay(10);
#endif
  Log("STARTED");

  pinMode(SWITCH_PIN, OUTPUT);

  WiFi.mode(WIFI_STA);
  WiFi.hostname(WIFI_HOSTNAME);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    Log("WIFI;TRY");

    if (WiFi.status() == WL_CONNECT_FAILED)
    {
      Log("WIFI;OFF");
      Log(String(WiFi.status()));
      return;
    }

    delay(1000);
  }

  Log("WIFI;ON");

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(onMessageReceive);
}

void mqttConnect()
{
  Log("MQTT;TRY");

  String willTopic = String(clientId) + "/status";
  const char *willTopic_ = willTopic.c_str();

  if (client.connect(clientId, mqtt_user, mqtt_pass, willTopic_, 0, true, "offline", true))
  {
    Log("MQTT;ON");
    client.publish(willTopic_, "online", true);

    if (client.subscribe(mqtt_topic_command))
    {
      Log("MQTT;SUB");
    }

    return;
  }

  Log("MQTT;OFF");
}

void loop()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Log("WIFI;OFF");
    return;
  }

  if (!client.connected())
  {
    mqttConnect();
  }

  client.loop();
}
