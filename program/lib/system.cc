#include "system.h"

Database dtbase;
ControlDevice control;

static bool track(uint32_t curr, std::vector<uint32_t> scheTime_, uint32_t &track_, uint8_t mode, std::mutex &busy) {

	busy.lock();
	if(mode==TRACK_MODE) {
		if(curr>=scheTime_[track_] && curr<=scheTime_.back()) {
			track_ = (track_+1) % scheTime_.size();
			busy.unlock();
			return true;
		}
		busy.unlock();
		return false;
	} else {
		for (uint i = 0; i < scheTime_.size(); ++i) {
			if(curr >= scheTime_[i]) {
				track_ = (i+1) % scheTime_.size();
			}
		}
		busy.unlock();
		return true;
	}
}

static bool trackLamp(uint32_t curr, std::vector<uint32_t> scheTime_, bool state, std::mutex &busy) {

	busy.lock();
	if(scheTime_[0]<scheTime_[1]) {
		busy.unlock();
		return ( (((scheTime_[0] <= curr) & (curr <= scheTime_[1])) != state) ? true : false );
	}
	else if(scheTime_[1]<scheTime_[0]) {
		busy.unlock();
		return ( (((scheTime_[0] <= curr) | (curr <= scheTime_[1])) != state) ? true : false );
	}
	else {
		busy.unlock();
		return false;
	}
}

static bool checkStatus(const char *collName, const char *object, const char *statusType) {

	bson_error_t error;
	bson_t *b;
	bson_iter_t iter;
	bson_iter_t field;
	
	bool res;	
	std::string jsonStr("{\"title\":\"");
	jsonStr.append(object);
	jsonStr.append("\"}");
	
	std::string jsonDoc = dtbase.readDocument(collName, (char *)jsonStr.c_str());

	if ((b = bson_new_from_json ((const uint8_t*)jsonDoc.c_str(), -1, &error))) {
		if (bson_iter_init (&iter, b) &&
		    bson_iter_find_descendant (&iter, statusType, &field) &&
		    BSON_ITER_HOLDS_BOOL (&field)) {
				res = bson_iter_bool(&field);
		}
	   	bson_destroy (b);
	}

	return res;
}

static float getFieldValue(const char *collName, const char *object, const char *key) {

	bson_error_t error;
	bson_t *b;
	bson_iter_t iter;
	bson_iter_t field;
	
	float res;	
	std::string jsonStr("{\"title\":\"");
	jsonStr.append(object);
	jsonStr.append("\"}");
	
	std::string jsonDoc = dtbase.readDocument(collName, (char *)jsonStr.c_str());

	if ((b = bson_new_from_json ((const uint8_t*)jsonDoc.c_str(), -1, &error))) {
		if (bson_iter_init (&iter, b) &&
		    bson_iter_find_descendant (&iter, key, &field) &&
		    BSON_ITER_HOLDS_INT32 (&field)) {
			res = bson_iter_int32(&field);
		}
	   	bson_destroy (b);
	}

	return res;
}

static std::vector<uint32_t> getFieldArray(const char *collName, char*jsonSelectColl, const char *key) {
	bson_error_t error;
	bson_t *b;
	bson_iter_t iter;
	bson_iter_t field;
	const uint8_t *arry = NULL;
	uint32_t len = 0;
	std::vector<uint32_t> scheTime;
	std::string jsonDoc = dtbase.readDocument(collName, jsonSelectColl);

	if ((b = bson_new_from_json ((const uint8_t*)jsonDoc.c_str(), -1, &error))) {
	   	
		if (bson_iter_init (&iter, b) &&
		    bson_iter_find_descendant (&iter, key, &field) &&
		    BSON_ITER_HOLDS_ARRAY (&field)) {

				bson_iter_array(&field, &len, &arry);
				bson_t * fSubArray = bson_new_from_data(arry, len);
				bson_iter_t fIter;

				if (bson_iter_init (&fIter, fSubArray)) {
	   	   			while (bson_iter_next (&fIter)) {
	   	      			scheTime.push_back(bson_iter_int32(&fIter));
	   	   			}
	   			}
		}

	   	bson_destroy (b);
	}

	return scheTime;
}

std::vector<int> array_map(const char* ss, const char *delim) {
    char* token;
    char* tofree;
    char* string = strdup(ss);
    std::vector<int> v;
    unsigned int count = 0;
    if (string != NULL) {
        tofree = string;
        while ((token = strsep(&string, delim)) != NULL) {
            v.push_back(atoi(token));
            count++;
        }
        free(tofree);
    }
    return v;
}

