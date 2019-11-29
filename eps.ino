#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

#define wifi_ssid "WIFI"
#define wifi_password "pass"

#define mqtt_server "1.1.1.1"

#define MQTT_CLIENT "espsen1"
#define MQTT_USER "user"
#define MQTT_PWD "pass"

#define humidity_topic "sensor/hum/esp1"
#define temperature_topic "sensor/temp/esp1"
#define motion_topic "sensor/motion/esp1"
#define lux_topic "sensor/lux/esp1"
#define press_topic "sensor/press/esp1"

#define DHTTYPE DHT22
#define DHTPIN  12     //D6
#define SENSR_PIN 13  //D7
#define LED_PIN 14    //D5

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE, 11);
Adafruit_BMP280 bme; // I2C

void setup() {
//  Serial.begin(115200);
  pinMode(SENSR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(A0, INPUT);
  if (!bme.begin()) {  
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    //while (1);
  }
  dht.begin();
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void setup_wifi() {
  delay(10);
  digitalWrite(LED_PIN, HIGH);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);

  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  digitalWrite(LED_PIN, LOW);
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    if (client.connect(MQTT_CLIENT, MQTT_USER, MQTT_PWD)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

bool checkBound(float newValue, float prevValue, float maxDiff) {
  return !isnan(newValue) &&
         (newValue < prevValue - maxDiff || newValue > prevValue + maxDiff);
}

long lastMsg = 0;
long lastMotion = 0;
bool prevstate = true;
float temp = 0;
float hum = 0;
int diff = 0.09;
int lux = 0;
float pressure = 0;
int motionPersist = 30000;

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  byte sensor = digitalRead(SENSR_PIN);
  delay(500);
  if(sensor == HIGH){
    if( (now - lastMotion) > motionPersist){
      digitalWrite(LED_PIN, HIGH);
      Serial.println("Motion");
      client.publish(motion_topic, "1", true);
      prevstate = (sensor == HIGH);
    }
//    Serial.println("Last motion updated");
    lastMotion = now;
    digitalWrite(LED_PIN, LOW);
    delay(50);
    digitalWrite(LED_PIN, HIGH);
  }
  if(sensor == LOW){
      if( (now - lastMotion) > motionPersist && (sensor == HIGH) != prevstate){
        digitalWrite(LED_PIN, LOW);
        Serial.println("No motion");
        client.publish(motion_topic, "0", true);
        prevstate = (sensor == HIGH);
      }
  }
  
  if (now - lastMsg > 60000) {
    lastMsg = now;

    float newPressure = ((int)(bme.readPressure()*0.00750062*100))/100.0;
    int newLux = analogRead(A0);
    float newTemp = dht.readTemperature();
    float newHum = dht.readHumidity();
    if(checkBound(newPressure, pressure, 500)){
      pressure = newPressure;
      client.publish(press_topic, String(pressure).c_str(), true);
    }
    if(checkBound(newLux, lux, 20)){
      lux = newLux;
      Serial.print("New lux:");
      Serial.println(String(lux).c_str());
      client.publish(lux_topic, String(lux).c_str(), true);
    }
    if(newTemp<60 && newHum < 100){
      
      if (checkBound(newTemp, temp, diff)) {
        temp = newTemp;
        Serial.print("New temperature:");
        Serial.println(String(temp).c_str());
        client.publish(temperature_topic, String(temp).c_str(), true);
      }
  
      if (checkBound(newHum, hum, diff)) {
        hum = newHum;
        Serial.print("New humidity:");
        Serial.println(String(hum).c_str());
        client.publish(humidity_topic, String(hum).c_str(), true);
      }
    }
  }
}