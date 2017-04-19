#define IS_GATEWAY 0 // 1 == gateway | 0 == node

#if (IS_GATEWAY)
  #include "rn2xx3.h"

  #define DevID "14203842"
  #define NwkSKey "e37001af062ed04f193a12a7eccecee1"
  #define AppSkey "5e9139972fe9767cb317fd8525a0982b"
#endif

static const uint8_t DEFAULT_RESPONSE_LENGTH     = 8;          // in characters
static const uint16_t COMMAND_TIMEOUT_TIME       = 100;        // in ms (discovered empirically)
static const uint16_t DELAY_AFTER_HW_RESET_BLE   = 300;        // in ms (discovered empirically)
static const uint16_t DELAY_AFTER_SW_RESET_BLE   = 900;        // in ms (discovered empirically)

static const uint16_t DEFAULT_DETECTION_TIME     = 5000;       // in ms
static const uint16_t MIN_RAM = 253; // in bytes -> keep the RAM > 200 to prevent bugs!

#if (IS_GATEWAY)
  rn2xx3 myLora(Serial1);
#endif

typedef enum : uint8_t {
  INTERV_100MS  = 0,
  INTERV_150MS  = 1,
  INTERV_210MS  = 2,
  INTERV_320MS  = 3,
  INTERV_420MS  = 4,
  INTERV_550MS  = 5,
  INTERV_760MS  = 6,
  INTERV_850MS  = 7,
  INTERV_1020MS = 8,
  INTERV_1285MS = 9
} advertInterval_t;

typedef struct {
  String name;               // 12 bytes
  String uuid;               // 16 bytes
  uint16_t major;            // 2 bytes
  uint16_t minor;            // 2 bytes
  advertInterval_t interv;   // 4 bytes
  String accessAddress;      // 8 bytes
  String deviceAddress;      // 12 bytes
  int16_t txPower;           // 2 bytes
} iBeaconData_t;

#if (IS_GATEWAY)
  void onBeaconFound(iBeaconData_t beacon) {
    if(beacon.uuid == "00001338B64445208F0C720EAF059935") {
      SerialUSB.print("Beacon found: ");
      SerialUSB.println(beacon.major);
      myLora.tx(String(beacon.major));
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
  
      ble_set_major(128);
  
      ble_start_advertising();
      delay(5000);
      ble_stop_advertising();
      delay(3000);

    }
  #endif
}





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

  SerialUSB.println(ble_set_conf("ROLE0"));
  SerialUSB.println(ble_set_conf("IBE000001338"));
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
  String minorHex = byteToHexString(uint8_t((value & 0xFF00) >> 8)) + byteToHexString(uint8_t(value));

  delay(10);
  SerialUSB.println(ble_set_conf("MINO0x" + minorHex)); // set minor
  delay(1000);
}

void ble_set_major(int value) {
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