TemptAndHumy::TemptAndHumy() : ds18b(NUM_OF_DS18B, PIN_DS2), dht(PIN_DHT1) { 
	enable_ = checkStatus(COLL_SETUP, "temptAndHumy", "enable");
	enable_tempt_ = checkStatus(COLL_SETUP, "temptAndHumy", "enable_tempt");
	enable_humy_ = checkStatus(COLL_SETUP, "temptAndHumy", "enable_humy");
	temptSet = getFieldValue(COLL_SETUP, "temptAndHumy", "temptInBox");
	humySet = getFieldValue(COLL_SETUP, "temptAndHumy", "humidity");
	log_progess_tempt = false; // default is not log for progressing. It just will be turned on when user wanna test set tempt
	log_progess_humy = false; // default is not log for progressing. It just will be turned on when user wanna test set humy
	setTempt_breakout = false;
	setHumy_breakout = false;
};

TemptAndHumy::~TemptAndHumy() {}

void TemptAndHumy::enableAutoTemptAndHumy(bool enable) {
	enable_ = enable;
	scheduleTemptAndHumy();
	dtbase.update(COLL_SETUP, "title", "temptAndHumy", "enable", enable_);
}

void TemptAndHumy::begin() {
	// setup pumper
	pinMode(PIN_PUMP, OUTPUT);
	digitalWrite(PIN_PUMP, RELAY_OFF);
	// temperature begin. DS18B20
	ds18b[0].setAddr(ds18b_1_rom);
	ds18b[1].setAddr(ds18b_2_rom);
	ds18b[0].resolution(11);
	ds18b[1].resolution(11);
	// begin program of balancing temperature and humidty
	// if(enable_tempt_) {
	// 	threadTempt_flag = true;
	// 	threadTempt = new std::thread(&TemptAndHumy::handleTemperature, this);
	// } else {
	// 	threadTempt_flag = false;
	// }
	threadTempt_flag = false;
	threadHumy_flag = false;
}

float TemptAndHumy::readTempt() {
	float dht_data[2];
	busy.lock();
	bool res =  dht.read(dht_data);
	busy.unlock();
	if(res==false){
		BONE_WARN << "DHT22 read failed !";
		return RET_FAIL;
	}
	else
		return dht_data[0];
}

float TemptAndHumy::readHumy() {
	float dht_data[2];
	busy.lock();
	bool res =  dht.read(dht_data);
	busy.unlock();
	if(res==false){
		BONE_WARN << "DHT22 read failed !";
		return RET_FAIL;
	}
	else
		return dht_data[1];
}

int TemptAndHumy::readTemptAndHumy(float data[]) {
	busy.lock();
	bool res =  dht.read(data);
	busy.unlock();
	if(res==false) {
		BONE_WARN << "DHT22 read failed !";
		return RET_FAIL;
	}
	return RET_SCES;
}

int TemptAndHumy::readAllTemptAndHumy(float data[]) {
	busy.lock();
	int count = 0;
	for(int i = 0; i < NUM_OF_DS18B; ++i) {
		data[i] = ds18b[i].tempt();
		BONE_VLOG << data[i];
		if(data[i]==VAL_FAIL) {
			BONE_WARN << "DS18B20 ["<< i << "] read failed";
			continue;
		}
		count++;
	}

	float dht_data[2];
	if(dht.read(dht_data)==false)
		BONE_WARN << "DHT22 read failed !";
	else {
		data[NUM_OF_TEMPT-1] = dht_data[0];
		data[NUM_OF_TEMPT] = dht_data[1];
		BONE_VLOG << data[NUM_OF_TEMPT-1];
		BONE_VLOG << data[NUM_OF_TEMPT];
		count++;
	}

	busy.unlock();
	if(count!=NUM_OF_DS18B)
		return RET_SCES;
	else
		return RET_FAIL;
}

int TemptAndHumy::setTempt(float tempt) {
	log_progess_tempt = true;
	if(tempt!=temptSet) {
		temptSet = tempt;
		dtbase.update(COLL_SETUP, "title", "temptAndHumy", "temptInBox", (double)tempt);
		setTempt_breakout = true;
		return RET_SCES;
	} else
		return RET_MISS;
}

