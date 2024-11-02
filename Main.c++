#include "WiFiConnection.h"
#include "FirebaseConnect.h"
#include "Action.h"


#include <DHT.h>
#define DHTPIN 14
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);


float humidity;
float temperature;

void setup() {
  Serial.begin(115200);
  dht.begin();
  setupWiFi();
  setupFirebase();
  setupPins();  

}

void loop() {
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  uploadSensorData(temperature, humidity);
  checkAction();
}
