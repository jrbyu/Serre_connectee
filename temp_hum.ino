#include <DHT.h>

#define DHTPIN 6      
#define DHTTYPE DHT11 

DHT dht(DHTPIN, DHTTYPE);

void setup() {
    Serial.begin(9600);
    Serial.println("Initialisation du capteur DHT22...");
    dht.begin();
}

void loop() {
    delay(2000);

    float temperature = dht.readTemperature();
    float humidite = dht.readHumidity();

    if (isnan(temperature) || isnan(humidite)) {
        Serial.println("Erreur de lecture du DHT22 !");
        return;
    }

    Serial.print("Température: ");
    Serial.print(temperature);
    Serial.println(" °C");

    Serial.print("Humidité: ");
    Serial.print(humidite);
    Serial.println(" %");

    Serial.println("--------------------");
}