int TemptAndHumy::setHumy(float humy) {
	if(humy!=humySet) {
		humySet = humy;
		dtbase.update(COLL_SETUP, "title", "temptAndHumy", "humidity", (double)humy);
		return RET_SCES;
	} else
		return RET_MISS;
}

int TemptAndHumy::saveTempt(const char* coll, float tempt) {
	bson_t *doc = BCON_NEW ("temptInBox", BCON_DOUBLE (tempt));
	int res = dtbase.insertInOrderWithDateTime(coll, doc);
	bson_destroy(doc);
	return res;
}

int TemptAndHumy::saveHumy(const char* coll, float humy) {
	bson_t *doc = BCON_NEW ("humidity", BCON_DOUBLE (humy));
	int res = dtbase.insertInOrderWithDateTime(coll, doc);
	bson_destroy(doc);
	return res;
}

int TemptAndHumy::saveTemptAndHumy(float data[]) {
	return (saveTempt(COLL_TEMPT, data[0]) & saveHumy(COLL_HUMY, data[1]));
}

int TemptAndHumy::saveTemptInProgress(const char* coll, float data[]) {
	bson_t *doc = BCON_NEW ("temptIn", BCON_DOUBLE (data[0]), "temptOut", BCON_DOUBLE (data[1]), "temptInBox", BCON_DOUBLE (data[2]));
	int res = dtbase.insertInOrderWithDateTime(coll, doc);
	bson_destroy(doc);
	return res;
}

int TemptAndHumy::logData() {
	float data[2];
	if(readTemptAndHumy(data)==RET_SCES)
		return saveTemptAndHumy(data);
	else
		return RET_FAIL;
}

int TemptAndHumy::logProgress() {
	float data[NUM_OF_DS18B+NUM_OF_DS18B];
	if(readAllTemptAndHumy(data)==RET_SCES)
		return saveTemptInProgress(COLL_PROGRESS_TEMPT, data) & saveHumy(COLL_PROGRESS_HUMY, data[3]);
	else
		return RET_FAIL;
}

int TemptAndHumy::logProgressHumy() {
	float data[2];
	if(readTemptAndHumy(data)==RET_SCES)
		return saveHumy(COLL_PROGRESS_HUMY, data[1]);
	else
		return RET_FAIL;
}

void TemptAndHumy::scheduleTemptAndHumy() {
	if(enable_) {
		track_ = 0;
		char *jsonSelectColl = (char *)"{\"title\" : \"temptAndHumy\"}";
		scheTime_ = getFieldArray(COLL_SETUP, jsonSelectColl, "schedule");
		time_t now = time(NULL);
		struct tm *curr = localtime(&now);
		track((curr->tm_hour)*3600 + (curr->tm_min)*60 + curr->tm_sec, scheTime_, track_, SET_TRACK_MODE, busy);
		BONE_LOG << "Automatic reading temperature and humidity is on";
	} else {
		BONE_LOG << "Automatic reading temperature and humidity is off";
	}
}

void TemptAndHumy::scheduleTemptAndHumy(std::vector<uint32_t> v) {
	scheTime_.clear();
	scheTime_ = v;
	if(enable_) {
		time_t now = time(NULL);
		struct tm *curr = localtime(&now);
		track((curr->tm_hour)*3600 + (curr->tm_min)*60 + curr->tm_sec, scheTime_, track_, SET_TRACK_MODE, busy);
	}
	dtbase.update(COLL_SETUP, "title", "temptAndHumy", "schedule", std::vector<int>(v.begin(), v.end()));
}

void TemptAndHumy::enableControlProperty(const char* prop, bool state) {
	if(strcmp(prop, "enable_tempt")==0) {
		enable_tempt_ = state;
		if(enable_tempt_ && !threadTempt_flag) {
			threadTempt_flag = true;
			log_progess_tempt = true;
			threadTempt = new std::thread(&TemptAndHumy::handleTemperature, this);
		} else if(!enable_tempt_) {
			threadTempt_flag = false;
			log_progess_tempt = false;
		}
	}
	else {
		enable_humy_ = state;
		if(enable_humy_ && !threadHumy_flag) {
			threadHumy_flag = true;
			log_progess_humy = true;
			threadHumy = new std::thread(&TemptAndHumy::handleHumidity, this);
		} else if(!enable_humy_) {
			threadHumy_flag = false;
			log_progess_humy = false;
		}
	}
	dtbase.update(COLL_SETUP, "title", "temptAndHumy", prop, state);
}

