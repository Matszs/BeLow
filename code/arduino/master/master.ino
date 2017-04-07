#include "BleSerial.h"

BleSerial bleSerial(&Serial2, &SerialUSB);

void onBeaconFound(iBeaconData_t beacon) {
  if(beacon.uuid == "00001338B64445208F0C720EAF059935") {
    Serial.print("Beacon found: ");
    Serial.println(beacon.major);
  }
}

void setup() {
  pinMode(VCC_SW, OUTPUT); // fully enable grove shield
  digitalWrite(VCC_SW, HIGH); // fully enable grove shield

  SerialUSB.begin(9600);
  bleSerial.setMaster();
}

void loop() {
  bleSerial.detectBeacons(&onBeaconFound, 10000); // max time to search

  delay(3000);
  SerialUSB.println("vis");
}
