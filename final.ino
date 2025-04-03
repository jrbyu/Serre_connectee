
#include <DHT.h>

#define DHTPIN 6      // Broche où est connecté le DHT22
#define DHTTYPE DHT11 // Type du capteur

DHT dht(DHTPIN, DHTTYPE);

void setup() {
    Serial.begin(9600);
    Serial.println("Initialisation du capteur DHT22...");
    dht.begin();
}


void loop() {
    delay(2000); // Pause entre les mesures

    float temperature = dht.readTemperature();
    float humidite = dht.readHumidity();
    int analogValue = analogRead(A0);


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

    Serial.print("Analog reading: ");
  Serial.print(analogValue); 
     if (analogValue < 10) {
    Serial.println(" - Dark");
  } else if (analogValue < 200) {
    Serial.println(" - Dim");
  } else if (analogValue < 500) {
    Serial.println(" - Light");
  } else if (analogValue < 800) {
    Serial.println(" - Bright");
  } else {
    Serial.println(" - Very bright");
  }
  
      Serial.println("--------------------");

}
