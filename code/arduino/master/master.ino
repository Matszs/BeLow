#define IS_GATEWAY 0 // 1 == gateway | 0 == node
#include "platform.h"



#if (IS_GATEWAY)
  void onBeaconFound(iBeaconData_t beacon) {
    if(beacon.uuid.startsWith("00001338")) {
      SerialUSB.print("Beacon found: ");
      SerialUSB.println(beacon.major);
      myLora.tx(beacon.uuid + "|" + String(beacon.major));
    }
  }
#endif

void setup() {
  pinMode(VCC_SW, OUTPUT); // fully enable grove shield
  digitalWrite(VCC_SW, HIGH); // fully enable grove shield
  digitalWrite(BEE_VCC, HIGH); // beeeee

  pinMode(0, INPUT);

  SerialUSB.begin(9600);
  Serial2.begin(9600);
  Serial1.begin(9600);

  randomSeed(analogRead(0));
  //while(!SerialUSB); // DEBUG: WAIT FOR SERIAL CONNECTION

  ble_start();
  #if (IS_GATEWAY)
    lora_init();
    ble_set_master();
  #else
    ble_set_slave();
  #endif

  delay(1000);
}

void loop() {
  #if (IS_GATEWAY)
    ble_scan();
    delay(2000);
  #else
    if(digitalRead(0) == LOW) {
  
      ble_set_major(888);
  
      ble_start_advertising();
      delay(5000);
      ble_stop_advertising();
      delay(3000);

    }
  #endif
}

