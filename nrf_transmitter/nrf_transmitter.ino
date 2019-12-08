#include <Arduino.h>
#include <SPI.h>
#include <RH_NRF24.h>
#include <EEPROM.h>

// Singleton instance of the radio driver
RH_NRF24 nrf24;

int deviceID = EEPROM.read(0);
int counter = 0;

void setup()
{
    Serial.begin(9600);

    while (!Serial)
        ;
    if (!nrf24.init())
    {
        Serial.println("init failed");
    }
    // Defaults after init are 2.402 GHz (channel 2), 2Mbps, 0dBm
    if (!nrf24.setChannel(3))
    {
        Serial.println("setChannel failed");
    }
    if (!nrf24.setRF(RH_NRF24::DataRate2Mbps, RH_NRF24::TransmitPower0dBm))
    {
        Serial.println("setRF failed");
    }

    Serial.println("Transmitter started");
}
// This wrapper is in charge of calling
// mus be defined like this for the lib work
void loop()
{

    Serial.println("Sending to gateway");
    uint8_t data[4];
    counter = counter + 1;
    data[0]=counter;
    nrf24.send(data, sizeof(data));
    nrf24.waitPacketSent();
    // Now wait for a reply
    uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (nrf24.waitAvailableTimeout(1000))
    {
        // Should be a reply message for us now
        if (nrf24.recv(buf, &len))
        {
            Serial.print("got reply: ");
            Serial.println((char *)buf);
        }
        else
        {
            Serial.println("recv failed");
        }
    }
    else
    {
        Serial.println("No reply.");
    }
    delay(2000);
}
