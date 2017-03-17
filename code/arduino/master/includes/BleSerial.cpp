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

void BleSerial::detectBeacons() {
	HwSerial->println("detectBeacons");

	
	String response = getConf("DISI");

	HwSerial->println(response);


}


bool BleSerial::setConf(String cmd) {
	BLESerial->print("AT+" + cmd);

}

String BleSerial::getConf(String cmd) {
	BLESerial->flush();
	BLESerial->print("AT+" + cmd + "?");
	HwSerial->println(BLESerial->available());
	while(!BLESerial->available()){} // wait for data

	String content = "";
	char character;

	while(BLESerial->available()) {
		character = BLESerial->read();
		content.concat(character);
	}

	return content;
}