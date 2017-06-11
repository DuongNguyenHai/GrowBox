/*

Author : Nguyen Hai Duong
Mar 8 2017
=========================

This is library for DS18B20 sensor. It works on raspberry pi

*/ 

#ifndef DS18B20_H
#define DS18B20_H

#include <stdio.h>
#include "OneWire.h"

#define VAL_FAIL -100			// define wrong value, its used for checking fail data

class DS18B20 : public OneWire {

public:
	DS18B20(uint8_t pin) : OneWire(pin) { mode_ = 12; }; // add pin for one wire pin
	~DS18B20();
	void setAddr(const uint8_t rom[8]);		// set address for only slave device
	void resolution(uint8_t mode);			// 9, 10, 11, 12 bit resolution, default mode_ is 12
	float tempt();							// read temperature
	void readAddr(uint8_t rom[8]);
private:
	uint8_t addr_[8];
	uint8_t mode_;
};

#endif