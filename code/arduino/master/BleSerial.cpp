#include "BleSerial.h"

BleSerial::BleSerial(SoftwareSerial * softwareSerial, HardwareSerial * hardwareSerial){
	HwSerial = hardwareSerial;
	BLESerial = softwareSerial;
	BLESerial->begin(9600);
	while(!BLESerial);
	BLESerial->flush();
}

void BleSerial::setupAsDetector() {
	HwSerial->println("cavia");
}

void BleSerial::setMaster() {
	waitUntillBleIsActive();

	setConf("IMME1");
	setConf("ROLE1"); // set to Central
	setConf("SHOW1");
	setConf("RESET");

	delay(1000);
}

void BleSerial::setSlave() {
	HwSerial->println("> Set slave");

	//HwSerial->println(setConf("RENEW"));

	//return;

	waitUntillBleIsActive();
  
	HwSerial->println(setConf("ROLE0")); // set to Peripheral 
	delay(100);
	HwSerial->println(setConf("IBE000001338"));
	delay(100);
	HwSerial->println(setConf("MARJ0x1337")); // set major
	delay(100);
	HwSerial->println(setConf("MINO0x1337")); // set minor
	delay(100);
	HwSerial->println(setConf("ADVI5")); // set interval
	delay(100);
	HwSerial->println(setConf("NAMEBeLowBeacon"));
	delay(100);

	HwSerial->println(setConf("ADTY3")); // advertising type (3 = advertising only)
	delay(100);
	HwSerial->println(setConf("IBEA1")); // work as iBeacon
	delay(100);
	HwSerial->println(setConf("DELO2")); // iBeacon deploy mode (2 = broadcast only)
	delay(100);

	HwSerial->println(setConf("PWRM1")); // auto sleep ON
	delay(100);
	HwSerial->println(setConf("ROLE1")); // set to Peripheral 
	delay(100);
	HwSerial->println(setConf("RESET"));

	delay(1000);

	waitUntillBleIsActive();
	HwSerial->println("> Ending set slave");
}

void BleSerial::setMinor(int value) {
  String minorHex = byteToHexString(uint8_t((value & 0xFF00) >> 8)) + byteToHexString(uint8_t(value));
  
  delay(10);
  HwSerial->println(setConf("MINO0x" + minorHex)); // set major
  //HwSerial->println(setConf("RESET"));
  delay(1000);
}

void BleSerial::setMajor(int value) {
  String majorHex = byteToHexString(uint8_t((value & 0xFF00) >> 8)) + byteToHexString(uint8_t(value));
  
  delay(10);
  HwSerial->println(setConf("MARJ0x" + majorHex)); // set major
  //HwSerial->println(setConf("RESET"));
  delay(1000);
}

void BleSerial::startAdvertising() {
  HwSerial->println("> Start advertising");
  waitUntillBleIsActive();
 
  delay(10);
  HwSerial->println(setConf("ROLE0")); // set to peripheral to enable advertising
  delay(1000);
}

void BleSerial::stopAdvertising() {
  HwSerial->println("> Stop advertising");
  waitUntillBleIsActive();
  
  delay(10);
  HwSerial->println(setConf("ROLE1")); // set to central to disable advertising
  delay(1000);
}

void BleSerial::wake() {
  HwSerial->println(setConf("123456781234567812345678123456781234567812345678123456781234567812345678912345678"));
  delay(1000);
}

void BleSerial::sleep() {
  HwSerial->println(setConf("SLEEP"));
  delay(1000);
}

void BleSerial::factoryReset() {
	setConf("RENEW");
}

void BleSerial::waitUntillBleIsActive() {
	// Waiting before the BLE module is working
	String isWorking = sendCmd("AT");
	while(isWorking.indexOf("OK") != 0) {
		delay(300);
		isWorking = sendCmd("AT");
	}
}

