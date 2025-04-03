#include <Servo.h>

// Crée une instance de l'objet Servo
Servo myServo;

// Pin où le signal du servo est connecté
const int servoPin = 9;

void setup() {
  // Attache le servo à la broche définie
  myServo.attach(servoPin);
}

void loop() {
  // Déplace le servo de 0° à 180°
  for (int angle = 0; angle <= 180; angle++) {
    myServo.write(angle); // Définit l'angle du servo
    delay(0);            // Pause pour permettre au servo de bouger
  }

  // Déplace le servo de 180° à 0°
  for (int angle = 180; angle >= 0; angle--) {
    myServo.write(angle); // Définit l'angle du servo
    delay(0);            // Pause pour permettre au servo de bouger
  }
}
