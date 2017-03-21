#include <SoftwareSerial.h>
#include "BleSerial.h"

SoftwareSerial softwareSerial(2, 3);
BleSerial bleSerial(&softwareSerial, &Serial);

void onBeaconFound(iBeaconData_t beacon) {
  if(beacon.uuid == "00001338B64445208F0C720EAF059935") {
    Serial.print("Beacon found: ");
    Serial.println(beacon.major);
  }
}

void setup() {
  Serial.begin(9600);
  bleSerial.setMaster();
}

void loop() {
  bleSerial.detectBeacons(&onBeaconFound, 10000); // max time to search

  delay(3000);
}
