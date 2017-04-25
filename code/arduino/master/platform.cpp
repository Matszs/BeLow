#include "platform.h"

FlashStorage(settings, BLEnode);





// LORA SHIT

#if (IS_GATEWAY)
  void lora_init() {
    myLora.autobaud();
    SerialUSB.println("DevEUI? ");
    SerialUSB.print(F("> "));
    SerialUSB.println(myLora.hweui());
    SerialUSB.println("Version?");
    SerialUSB.print(F("> "));
    SerialUSB.println(myLora.sysver());
    SerialUSB.println(F("--------------------------------"));
  
    SerialUSB.println(F("Connecting to KPN"));
    bool join_result = false;
  
    join_result = myLora.initABP(DevID, NwkSKey, AppSkey);
  
    while(!join_result) {
      SerialUSB.println("\u2A2F Unable to join. Are your keys correct, and do you have KPN coverage?");
      delay(30000); //delay 30s before retry
      join_result = myLora.init();
    }
  
    SerialUSB.println("\u2713 Successfully joined KPN");
  }
#endif

// BLE SHIT

void ble_start() {
  SerialUSB.print("Waiting for BLE module...");
  while(!Serial2);
  SerialUSB.println(" done.");
}

void ble_set_master() {
  SerialUSB.print("Setting device as master...");
  ble_wait_till_active();

  SerialUSB.println(ble_set_conf("IMME1"));
  SerialUSB.println(ble_set_conf("ROLE1")); // set to Central
  SerialUSB.println(ble_set_conf("SHOW1"));
  SerialUSB.println(ble_set_conf("RESET"));

  SerialUSB.println(" done.");
}

void ble_set_slave() {
  SerialUSB.print("Setting device as slave...");
  ble_wait_till_active();
  char IBE[9];
  getBleId(IBE);
  String UUID = "IBE3"+ String(IBE);
  
  SerialUSB.println(ble_set_conf("ROLE0"));
  SerialUSB.println(ble_set_conf("IBE000001338"));
  SerialUSB.println(ble_set_conf(UUID));
  SerialUSB.println(ble_set_conf("MARJ0x1337"));
  SerialUSB.println(ble_set_conf("MINO0x1337"));
  SerialUSB.println(ble_set_conf("ADVI5"));
  SerialUSB.println(ble_set_conf("NAMEBeLowBeacon"));
  SerialUSB.println(ble_set_conf("ADTY3"));
  SerialUSB.println(ble_set_conf("IBEA1"));
  SerialUSB.println(ble_set_conf("DELO2"));
  SerialUSB.println(ble_set_conf("PWRM1"));
  SerialUSB.println(ble_set_conf("ROLE1"));
  SerialUSB.println(ble_set_conf("RESET"));

  delay(1000);

  ble_wait_till_active();

  SerialUSB.println(" done.");
}

void ble_scan() {
  ble_wait_till_active();

  // Start finding nodes
  String response = ble_get_conf("DISI");
  if(response.indexOf("OK+DISIS") >= 0) {
    SerialUSB.println("Searching for nodes...");

    int maxTimeToSearch = 10000;
    String data = "";
    String uuidHex = "";
    bool timeout = false;
    bool hashtag = false;
    uint32_t startMillis_BLE_total = millis();

    //int16_t freeBytes = getFreeRAM() - MIN_RAM;
    //data.reserve(freeBytes);


    while(data.indexOf("OK+DISCE") < 0 && !timeout/* && freeBytes > 0*/) {
      if (Serial2.available() > 0) {
        data.concat(String(char(Serial2.read())));
        //freeBytes--;
      }

      if ((millis() - startMillis_BLE_total) >= maxTimeToSearch) {
        timeout = true;
        SerialUSB.print("*");
        hashtag = true;
      }
    }

    if(hashtag)
      SerialUSB.println();

    uint16_t j = 0;
    byte deviceCounter;
    for(deviceCounter = 0; data.indexOf("OK+DISC:", j) >= 0; deviceCounter++) {
      j = data.indexOf("OK+DISC:", j) + DEFAULT_RESPONSE_LENGTH;

      iBeaconData_t iBeacon;
      uint16_t indexMatch = j;
      iBeacon.uuid      = data.substring(indexMatch+9, indexMatch+41);
      iBeacon.deviceAddress = data.substring(indexMatch+53, indexMatch+65);
      iBeacon.accessAddress = data.substring(indexMatch, indexMatch+8);
      iBeacon.major         = hexStringToByte(data.substring(indexMatch+42, indexMatch+44)) << 8;
      iBeacon.major        |= hexStringToByte(data.substring(indexMatch+44, indexMatch+46));
      iBeacon.minor         = hexStringToByte(data.substring(indexMatch+46, indexMatch+48)) << 8;
      iBeacon.minor        |= hexStringToByte(data.substring(indexMatch+48, indexMatch+50));
      iBeacon.txPower       = hexStringToByte(data.substring(indexMatch+67, indexMatch+68)) << 8;
      iBeacon.txPower      |= hexStringToByte(data.substring(indexMatch+68, indexMatch+70));
      iBeacon.txPower *= -1;

      #if (IS_GATEWAY)
        onBeaconFound(iBeacon);
      #endif
    }
  }
}