void TemptAndHumy::handleTemperature() {

	bool flag_balance = false, flag_hot = false, flag_cold = false;

	while(threadTempt_flag) {
		
		float tempt = readTempt();
		
		// perfect range: temptSet - 0.5 <= tempt <= temptSet + 0.5. turn off everything and delay 10'
		if( (temptSet - 0.5 <= tempt) && (tempt <= temptSet + 0.5) ) {
			flag_balance = true;
			if(flag_hot) {
				flag_hot = false;
				control.switchDevice(HOT, OFF);
			}
			if(flag_cold) {
				flag_hot = false;
				control.switchDevice(COLD, OFF);
			}
			control.switchDevice(FAN_BOX, OFF);
			control.switchDevice(FAN_OUTSIDE, OFF);
			digitalWrite(PIN_PUMP, RELAY_OFF);
			log_progess_tempt = false; // from now will no longer log for setting tempt progess
			TemptAndHumy::logData();
			BONE_LOG << "Get the perfect point! (temptSet - 0.5 <= tempt) && (tempt <= temptSet + 0.5)";
			for (int i = 0; i < 300; ++i) {	// sleep 10 minutes
				if(setTempt_breakout) {
					setTempt_breakout = false;
					break;
				}
				std::this_thread::sleep_for(std::chrono::seconds(2));	
			}
			continue;
		}
		// control hot // range: temptSet - 3 < tempt < temptSet - 0.5, Doing turn heat depend on flag_balance which has setted before or not
		else if( (temptSet - 3 < tempt) && (tempt < temptSet - 0.5) && !flag_balance ) {
			if(!flag_hot) {
				BONE_LOG << "Hot control: (temptSet - 3 < tempt) && (tempt < temptSet - 0.5) && !flag_balance ) || (tempt <= temptSet - 3)";
				control.switchDevice(HOT, ON);
				control.switchDevice(FAN_BOX, ON);
				control.switchDevice(FAN_OUTSIDE, ON);
				digitalWrite(PIN_PUMP, RELAY_ON);
				flag_hot = true;
			}
		}
		// control hot // range: temptSet - 3 < tempt < temptSet - 0. have gotten the perfect range
		else if( (temptSet - 3 < tempt) && (tempt < temptSet - 0.5) && flag_balance ) {
			BONE_LOG << "Hot control: (temptSet - 3 < tempt) && (tempt < temptSet - 0.5) && flag_balance )";
			for (int i = 0; i < 150; ++i) {	// sleep 5 minutes
				if(setTempt_breakout) {
					setTempt_breakout = false;
					break;
				}
				std::this_thread::sleep_for(std::chrono::seconds(2));	
			}
		}
		// control hot // poor range: tempt <= temptSet - 3 
		else if(tempt <= temptSet - 3) {
			if(!flag_hot) {
				BONE_LOG << "Hot control: tempt <= temptSet - 3 !";
				control.switchDevice(HOT, ON);
				control.switchDevice(FAN_BOX, ON);
				control.switchDevice(FAN_OUTSIDE, ON);
				digitalWrite(PIN_PUMP, RELAY_ON);
				flag_hot = true;
				flag_balance = false;
			}
		} 
		// control cold // range: temptSet + 0.5 < tempt < temptSet + 3, Doing turn cold depend on flag_balance which has setted before or not
		else if( (temptSet + 0.5 < tempt) && (tempt < temptSet + 3) && !flag_balance ) {
			if(!flag_cold) {
				BONE_LOG << "Cold control: ( (temptSet + 0.5 < tempt) && (tempt < temptSet + 3) && !flag_balance ) || (temptSet + 3 <= tempt)";
				control.switchDevice(COLD, ON);
				control.switchDevice(FAN_BOX, ON);
				control.switchDevice(FAN_OUTSIDE, ON);
				digitalWrite(PIN_PUMP, RELAY_ON);
				flag_cold = true;
			}
		}
		// control cold // range: temptSet + 0.5 < tempt < temptSet + 3, have gotten the perfect range
		else if( (temptSet + 0.5 < tempt) && (tempt < temptSet + 3) && flag_balance ) {
			BONE_LOG << "Cold control: (temptSet + 0.5 < tempt) && (tempt < temptSet + 3) && flag_balance ";
			for (int i = 0; i < 150; ++i) {	// sleep 5 minutes
				if(setTempt_breakout) {
					setTempt_breakout = false;
					break;
				}
				std::this_thread::sleep_for(std::chrono::seconds(2));	
			}
		}
		// control cold // poor range: temptSet + 3 <= tempt
		else if(temptSet + 3 <= tempt) {
			if(!flag_cold) {
				BONE_LOG << "Cold control: temptSet + 3 <= tempt";
				control.switchDevice(COLD, ON);
				control.switchDevice(FAN_BOX, ON);
				control.switchDevice(FAN_OUTSIDE, ON);
				digitalWrite(PIN_PUMP, RELAY_ON);
				flag_cold = true;
				flag_balance = false;
			}
		}

		// BONE_LOG << "temperature: " << tempt;
		if(log_progess_tempt) {
			TemptAndHumy::logProgress();
		}
		
		std::this_thread::sleep_for(std::chrono::seconds(2));
	}
	// turn off all temperature system
	control.switchDevice(HOT, OFF);
	control.switchDevice(COLD, OFF);
	control.switchDevice(FAN_BOX, OFF);
	control.switchDevice(FAN_OUTSIDE, OFF);
	digitalWrite(PIN_PUMP, RELAY_OFF);

}

