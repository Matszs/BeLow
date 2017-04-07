#include "BleSerial.h"

BleSerial bleSerial(&Serial2, &SerialUSB);

void onBeaconFound(iBeaconData_t beacon) {
  Serial.println(beacon.major);
}

void setup() {
  pinMode(VCC_SW, OUTPUT); // fully enable grove shield
  digitalWrite(VCC_SW, HIGH); // fully enable grove shield
  
  pinMode(A0, INPUT);
  SerialUSB.begin(9600);
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

  SerialUSB.println("vis");

  int LDRvalue = analogRead(A0);
  if(LDRvalue < 200) {
    advertise(LDRvalue);
  }

  delay(1000);
}