void ble_set_minor(int value) {
  ble_wait_till_active();
  String minorHex = byteToHexString(uint8_t((value & 0xFF00) >> 8)) + byteToHexString(uint8_t(value));

  delay(10);
  SerialUSB.println(ble_set_conf("MINO0x" + minorHex)); // set minor
  delay(1000);
}

void ble_set_major(int value) {
  ble_wait_till_active();
  String majorHex = byteToHexString(uint8_t((value & 0xFF00) >> 8)) + byteToHexString(uint8_t(value));

  delay(10);
  SerialUSB.println(ble_set_conf("MARJ0x" + majorHex)); // set major
  delay(1000);
}

void ble_start_advertising() {
  SerialUSB.print("Start advertising...");
  ble_wait_till_active();
  delay(10);
  SerialUSB.print(ble_set_conf("ROLE0")); // set to peripheral to enable advertising
  delay(1000);
  SerialUSB.println("done.");
}

void ble_stop_advertising() {
  SerialUSB.print("Stop advertising...");
  ble_wait_till_active();

  delay(10);
  SerialUSB.print(ble_set_conf("ROLE1")); // set to central to disable advertising
  delay(1000);
  SerialUSB.println("done.");
}








void ble_wait_till_active() {
  // Waiting before the BLE module is working
  String isWorking = ble_send_cmd("AT");
  while(isWorking.indexOf("OK") != 0) {
    delay(300);
    isWorking = ble_send_cmd("AT");
  }
}

boolean ble_set_conf(String cmd) {
  String response = ble_send_cmd("AT+" + cmd);
  return response.indexOf("OK") > -1 ? true : false;
}

String ble_get_conf(String cmd) {
  return ble_send_cmd("AT+" + cmd + "?");
}




String ble_send_cmd(String cmd) {
  bool failed = false;
  String response = "";
  //response.reserve(DEFAULT_RESPONSE_LENGTH);

  /* wait for more data if the cmd has a '+' */
  bool waitForMore = false;
  if(cmd.indexOf("+") >= 0)
    waitForMore = true;

  /* send command */
  Serial2.print(cmd);

  /* get response */
  boolean hashtag = false;
  uint32_t startMillis_BLE = millis();

  while ((response.indexOf("OK") < 0 || Serial2.available() || waitForMore) && !failed) {
    if (Serial2.available()) {
      response.concat(char(Serial2.read()));
      delayMicroseconds(2000);
    }

    // stop waiting for more data if we got a '+'
    if (waitForMore && response.indexOf("+") >= 0)
      waitForMore = false;

    if ((millis() - startMillis_BLE) >= COMMAND_TIMEOUT_TIME) {
      failed = true;
      response = "error";
      SerialUSB.print("#");
      hashtag = true;
    }
  }

  if(hashtag)
    SerialUSB.println();
  Serial2.flush();
  response.trim();
  return response;
}

int16_t getFreeRAM() {
  extern uintptr_t __bbs_end, *__brkval;
  int16_t v;
  return (uintptr_t) &v - (__brkval == 0 ? (uintptr_t) &__bbs_end : (uintptr_t) __brkval);
}

String byteToHexString(uint8_t hex) {
  String str;
  str.reserve(2);
  str.concat(nibbleToHexCharacter((hex & 0xF0) >> 4));
  str.concat(nibbleToHexCharacter(hex & 0x0F));
  return str;
}

uint8_t hexStringToByte(String str) {
  return ((hexCharacterToNibble(str[0]) << 4) & 0xF0) | (hexCharacterToNibble(str[1]) & 0x0F);
}

char nibbleToHexCharacter(uint8_t nibble) {
  return (nibble > 9) ? (char)(nibble + 'A' - 10) : (char)(nibble + '0');
}

uint8_t hexCharacterToNibble(char hex) {
  return (hex >= 'A') ? (uint8_t)(hex - 'A' + 10) : (uint8_t)(hex - '0');
}

void getBleId(char* ble_id) {
    BLEnode node;
    node = settings.read();
    if (node.set == false){
      node.set = true;
      char charSelection[] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
      char rId[8];
      for(int i = 0; i < 8; i++){
        ble_id[i] = charSelection[random(0,16)];
      }
      ble_id[8] = '\0';
      
      strncpy(node.id, ble_id, 8);
      settings.write(node);
    }
    else {
      memcpy(ble_id, node.id, 8);
    }
}


