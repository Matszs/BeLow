#include <SoftwareSerial.h>
#include "BleSerial.h"

SoftwareSerial softwareSerial(2, 3);
BleSerial bleSerial(&softwareSerial, &Serial);

void onBeaconFound(iBeaconData_t beacon) {
  Serial.println(beacon.major);
}

void setup() {
  pinMode(A0, INPUT);
  Serial.begin(9600);
  // SLAVE
  bleSerial.setSlave();
  // MASTRER
  //bleSerial.setMaster();
  //bleSerial.detectBeacons(&onBeaconFound, 10000); // max time to search
}

void advertise(int major) {
  bleSerial.setMajor(major);
  bleSerial.startAdvertising();
  delay(5000);
  bleSerial.stopAdvertising();
}

void loop() {

  int LDRvalue = analogRead(A0);
  if(LDRvalue < 200) {
    advertise(LDRvalue);
  }

  delay(1000);
}