void BleSerial::detectBeacons(void (*callback)(iBeaconData_t beacon), uint16_t maxTimeToSearch) {
	waitUntillBleIsActive();

	// Start finding nodes
	String response = getConf("DISI");
	if(response.indexOf("OK+DISIS") >= 0) {
		HwSerial->println("Searching for nodes...");

		String data = "";
		String uuidHex = "";
		bool timeout = false;
    bool hashtag = false;
		uint32_t startMillis_BLE_total = millis();

		int16_t freeBytes = getFreeRAM() - MIN_RAM;
		data.reserve(freeBytes);

		while(data.indexOf("OK+DISCE") < 0 && !timeout && freeBytes > 0) {
			if (BLESerial->available() > 0) {
				data.concat(String(char(BLESerial->read())));
				freeBytes--;
			}

			if ((millis() - startMillis_BLE_total) >= maxTimeToSearch) {
				timeout = true;
				HwSerial->print("*");
        hashtag = true;
			}
		}
    if(hashtag)
      HwSerial->println();

		//HwSerial->println(data);

		uint16_t j = 0;
		byte deviceCounter;
		for(deviceCounter = 0; data.indexOf("OK+DISC:", j) >= 0; deviceCounter++) {
			j = data.indexOf("OK+DISC:", j) + DEFAULT_RESPONSE_LENGTH;

			iBeaconData_t iBeacon;
			uint16_t indexMatch = j;
			iBeacon.uuid 		  = data.substring(indexMatch+9, indexMatch+41);
			iBeacon.deviceAddress = data.substring(indexMatch+53, indexMatch+65);
			iBeacon.accessAddress = data.substring(indexMatch, indexMatch+8);
			iBeacon.major         = hexStringToByte(data.substring(indexMatch+42, indexMatch+44)) << 8;
			iBeacon.major        |= hexStringToByte(data.substring(indexMatch+44, indexMatch+46));
			iBeacon.minor         = hexStringToByte(data.substring(indexMatch+46, indexMatch+48)) << 8;
			iBeacon.minor        |= hexStringToByte(data.substring(indexMatch+48, indexMatch+50));
			iBeacon.txPower       = hexStringToByte(data.substring(indexMatch+67, indexMatch+68)) << 8;
			iBeacon.txPower      |= hexStringToByte(data.substring(indexMatch+68, indexMatch+70));
			iBeacon.txPower *= -1;

			callback(iBeacon);
		}

		//HwSerial->print("DeviceCount ");
		//HwSerial->println(deviceCounter);
	}
}


bool BleSerial::setConf(String cmd) {
	String response = sendCmd("AT+" + cmd);
	return response.indexOf("OK") > -1 ? true : false;
}

String BleSerial::getConf(String cmd) {
	return sendCmd("AT+" + cmd + "?");
}

String BleSerial::sendCmd(String cmd) {
	bool failed = false;
	String response = "";
	response.reserve(DEFAULT_RESPONSE_LENGTH);

	/* wait for more data if the cmd has a '+' */
	bool waitForMore = false;
	if(cmd.indexOf("+") >= 0) 
		waitForMore = true;

	/* send command */
	BLESerial->print(cmd);

	/* get response */
  boolean hashtag = false;
	uint32_t startMillis_BLE = millis();
	while ((response.indexOf("OK") < 0 || BLESerial->available() || waitForMore) && !failed) {
		if (BLESerial->available()) {
			response.concat(char(BLESerial->read()));
			delayMicroseconds(100);
		}

		/* stop waiting for more data if we got a '+' */
		if (waitForMore && response.indexOf("+") >= 0)
			waitForMore = false;

		if ((millis() - startMillis_BLE) >= COMMAND_TIMEOUT_TIME) {
			failed = true;
			response = "error";
			HwSerial->print("#");
      hashtag = true;
		}
	}
  if(hashtag)
    HwSerial->println();

	BLESerial->flush();

	response.trim();

	return response;
}

int16_t BleSerial::getFreeRAM() {
	extern uintptr_t __heap_start, *__brkval;
	int16_t v;
	return (uintptr_t) &v - (__brkval == 0 ? (uintptr_t) &__heap_start : (uintptr_t) __brkval);
}

String BleSerial::byteToHexString(uint8_t hex) {
	String str;
	str.reserve(2);
	str.concat(nibbleToHexCharacter((hex & 0xF0) >> 4));
	str.concat(nibbleToHexCharacter(hex & 0x0F));
	return str;
}

uint8_t BleSerial::hexStringToByte(String str) {
	return ((hexCharacterToNibble(str[0]) << 4) & 0xF0) | (hexCharacterToNibble(str[1]) & 0x0F);
}

char BleSerial::nibbleToHexCharacter(uint8_t nibble) {
	return (nibble > 9) ? (char)(nibble + 'A' - 10) : (char)(nibble + '0');
}

uint8_t BleSerial::hexCharacterToNibble(char hex) {
	return (hex >= 'A') ? (uint8_t)(hex - 'A' + 10) : (uint8_t)(hex - '0');
}