void TemptAndHumy::handleHumidity() {
	while(threadHumy_flag) {
		BONE_LOG << "humidity system working !";
		TemptAndHumy::logProgressHumy();
		std::this_thread::sleep_for(std::chrono::seconds(5));
	}
}

Lamp::Lamp() {
	enable_ = checkStatus(COLL_SETUP, "lamp", "enable");
	state_ = checkStatus(COLL_SETUP, "lamp", "state");
}

Lamp::~Lamp() {}

void Lamp::begin() {
	// light metter begin. BH1750
	pinMode(PIN_LAMP, OUTPUT);
	digitalWrite(PIN_LAMP, RELAY_OFF);
	
	lightMeter.begin();

	if(!enable_) {
		if(state_)
			switchLamp(state_, INSTANT);
	}
	else
		state_ = false;		// syns state between state_ and status of real device (When turn on system, lamp is off).
}

void Lamp::enableAutoLamp(bool enable) {
	enable_ = enable;
	scheduleLamp();
	dtbase.update(COLL_SETUP, "title", "lamp", "enable", enable_);
}

int Lamp::switchLamp(bool state, bool saveMode) {
	BONE_LOG << "switch lamp: " << (state ? "on" : "off");
	state_ = state;
	digitalWrite(PIN_LAMP, NOT(state));
	if(state_) {	// measure the light level of lamp
		usleep(250000);		// wait for lamp on
		Lamp::lightLevel();
	}
	if(saveMode)
		return dtbase.update(COLL_SETUP, "title", "lamp", "state", state);
	else
		return RET_SCES;
}

// light level of the lamp. unit lux
int Lamp::lightLevel() {
	uint16_t level = lightMeter.readLightLevel();
	bson_t *doc = BCON_NEW ("lightLevel", BCON_INT32 (level));
	int res = dtbase.insertInOrderWithDateTime(COLL_LAMP, doc);
	bson_destroy(doc);
	BONE_LOG << "The light level: " << level << " lux";
	return res;
}

void Lamp::scheduleLamp() {
	if(enable_) {
		BONE_LOG << "Automatic lamp is on";
		char *jsonSelectColl = (char *)"{\"title\" : \"lamp\"}";
		scheTime_ = getFieldArray(COLL_SETUP, jsonSelectColl, "schedule");
		time_t now = time(NULL);
		struct tm *curr = localtime(&now);
		if(trackLamp((curr->tm_hour)*3600 + (curr->tm_min)*60 + curr->tm_sec, scheTime_, state_, busy))
			switchLamp(!state_);
	} else {
		BONE_LOG << "Automatic lamp is off";
	}
}

void Lamp::scheduleLamp(std::vector<uint32_t> v) {
	scheTime_.clear();
	scheTime_ = v;
	time_t now = time(NULL);
	struct tm *curr = localtime(&now);
	if(enable_) {
		if(trackLamp((curr->tm_hour)*3600 + (curr->tm_min)*60 + curr->tm_sec, scheTime_, state_, busy))
			switchLamp(!state_);
	}
	dtbase.update(COLL_SETUP, "title", "lamp", "schedule", std::vector<int>(v.begin(), v.end()));
}

