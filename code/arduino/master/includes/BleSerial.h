//https://github.com/DINGGLABS/BLE_HM11/blob/master/BLE_HM11.h
#ifndef BleSerial_h

#define BleSerial_h
#include <Arduino.h>
#include <SoftwareSerial.h>

class BleSerial {

private:
	SoftwareSerial * BLESerial;
	HardwareSerial * HwSerial;
	bool setConf(String cmd);
	String getConf(String cmd);

public:
	BleSerial(SoftwareSerial * softwareSerial, HardwareSerial * hardwareSerial);
	void setupAsDetector();
	void detectBeacons();

};







#endif