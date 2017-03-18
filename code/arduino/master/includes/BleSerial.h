//https://github.com/DINGGLABS/BLE_HM11/blob/master/BLE_HM11.h
//http://www.warski.org/blog/2014/01/how-ibeacons-work/
#ifndef BleSerial_h

#define BleSerial_h
#include <Arduino.h>
#include <SoftwareSerial.h>

#define CMD_TIMEOUT 800

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


class BleSerial {

	typedef enum : uint32_t {
		BAUDRATE0 = 9600,
		BAUDRATE1 = 19200,
		BAUDRATE2 = 38400,
		BAUDRATE3 = 57600,
		BAUDRATE4 = 115200
	} baudrate_t;
	
	
private:
	static const baudrate_t DEFAULT_BAUDRATE         = BAUDRATE0;
	static const uint8_t DEFAULT_RESPONSE_LENGTH     = 8;          // in characters
	static const uint16_t COMMAND_TIMEOUT_TIME       = 100;        // in ms (discovered empirically)
	static const uint16_t DELAY_AFTER_HW_RESET_BLE   = 300;        // in ms (discovered empirically)
	static const uint16_t DELAY_AFTER_SW_RESET_BLE   = 900;        // in ms (discovered empirically)

	// I-Beacon detector
	static const uint16_t DEFAULT_DETECTION_TIME     = 5000;       // in ms
	static const uint16_t MIN_RAM = 253; // in bytes -> keep the RAM > 200 to prevent bugs!


	SoftwareSerial * BLESerial;
	HardwareSerial * HwSerial;
	bool setConf(String cmd);
	String getConf(String cmd);
	String getConf(String cmd, boolean formatCmd, boolean returnAnswer);
	String sendCmd(String cmd);
	int16_t getFreeRAM();
	String byteToHexString(uint8_t hex);
	uint8_t hexStringToByte(String str);
	char nibbleToHexCharacter(uint8_t nibble);
	uint8_t hexCharacterToNibble(char hex);

public:
	BleSerial(SoftwareSerial * softwareSerial, HardwareSerial * hardwareSerial);
	void setupAsDetector();
	void detectBeacons(void (*callback)(iBeaconData_t beacon), uint16_t maxTimeToSearch);
	void sleep();
	void wake();
	void setMaster();
	void setSlave();

};







#endif