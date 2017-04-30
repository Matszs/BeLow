#ifndef platform_h
  #define platform_h
  
  #include "FlashAsEEPROM.h"
  #include "FlashStorage.h"
  #include "OneWire.h"
  #include "DallasTemperature.h"
  #define ONE_WIRE_BUS 4
  
  typedef struct {
    char id[9];
    bool set;
  } BLEnode;
  
  typedef enum {
    GATEWAY,
    DOOR_SENSOR,
    PIR_SENSOR,
    TEMPERATURE_SENSOR,
    LDR_SENSOR
  } sensor_type;
  
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
  
  extern boolean deviceIsSleeping;
  extern int sleepTimerMillis;
  
  void lora_init();
  void ble_start();
  void ble_set_master();
  void ble_set_slave();
  void ble_scan();
  void ble_set_minor(int value);
  void ble_set_major(int value);
  void ble_start_advertising();
  void ble_stop_advertising();
  void ble_wait_till_active();
  boolean ble_set_conf(String cmd);
  String ble_get_conf(String cmd);
  String ble_send_cmd(String cmd);
  void getBleId(char* ble_id);
  void sleepDevice();
  void wakeDevice();
  void sleepActions();
  
  int16_t getFreeRAM();
  String byteToHexString(uint8_t hex);
  uint8_t hexStringToByte(String str);
  char nibbleToHexCharacter(uint8_t nibble);
  uint8_t hexCharacterToNibble(char hex);

#endif
