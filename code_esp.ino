#include <SoftwareSerial.h>
SoftwareSerial swSer(14, 12);//Define hardware connections
String msg = " ";
void setup() {
  Serial.begin(115200);   //Initialize hardware serial with baudrate of 115200
  swSer.begin(9600);    //Initialize software serial with baudrate of 115200

  Serial.println("\nSoftware serial test started");
}
void loop() {
  while (swSer.available() > 0) {  //wait for data at software serial
     delay(10);
     if (swSer.available() > 0) {
      char c = swSer.read();
      msg += c;
     }
     Serial.write(swSer.read()); //Send data recived from software serial to hardware serial    
  }
  Serial.println (msg);
  msg = "";
  delay(1000);
  swSer.flush();
 }
