#include <Arduino.h>
#include <SPI.h>
#include <RH_NRF24.h>
#include "DHT.h"
#include <EEPROM.h>
#include <avr/sleep.h>

#define DHTTYPE DHT22
#define DHTPIN 2

#define DEVICE_ID = EEPROM.read(0)

#define interruptPin 3

// Singleton instance of the radio driver
RH_NRF24 nrf24;

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(interruptPin, INPUT_PULLUP);
  digitalWrite(LED_BUILTIN, HIGH);

  Serial.println("Setup");
  
  dht.begin();

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
  
  uint8_t data[4];
  data[0] = DEVICE_ID;
  data[1] = dht.readTemperature();
  data[2] = dht.readHumidity();
  data[3] = analogRead(A0);

  Serial.print("Device ID: ");
  Serial.println(data[0]);

  Serial.print("Temperature: ");
  Serial.println(data[1]);

  Serial.print("Humidity: ");
  Serial.println(data[2]);

  Serial.print("Lux: ");
  Serial.println(data[3]);

  nrf24.send(data, sizeof(data));
  nrf24.waitPacketSent();
  // Now wait for a reply
  uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);
  delay(1000);
  Going_To_Sleep(30e6);
}

void Going_To_Sleep(ms) {
  sleep_enable();                      
  attachInterrupt(0, wakeUp, LOW);     
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); 
  digitalWrite(LED_BUILTIN, LOW);      
  delay(ms);                         
  sleep_cpu();                         
  Serial.println("just woke up!");     
  digitalWrite(LED_BUILTIN, HIGH);     
}

void wakeUp() {
  Serial.println("Interrrupt Fired"); 
  sleep_disable();                    
  detachInterrupt(0);                 
}