// Nguyen Hai Duong
// May, June 2017

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include "system.h"

using namespace BONE;
#define DATABASE_GROWBOX "GrowBox"
#define PORT 8888

static int second2hour(int sec);
static std::string printHourSchedule(std::vector<int> v);
void sig_handler(int signo);

System *sys;

int main (int   argc, char *argv[]) {

    PRINT_FILE = true;
    DEBUG_LEVEL = 2;
	char buff[BUFFSIZE];
	
    signal(SIGINT, sig_handler);
	// Start connect to database. dtbase has declareed in system.h file
	dtbase.begin(DATABASE_GROWBOX);		// its really important to put this line at the top of main program

	// main control system
	System system;
    system.begin();
	system.schedule(true);	// start tracking schedule
    
    sys = &system;
    // Open socket to listen message form web-server
    Socket mailBox;
    mailBox.creatSocket(PORT); // open connection
    
	while(1) {

		memset(buff, 0, BUFFSIZE);
		if(mailBox.readMessage(buff)>1) {

			BONE_LOG << buff;
			
			char *object = strtok(buff, ":");
    		char *key = strtok(NULL, ":");
    		char *value = strtok(NULL, ":");

    		if(strcmp(object, "lamp")==0) {
    			if(strcmp(key, "state")==0) {
                    bool st = (strcmp(value, "true")==0) ? true: false;
                    system.switchLamp(st);
    			}
    			else if(strcmp(key, "schedule")==0) {
                    std::vector<int> arr;
                    arr = array_map(value, ",");
                    system.scheduleLamp(std::vector<uint32_t>(arr.begin(), arr.end()));
                    BONE_LOG << "The Lamp will be turn on from " << second2hour(arr[0]) << "h to " << second2hour(arr.back()) << "h";
    			}
    			else if(strcmp(key, "enable")==0) {
                    system.enableAutoLamp((strcmp(value, "true")==0) ? true: false);
    			}
    		} 
    		else if(strcmp(object, "temptAndHumy")==0) {
    			if(strcmp(key, "schedule")==0) {
                    std::vector<int> arr;
                    arr = array_map(value, ",");
                    system.scheduleTemptAndHumy(std::vector<uint32_t>(arr.begin(), arr.end()));
                    BONE_LOG << "The schedule of reading temperature and humidity has changed to: " << printHourSchedule(arr) << " (h)";
    			}
    			else if(strcmp(key, "temptInBox")==0) {
                    system.setTempt(atoi(value));
    			}
    			else if(strcmp(key, "humidity")==0) {
                    control.switchDevice(HUMIDITY, (atoi(value)>80?true:false));
                    system.setHumy(atoi(value));
    			}
    			else if(strcmp(key, "enable")==0) {
                    system.enableAutoTemptAndHumy( (strcmp(value, "true")==0) ? true: false );
    			}
                else if(strcmp(key, "reading")==0) {
                    BONE_LOG << "Reading temperature and humidity now !";
                    system.logData();
                }
                else if(strcmp(key, "enable_tempt")==0) {
                    bool st = (strcmp(value, "true")==0) ? true: false;
                    if(st)
                        BONE_LOG << "Enable automatic system control temperature !";
                    else
                        BONE_LOG << "Disable automatic system control temperature !";
                    system.enableControlProperty(key, st);
                }
                else if(strcmp(key, "enable_humy")==0) {
                    bool st = (strcmp(value, "true")==0) ? true: false;
                    if(st)
                        BONE_LOG << "Enable automatic system control humidity !";
                    else
                        BONE_LOG << "Disable automatic system control humidity !";
                    system.enableControlProperty(key, st);
                }
    		}
    		else if(strcmp(object, "ventilation")==0) {
    			if(strcmp(key, "state")==0) {
    				system.switchVentilation( (strcmp(value, "true")==0) ? true: false );
    			}
    			else if(strcmp(key, "cycle")==0) {
                    std::vector<int> arr;
                    arr = array_map(value, ",");
                    system.scheduleVentilation(std::vector<uint32_t>(arr.begin(), arr.end()));
                    BONE_LOG << "The ventilation will be opened in " << arr[0] << "s and will be closed in " << arr.back() << "s";
    			}
    			else if(strcmp(key, "enable")==0) {
                    system.enableAutoVentilation( (strcmp(value, "true")==0) ? true: false );
    			}
    		}
		}
	}
    
    // system.thread_schedu_.join();
	return 0;
}

// clean up pin system
void sig_handler(int signo) {
    sys->shutdown();
    exit(1);
}

static int second2hour(int sec) {
    return (sec / 3600) % 24;
}

static std::string printHourSchedule(std::vector<int> v) {
    std::stringstream ss;
    for (uint i = 0; i < v.size() - 1; ++i)
        ss << second2hour(v[i]) << ",";
    ss << second2hour(v.back());
    return ss.str();
}