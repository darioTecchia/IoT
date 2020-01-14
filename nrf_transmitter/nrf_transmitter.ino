#include <Arduino.h>

#include <SPI.h>

#include <RH_NRF24.h>

#include <DHT.h>

#include <EEPROM.h>

// Singleton instance of the radio driver
RH_NRF24 nrf24;

DHT dht(4, DHT22, 11);

int idDHT11pin = 2; //Digital pin for comunications
int idDHT11intNumber = 0; //interrupt number (must be the one that use the previus defined pin (see table above)

int deviceID = EEPROM.read(0);
int counter = 0;
int analogValue = 0;

void setup() {
  Serial.begin(9600);

  Serial.println("Setup");

  while (!Serial);

  if (!nrf24.init()) {
    Serial.println("init failed");
  }
  // Defaults after init are 2.402 GHz (channel 2), 2Mbps, 0dBm
  if (!nrf24.setChannel(3)) {
    Serial.println("setChannel failed");
  }
  if (!nrf24.setRF(RH_NRF24::DataRate2Mbps, RH_NRF24::TransmitPower0dBm)) {
    Serial.println("setRF failed");
  }

  Serial.println("Transmitter started");
}

// This wrapper is in charge of calling
// mus be defined like this for the lib work
void loop() {
  Serial.println("Sending to gateway");
  analogValue = analogRead(A0);
  uint8_t data[4];
  data[0] = deviceID;
  data[1] = dht.readTemperature();
  data[2] = dht.readHumidity();
  data[3] = analogValue;

  Serial.println("Device ID");
  Serial.println(data[0]);

  Serial.println("Temperature");
  Serial.println(data[1]);

  Serial.println("Humidity");
  Serial.println(data[2]);

  Serial.println("Humidity");
  Serial.println(data[3]);

  nrf24.send(data, sizeof(data));
  nrf24.waitPacketSent();
  // Now wait for a reply
  uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);
  delay(2000);
}