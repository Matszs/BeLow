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

  pinMode(0, INPUT); // door sensor
  pinMode(2, INPUT); // tempature sensor
  pinMode(13, INPUT); // tempature sensor

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


OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void loop() {
  #if (IS_GATEWAY)
    ble_scan();
    delay(2000);
  #else
    /*
     * DOOR SENSOR
     * if(digitalRead(0) == LOW) {
  
      ble_set_major(888);
  
      ble_start_advertising();
      delay(5000);
      ble_stop_advertising();
      delay(3000);

    }*/

    /*// temperature sensor
    sensors.requestTemperatures();
    ble_set_major((int)(sensors.getTempCByIndex(0) * 100));

    ble_start_advertising();
    delay(5000);
    ble_stop_advertising();

    delay(60000);*/

    int ldrValue = analogRead(13);
    SerialUSB.println(ldrValue);
    ble_set_major(ldrValue);
    
    ble_start_advertising();
    delay(5000);
    ble_stop_advertising();

    delay(60000);

    
    
  #endif
}