Ventilation::Ventilation() {
	enable_ = checkStatus(COLL_SETUP, "ventilation", "enable");
	state_ = checkStatus(COLL_SETUP, "ventilation", "state");
}

Ventilation::~Ventilation() {}

void Ventilation::begin() {
	if(!enable_) {
		if(state_)
			control.switchDevice(VENTILATION_WIN, state_);
		state_ = false;
	}
	else {}
}

void Ventilation::enableAutoVentilation(bool enable) {
	enable_ = enable;
	scheduleVentilation();
	dtbase.update(COLL_SETUP, "title", "ventilation", "enable", enable_);
}

int Ventilation::switchVentilation(bool state, bool saveMode) {
	BONE_LOG << "switch ventilation: " << (state ? "on" : "off");
	state_ = state;
	control.switchDevice(VENTILATION, state_);
	if(saveMode)
		return dtbase.update(COLL_SETUP, "title", "ventilation", "state", state_);
	else
		return RET_SCES;
}

void Ventilation::scheduleVentilation() {
	if(enable_) {
		if(state_)
			switchVentilation(state_);
		state_ = false;
		track_ = 1;
		char *jsonSelectColl = (char *)"{\"title\" : \"ventilation\"}";
		cycleTime_ = getFieldArray(COLL_SETUP, jsonSelectColl, "cycle");
		countInterVal_ = 0;
		BONE_LOG << "Automatic ventilation is on";
	} else {
		BONE_LOG << "Automatic ventilation is off";
	}
}

void Ventilation::scheduleVentilation(std::vector<uint32_t> v) {
	if(enable_) {
		if(state_)
			switchVentilation(state_);
		state_ = false;
	}
	track_ = 1;
	cycleTime_.clear();
	cycleTime_ = v;
	dtbase.update(COLL_SETUP, "title", "ventilation", "cycle", std::vector<int>(v.begin(), v.end()));
	countInterVal_ = 0;
}

Camera::Camera() {}
Camera::~Camera() {}

void Camera::scheduleCamera() {
	char *jsonSelectColl = (char *)"{\"title\" : \"camera\"}";
	scheTime_ = getFieldArray(COLL_SETUP, jsonSelectColl, "schedule");

	time_t now = time(NULL);
	struct tm *curr = localtime(&now);
	track((curr->tm_hour)*3600 + (curr->tm_min)*60 + curr->tm_sec, scheTime_, track_, SET_TRACK_MODE, busy);
}

void System::begin() {
	wiringPiSetupGpio();	// set up GPIO use the Broadcom GPIO pin numbers
	control.begin();
	Lamp::begin();
	TemptAndHumy::begin();
	Ventilation::begin();
}

System::System() {}
System::~System() {}

void System::autoTracking() {

	while(1) {

		time_t now = time(NULL);
		struct tm *curr = localtime(&now);
		uint32_t secs = (curr->tm_hour)*3600 + (curr->tm_min)*60 + curr->tm_sec;

		if(TemptAndHumy::enable_) {
			if(track(secs, TemptAndHumy::scheTime_, TemptAndHumy::track_, TRACK_MODE, TemptAndHumy::busy)) {
				BONE_LOG << "alarm tempt and humy, track: " << TemptAndHumy::track_;
				logData();
			}
		}
		
		if(Lamp::enable_) {
			if(trackLamp(secs, Lamp::scheTime_, Lamp::state_, Lamp::busy)) {
				switchLamp(!Lamp::state_);
			}
		}

		if(Ventilation::enable_) {
			if(countInterVal_==Ventilation::cycleTime_[Ventilation::track_]) {
				switchVentilation(!Ventilation::state_);
				Ventilation::track_ = (Ventilation::track_+1) % (Ventilation::cycleTime_.size());
				countInterVal_ = 0;
			}
			countInterVal_++;
		}

		// BONE_LOG << "";
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

void System::schedule(bool state) {
	scheduleTemptAndHumy();
	scheduleLamp();
	// scheduleCamera();
	scheduleVentilation();
	thread_schedu_ = std::thread(&System::autoTracking, this);
}

void System::shutdown() {
	pinMode(PIN_LAMP, INPUT);
	pinMode(PIN_PUMP, INPUT);
	pinMode(PIN_DS1, INPUT);
	pinMode(PIN_DS2, INPUT);
	pinMode(PIN_DHT1, INPUT);
	pinMode(PIN_DHT2, INPUT);
	control.shutdown();			// shutdown control device system
}