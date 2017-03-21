#include <SoftwareSerial.h>
#include "BleSerial.h"

SoftwareSerial softwareSerial(2, 3);
BleSerial bleSerial(&softwareSerial, &Serial);

void onBeaconFound(iBeaconData_t beacon) {
  Serial.println(beacon.major);
}

void setup() {
  Serial.begin(9600);
  bleSerial.setMaster();
  bleSerial.detectBeacons(&onBeaconFound, 10000); // max time to search
}

void loop() {



}
