#include <SPI.h>
#include <RH_NRF24.h>
#include <Wire.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
String apiKey = "BN1SUH6WGG8J9NGT";
const char *ssid = "Fimo";
const char *password = "Fimo@!23";
WiFiClient client;
int gatewayID = EEPROM.read(0);
const char *server = "api.thingspeak.com";
// Singleton instance of the radio driver
//RH_NRF24 nrf24;
RH_NRF24 nrf24(2, 4); // use this for NodeMCU Amica/AdaFruit Huzzah ESP8266 Feather
// RH_NRF24 nrf24(8, 7); // use this to be electrically compatible with Mirf
// RH_NRF24 nrf24(8, 10);// For Leonardo, need explicit SS pi/ RH_NRF24 nrf24(8, 7); // For RFM73 on Anarduino Mini
int LED = 5;
void setup()
{
    Serial.begin(9600);
    Serial.print("Receiver Started, ID: ");
    Serial.print("Connecting to ");

    nrf24.init();
    nrf24.setChannel(3);
    nrf24.setRF(RH_NRF24::DataRate2Mbps, RH_NRF24::TransmitPower0dBm);
    pinMode(LED, OUTPUT);
}
void loop()
{
    if (nrf24.available())
    {
        // Should be a message for us now
        uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];
        uint8_t len = sizeof(buf);
        if (nrf24.recv(buf, &len))
        {
            // new message, turn on LED
            digitalWrite(LED, HIGH);
            // Send a reply
            uint8_t sdata[] = "Data Received.";
            nrf24.send(sdata, sizeof(sdata));
            nrf24.waitPacketSent();

            int temperature = buf[1];
            int deviceID = buf[2];

            Serial.println("--- Data retrieved from device ---");
        }
        else
        {
            // no new message, turn off LED
            digitalWrite(LED, LOW);
        }
    }
}
