#include <SPI.h>
#include <Ethernet.h>
#include <Servo.h>
#include <DHT.h>

// Définition des broches
#define DHTPIN 6
#define DHTTYPE DHT11
#define SERVO_PIN 9
#define LUM_PIN A0

DHT dht(DHTPIN, DHTTYPE);
Servo volet;

// Adresse MAC & IP statique
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 174);
EthernetServer server(80);

// Variables globales
bool voletOuvert = false;
float temperature = 0;
float humidite = 0;
int luminosite = 0;
bool modeAuto = false;

void setup() {
  Serial.begin(9600);
  Ethernet.begin(mac, ip);
  server.begin();
  dht.begin();
  volet.attach(SERVO_PIN);
  fermerVolet();

  // Affichage IP et MAC
  Serial.println(F("Serveur lancé."));
  Serial.print(F("IP : ")); Serial.println(Ethernet.localIP());
  Serial.print(F("MAC : "));
  for (byte i = 0; i < 6; i++) {
    if (mac[i] < 16) Serial.print("0");
    Serial.print(mac[i], HEX);
    if (i < 5) Serial.print(":");
  }
  Serial.println();
}

void loop() {
  // Lecture des capteurs
  temperature = dht.readTemperature();
  humidite = dht.readHumidity();
  luminosite = analogRead(LUM_PIN);

  // Vérification des lectures
  if (isnan(temperature) || isnan(humidite)) {
    Serial.println("Erreur de lecture du DHT11");
    temperature = 0;
    humidite = 0;
  }

  if (luminosite < 0 || luminosite > 1023) {
    Serial.println("Erreur de lecture de la luminosité");
    luminosite = 0;
  }

  // Contrôle automatique du volet
  if (modeAuto) {
    if (temperature >= 24.0 && !voletOuvert) ouvrirVolet();
    else if (temperature <= 21.0 && voletOuvert) fermerVolet();
  }

  EthernetClient client = server.available();
  if (client) {
    String request = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        request += c;
        if (request.endsWith("\r\n\r\n")) break;
      }
    }

    // Traitement des commandes
    if (request.indexOf("GET /ouvrir") >= 0 && !modeAuto) ouvrirVolet();
    else if (request.indexOf("GET /fermer") >= 0 && !modeAuto) fermerVolet();
    else if (request.indexOf("GET /auto") >= 0) modeAuto = true;
    else if (request.indexOf("GET /manuel") >= 0) modeAuto = false;

    // Réponse JSON
    if (request.indexOf("GET /data") >= 0) {
      client.println(F("HTTP/1.1 200 OK"));
      client.println(F("Content-Type: application/json"));
      client.println(F("Connection: close"));
      client.println();

      // Débogage
      Serial.print("Température: "); Serial.println(temperature);
      Serial.print("Humidité: "); Serial.println(humidite);
      Serial.print("Luminosité: "); Serial.println(luminosite);
      Serial.print("Volet: "); Serial.println(voletOuvert ? "OUVERT" : "FERMÉ");
      Serial.print("Mode: "); Serial.println(modeAuto ? "AUTO" : "MANUEL");

      client.print(F("{\"temperature\":"));
      client.print(temperature, 1);
      client.print(F(",\"humidite\":"));
      client.print(humidite, 1);
      client.print(F(",\"luminosite\":"));
      client.print(luminosite);
      client.print(F(",\"volet\":\""));
      client.print(voletOuvert ? F("OUVERT") : F("FERMÉ"));
      client.print(F("\",\"mode\":\""));
      client.print(modeAuto ? F("AUTO") : F("MANUEL"));
      client.println(F("\"}"));
    }
    // Page HTML
    else {
      client.println(F("HTTP/1.1 200 OK"));
      client.println(F("Content-Type: text/html"));
      client.println(F("Connection: close"));
      client.println();
      client.println(F("<!DOCTYPE html><html><head><meta charset='UTF-8'>"));
      client.println(F("<title>Serre</title><script>"));
      client.println(F("function sendCmd(c){fetch(c);}"));
      client.println(F("function maj(){fetch('/data').then(r=>r.json()).then(d=>{"));
      client.println(F("document.getElementById('temp').innerText = d.temperature + ' °C';"));
      client.println(F("document.getElementById('hum').innerText = d.humidite + ' %';"));
      client.println(F("document.getElementById('lum').innerText = d.luminosite + ' /1023';"));
      client.println(F("document.getElementById('volet').innerText = d.volet;"));
      client.println(F("document.getElementById('mode').innerText = d.mode;")); 
      client.println(F("});} setInterval(maj, 5000); window.onload=maj;"));
      client.println(F("</script></head><body><h1>Serre Connectée</h1>"));
      client.println(F("<p><b>Température :</b> <span id='temp'>...</span></p>"));
      client.println(F("<p><b>Humidité :</b> <span id='hum'>...</span></p>"));
      client.println(F("<p><b>Luminosité :</b> <span id='lum'>...</span></p>"));
      client.println(F("<p><b>Volet :</b> <span id='volet'>...</span></p>"));
      client.println(F("<p><b>Mode :</b> <span id='mode'>...</span></p>"));
      client.println(F("<p><button onclick=\"sendCmd('/ouvrir')\">Ouvrir</button>"));
      client.println(F("<button onclick=\"sendCmd('/fermer')\">Fermer</button></p>"));
      client.println(F("<p><button onclick=\"sendCmd('/auto')\">Auto</button>"));
      client.println(F("<button onclick=\"sendCmd('/manuel')\">Manuel</button></p>"));
      client.println(F("</body></html>"));
    }

    delay(1);
    client.stop();
  }
}

// Contrôle du volet
void ouvrirVolet() {
  volet.write(180);
  voletOuvert = true;
}

void fermerVolet() {
  volet.write(0);
  voletOuvert = false;
}
