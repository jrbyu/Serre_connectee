#include <SPI.h>
#include <Ethernet.h>
#include <Servo.h>
#include <DHT.h>

#define DHTPIN 6
#define DHTTYPE DHT11
#define SERVO_PIN 9
#define LUM_PIN A0

DHT dht(DHTPIN, DHTTYPE);
Servo volet;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 174);

EthernetServer server(80);

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
}

void loop() {
  // Lecture des capteurs
  temperature = dht.readTemperature();
  humidite = dht.readHumidity();
  luminosite = analogRead(LUM_PIN);

  // Gestion du mode automatique
  if (modeAuto) {
    if (temperature >= 24.0 && !voletOuvert) {
      ouvrirVolet();
    } else if (temperature <= 21.0 && voletOuvert) {
      fermerVolet();
    }
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

    // Gestion des actions
    if (request.indexOf("GET /ouvrir") >= 0) { if (!modeAuto) ouvrirVolet(); }
    else if (request.indexOf("GET /fermer") >= 0) { if (!modeAuto) fermerVolet(); }
    else if (request.indexOf("GET /auto") >= 0) { modeAuto = true; }
    else if (request.indexOf("GET /manuel") >= 0) { modeAuto = false; }

    if (request.indexOf("GET /data") >= 0) {
      // Envoi JSON
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: application/json");
      client.println("Connection: close");
      client.println();
      client.print("{\"temperature\":");
      client.print(temperature);
      client.print(",\"humidite\":");
      client.print(humidite);
      client.print(",\"luminosite\":");
      client.print(luminosite);
      client.print(",\"volet\":\"");
      client.print(voletOuvert ? "OUVERT" : "FERMÉ");
      client.print("\",\"mode\":\"");
      client.print(modeAuto ? "AUTO" : "MANUEL");
      client.println("\"}");
    } else {
      // Page HTML
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/html");
      client.println("Connection: close");
      client.println();
      client.println("<!DOCTYPE html><html><head><meta charset='UTF-8'>");
      client.println("<title>Serre Connectée</title>");
      client.println("<script>");
      client.println("function sendCmd(cmd){fetch(cmd);}");
      client.println("function maj(){");
      client.println("fetch('/data').then(r=>r.json()).then(data=>{");
      client.println("document.getElementById('temp').innerText = data.temperature + ' °C';");
      client.println("document.getElementById('hum').innerText = data.humidite + ' %';");
      client.println("document.getElementById('lum').innerText = data.luminosite + ' /1023';");
      client.println("document.getElementById('volet').innerText = data.volet;");
      client.println("document.getElementById('mode').innerText = data.mode;");
      client.println("});}");
      client.println("setInterval(maj, 5000); window.onload=maj;");
      client.println("</script></head>");
      client.println("<body style='font-family:sans-serif'>");
      client.println("<h1>Serre Connectée</h1>");
      client.println("<p><b>Température :</b> <span id='temp'>...</span></p>");
      client.println("<p><b>Humidité :</b> <span id='hum'>...</span></p>");
      client.println("<p><b>Luminosité :</b> <span id='lum'>...</span></p>");
      client.println("<p><b>Volet :</b> <span id='volet'>...</span></p>");
      client.println("<p><b>Mode :</b> <span id='mode'>...</span></p>");
      client.println("<p>");
      client.println("<button onclick=\"sendCmd('/ouvrir')\">Ouvrir</button>");
      client.println("<button onclick=\"sendCmd('/fermer')\">Fermer</button>");
      client.println("</p><p>");
      client.println("<button onclick=\"sendCmd('/auto')\">Mode Auto</button>");
      client.println("<button onclick=\"sendCmd('/manuel')\">Mode Manuel</button>");
      client.println("</p></body></html>");
    }

    delay(1);
    client.stop();
  }
}

void ouvrirVolet() {
  volet.write(180);
  voletOuvert = true;
}

void fermerVolet() {
  volet.write(0);
  voletOuvert = false;
}
