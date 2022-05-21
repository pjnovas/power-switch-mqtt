# power-switch-mqtt

simple MQTT switch using an ESP-01s relay board

## Configuration

A Platform.io build > rename `config.example` into `config.h` at `./include` dir with your configurations and upload the code into your ESP

* `[WIFI_HOSTNAME]/status`: topic of will message with `offline` payload and publishes `online` as soon as connected.
* `MQTT_TOPIC_COMMAND`: topic to subscribe for switch `ON` / `OFF`.
* `MQTT_TOPIC_STATE`: topic to publish when a digitalWrite has happened with the same message `ON` / `OFF`.
  
![pic.jpg](pic.jpg)
