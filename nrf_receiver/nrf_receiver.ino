#include <SPI.h>
#include <RH_NRF24.h>
#include <Wire.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>

#define wifi_ssid "Fimo"
#define wifi_password "Fimo@!23"

#define mqtt_server "api.thingspeak.com"

#define apiKey = "BN1SUH6WGG8J9NGT"

#define MQTT_CLIENT "espsen1"
#define MQTT_USER "user"
#define MQTT_PWD "pass"

#define humidity_topic "sensor/hum"
#define temperature_topic "sensor/temp"
#define lux_topic "sensor/lux"

#define GATEWAY_ID = EEPROM.read(0)

WiFiClient client;

int gatewayID = EEPROM.read(0);

// Singleton instance of the radio driver
//RH_NRF24 nrf24;
RH_NRF24 nrf24(2, 4); // use this for NodeMCU Amica/AdaFruit Huzzah ESP8266 Feather
// RH_NRF24 nrf24(8, 7); // use this to be electrically compatible with Mirf
// RH_NRF24 nrf24(8, 10);// For Leonardo, need explicit SS pi/ RH_NRF24 nrf24(8, 7); // For RFM73 on Anarduino Mini
int LED = 5;
void setup() {
  Serial.begin(9600);
  Serial.print("Receiver Started, ID: ");
  Serial.print("Connecting to ");

  nrf24.init();
  nrf24.setChannel(3);
  nrf24.setRF(RH_NRF24::DataRate2Mbps, RH_NRF24::TransmitPower0dBm);
  pinMode(LED, OUTPUT);
}
void loop() {
  if (nrf24.available()) {
    // Should be a message for us now
    uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (nrf24.recv(buf, & len)) {
      // new message, turn on LED
      digitalWrite(LED, HIGH);
      // Send a reply
      uint8_t sdata[] = "Data Received.";
      nrf24.send(sdata, sizeof(sdata));
      nrf24.waitPacketSent();

      int deviceID = buf[0];
      int temperature = buf[1];
      int humidity = buf[2];
      int lux = buf[3];

      Serial.println("--- Data retrieved from device ---");
      Serial.print("Device ID: ");
      Serial.println(deviceID);

      Serial.print("Temperature: ");
      Serial.println(temperature);

      Serial.print("Humidity: ");
      Serial.println(humidity);

      Serial.print("Lux: ");
      Serial.println(lux);

    } else {
      // no new message, turn off LED
      digitalWrite(LED, LOW);
    }
  }
}