#define IS_GATEWAY 0 // 1 == gateway | 0 == node
#include "platform.h"
#define SENSOR_TYPE (IS_GATEWAY ? GATEWAY : ALL)

#define ONE_WIRE_BUS 4 // temperature sensor
unsigned long timer = 0;

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

  switch(SENSOR_TYPE) {
    case DOOR_SENSOR:
      pinMode(0, INPUT); // door sensor
    break;
    case PIR_SENSOR:

    break;

    case TEMPERATURE_SENSOR:
      pinMode(2, INPUT); // tempature sensor
      NVIC_EnableIRQ((IRQn_Type)0x02);
    break;

    case LDR_SENSOR:
      pinMode(13, INPUT); // tempature sensor
    break;

    case ALL:
      pinMode(0, INPUT); // door sensor
      // temperature sensor is initalized on top 'ONE_WIRE_BUS'
      // TODO: Init other sensors
    break;
  }
  
  SerialUSB.begin(9600); // debugging
  Serial2.begin(9600); // BLE module
  Serial1.begin(9600); // LoRa

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

  switch(SENSOR_TYPE) {
    case DOOR_SENSOR:{
      if(digitalRead(0) == LOW) {
        ble_set_minor(00001); // 1 = doorsensor
        ble_set_major(11111);
    
        ble_start_advertising();
        delay(5000);
        ble_stop_advertising();
        delay(3000);
      }
    }
    break;
    case PIR_SENSOR:{

      
    }
    break;

    case TEMPERATURE_SENSOR: {
      if(!deviceIsSleeping) {
        sensors.requestTemperatures();
       
        ble_set_minor(00002); // 2 = temperature sensor
        ble_set_major((int)(sensors.getTempCByIndex(0) * 100));
    
        ble_start_advertising();
        delay(10000);
        ble_stop_advertising();
    
        delay(2000);
        sleepDevice();
      }
      
      sleepActions();
    }
    break;

    case LDR_SENSOR: {
      int ldrValue = analogRead(13);
      SerialUSB.println(ldrValue);
      ble_set_minor(00003); // 3 = LDR sensor
      ble_set_major(ldrValue);
      
      ble_start_advertising();
      delay(5000);
      ble_stop_advertising();
  
      delay(60000);
    }
    break;

    case GATEWAY: {
      ble_scan();
      delay(2000);
    }
    break;

    case ALL: {
      if(digitalRead(0) == LOW) {
        SerialUSB.println("> Doorsensor trigger");
        ble_set_minor(00001); // 1 = doorsensor
        ble_set_major(11111);
    
        ble_start_advertising();
        delay(5000);
        ble_stop_advertising();
        delay(2000);
        
      } else if(timer == 0 || (timer + (1000 * 60) < millis())) { // 1 minute interval
        SerialUSB.println("> temperature trigger");
        sensors.requestTemperatures();
        
        ble_set_minor(00002); // 2 = temperature sensor
        ble_set_major((int)(sensors.getTempCByIndex(0) * 100));
    
        ble_start_advertising();
        delay(5000);
        ble_stop_advertising();
        delay(2000);

        timer = millis();
      }
    }
    break;
    
  }
}

