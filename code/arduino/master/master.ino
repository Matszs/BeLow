#include <SoftwareSerial.h>
#include "includes/BleSerial.h"

SoftwareSerial softwareSerial(2, 3);
BleSerial bleSerial(&softwareSerial, &Serial);

void setup() {
  Serial.begin(9600);
  //bleSerial.setupAsDetector();
}

void loop() {
  delay(10000);

  bleSerial.detectBeacons();
  Serial.print("TEST");


}
