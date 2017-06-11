#ifndef SYSTEM_H
#define SYSTEM_H

#include <iostream>
#include <vector>
#include <time.h>
#include <thread>         // std::thread, std::this_thread::sleep_for
#include <mutex>          // std::mutex
#include <chrono>         // std::chrono::seconds
#include <atomic>		  // atomic variable
#include <wiringPi.h>
#include "bone-config.h"
#include "bone-database.h"
#include "DHT22.h"
#include "DS18B20.h"
#include "BH1750.h"
#include "config.h"
#include "communicate.h"

using namespace BONE;

#ifndef RET_MISS
#define RET_MISS -1
#endif
#ifndef RET_SCES
#define RET_SCES 0
#endif
#ifndef RET_FAIL
#define RET_FAIL 1
#endif

#define ENABLE 1
#define DISABLE 0
	
#define INSTANT 0		// use it when dont wanna save data in database

#define NUM_OF_TEMPT NUM_OF_DS18B+NUM_OF_DHT
#define NUM_OF_HUMY NUM_OF_DHT

#define NOT(state) (!state)

#define TRACK_MODE 0
#define SET_TRACK_MODE 1

#define ON 0x01
#define OFF 0x00

#define RELAY_ON 0
#define RELAY_OFF 1

extern Database dtbase;
extern ControlDevice control;
std::vector<int> array_map(const char* ss, const char *delim);

class TemptAndHumy {

public:
	TemptAndHumy();
	~TemptAndHumy();
	
	void begin(); // ok

	// read tempt and humy. Almost is read tempt and humy in box except function readAllTemptAndHumy()
	float readTempt();					// return temperature value in box
	float readHumy();					// return humidity value in the box
	int readTemptAndHumy(float data[]);	// read tempt and humy
	int readAllTemptAndHumy(float data[]); // read all tempt + humy in whole system

	// set tempt and humy
	int setTempt(float tempt);
	int setHumy(float humy); 
	
	// save tempt and humy in database
	int saveTempt(const char* coll, float tempt); // save tempt in box ok
	int saveHumy(const char* coll, float humy); // save humidity in box ok
	int saveTemptAndHumy(float data[]); // ok
	// save tempt and humy in progress setting to database
	int saveTemptInProgress(const char* coll, float data[]); // ok

	// read and save tempt + huy
	int logData();
	int logProgress();	// read all tempt + humy in progress setting and save them to database
	int logProgressHumy();

	void scheduleTemptAndHumy(); // ok
	void scheduleTemptAndHumy(std::vector<uint32_t> v); 

	void enableControlProperty(const char* prop, bool state);
	void enableAutoTemptAndHumy(bool state); // ok

protected:
	std::vector<uint32_t> scheTime_;
	uint32_t track_;
	std::mutex busy;
	bool enable_;
	bool enable_tempt_;
	bool enable_humy_;
	std::atomic<float> temptSet;
	std::atomic<float> humySet;
private:
	std::vector<DS18B20> ds18b;				// group of sensor DS18B20
	DHT22 dht;								// DHT22 sensor
	std::atomic<bool> threadTempt_flag;  	// use it to shutdown thread of tempt
	std::atomic<bool> threadHumy_flag;  	// use it to shutdown thread of humy
	std::atomic<bool> log_progess_tempt;	// flag of temperature processing
	std::atomic<bool> log_progess_humy;		// flag of humidity processing
	std::atomic<bool> setTempt_breakout;
	std::atomic<bool> setHumy_breakout;
	std::thread *threadTempt;				// thead of balancing temperature
	std::thread *threadHumy;				// thead of balancing humidity		
	void handleTemperature();				// balancing temperature function
	void handleHumidity();					// balancing humidity function
};

class Lamp {
public:
	Lamp();
	~Lamp();

	BH1750 lightMeter;

	void begin(); // ok
	int switchLamp(bool state, bool saveMode = true);
	int lightLevel(); // 
	void scheduleLamp(); //ok
	void scheduleLamp(std::vector<uint32_t> v); //ok
	void enableAutoLamp(bool state);

protected:
	bool state_;
	bool enable_;
	std::vector<uint32_t> scheTime_;
	std::mutex busy;
};

class Ventilation {
public:
	Ventilation();
	~Ventilation();
	void begin();
	int switchVentilation(bool state, bool saveMode = true);
	void scheduleVentilation(); // ok
	void scheduleVentilation(std::vector<uint32_t> v); // ok
	void enableAutoVentilation(bool state);
protected:
	bool state_;
	bool enable_;
	std::vector<uint32_t> cycleTime_;
	uint32_t track_;
	uint32_t countInterVal_;
	std::mutex busy;
};

class Camera {
public:
	Camera();
	~Camera();
	void begin();
	int picture();
	void scheduleCamera(); // ok

protected:
	std::vector<uint32_t> scheTime_;
	uint32_t track_;
	std::mutex busy;
	bool enable_;
};

class System : public TemptAndHumy, public Lamp, public Camera, public Ventilation {

public:
	System();
	~System();
	void begin();
	void schedule(bool state);
	void shutdown();
	std::thread thread_schedu_;
private:
	void autoTracking();
	
};


#endif