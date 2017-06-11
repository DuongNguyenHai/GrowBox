#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>		/* for setitimer */
#include <unistd.h>		/* for pause */
#include <signal.h>		/* for signal */
#include <iostream>
#include <fstream>
#include <iomanip>
#include "DHT22.h"
#include "DS18B20.h"
using namespace std;

#define GMT 7*60*60
#define UNIX_TIME(time) (time+GMT)
#define INTERVAL 5000 		// >= 5s
#define PIN_PELTIER 22

const uint8_t ds18b_1_rom[8] = {0x28, 0xff, 0xc6, 0x13, 0x50, 0x16, 0x04, 0x1a};
// const uint8_t ds18b_2_rom[8] = {0x28, 0xff, 0x02, 0x08, 0x44, 0x16, 0x03, 0xdc};
const uint8_t ds18b_2_rom[8] = {0x28, 0xff, 0x55, 0xa8, 0xb3, 0x16, 0x04, 0xc5};
// const uint8_t ds18b_3_rom[8] = {0x28, 0xff, 0xbd, 0xb7, 0x44, 0x16, 0x03, 0xf7};

// 0x28, 0xff, 0xc6, 0x13, 0x50, 0x16, 0x04, 0x1a
// 0x28, 0xff, 0x02, 0x08, 0x44, 0x16, 0x03, 0xdc
// 0x28, 0xff, 0xbd, 0xb7, 0x44, 0x16, 0x03, 0xf7
// 0x28, 0xff, 0x55, 0xa8, 0xb3, 0x16, 0x04, 0xc5

const int32_t MAX_TIMES = 6000;

DHT22 dht(27);
DS18B20 ds18b_1(17);
DS18B20 ds18b_2(17);
// DS18B20 ds18b_3(17);

struct count_times {
	int num;
	int dht;
	int ds18b_1;
	int ds18b_2;
	int ds18b_3;
};

ofstream file_time, file_dht_tempt, file_dht_humy, file_ds18b_1, file_ds18b_2;
time_t rawtime;
struct tm *info;
char buffer[50];
float data[2], tempt_1, tempt_2, tempt_3;

bool flag_peltier = true;

struct count_times count = {0};

void reading(void);
void cancel();

int main(int argc, char const *argv[]) {

	
	if (wiringPiSetupGpio() == -1)
		return -1;

	pinMode(PIN_PELTIER, OUTPUT);

	ds18b_1.setAddr(ds18b_1_rom);
	ds18b_2.setAddr(ds18b_2_rom);

	file_time.open("/var/www/html/tempt-graph/time.txt", std::ofstream::out | std::ofstream::app);

	if(!file_time.is_open()) {
		printf("Unable to open file\n");
	}

	file_dht_tempt.open("/var/www/html/tempt-graph/tempt_box.txt", std::ofstream::out | std::ofstream::app);
	if(!file_dht_tempt.is_open()) {
		printf("Unable to open file\n");
	}

	file_dht_humy.open("/var/www/html/tempt-graph/humidity_box.txt", std::ofstream::out | std::ofstream::app);
	if(!file_dht_humy.is_open()) {
		printf("Unable to open file\n");
	}

	file_ds18b_1.open("/var/www/html/tempt-graph/tempt_in.txt", std::ofstream::out | std::ofstream::app);
	if(!file_ds18b_1.is_open()) {
		printf("Unable to open file\n");
	}
	
	file_ds18b_2.open("/var/www/html/tempt-graph/tempt_out.txt", std::ofstream::out | std::ofstream::app);
	if(!file_ds18b_2.is_open()) {
		printf("Unable to open file\n");
	}

	if (signal(SIGINT, (void (*)(int)) cancel) == SIG_ERR) {
		perror("Unable to catch SIGINT");
		return -1;
	}

	reading(); // the first time
	
	struct itimerval it_val;
	if (signal(SIGALRM, (void (*)(int)) reading) == SIG_ERR) {
		perror("Unable to catch SIGALRM");
		exit(1);
	}
	it_val.it_value.tv_sec = INTERVAL/1000;
	it_val.it_value.tv_usec = (INTERVAL*1000) % 1000000;	
	it_val.it_interval = it_val.it_value;

	if (setitimer(ITIMER_REAL, &it_val, NULL) == -1) {
		perror("error calling setitimer()");
		exit(1);
	}

	while (1) 
    	pause();

	return 0;
}

void reading() {

	time(&rawtime);
	file_time << UNIX_TIME(rawtime) << ",";
	info = localtime(&rawtime);
	strftime(buffer, sizeof(buffer),"%X", info);
	printf("Time |%s|\n", buffer);
	file_time.flush();

	if(dht.read(data)==false) {
		count.dht++;
		printf("DHT22 read failed !\n");
	} else {
		printf("DHT22 temperature: %.1f *C, humidity: %.1f %%\n", data[0], data[1]);
		file_dht_tempt << fixed << setprecision(2) << data[0] << ",";
		file_dht_tempt.flush();
		file_dht_humy << fixed << setprecision(2) << data[1] << ",";
		file_dht_humy.flush();
	}

	tempt_1 = ds18b_1.tempt();
	tempt_2 = ds18b_2.tempt();

	if(tempt_1==VAL_FAIL) {
		count.ds18b_1++;
		printf("DS18B20 [28-ffc613501604] read failed !\n");
	} else {
		printf("DS18B20 [28-ffc613501604] temperature: %.1f\n", tempt_1);
		file_ds18b_1 << fixed << setprecision(2) << tempt_1 << ",";
		file_ds18b_1.flush();
	}

	if(tempt_2==VAL_FAIL) {
		count.ds18b_2++;
		printf("DS18B20 [28-ffbdb7441603] read failed !\n");
	} else {
		printf("DS18B20 [28-ffbdb7441603] temperature: %.1f\n", tempt_2);
		file_ds18b_2 << fixed << setprecision(2) << tempt_2 << ",";
		file_ds18b_2.flush();
	}

	// control tempt in box
	// test cold
	
	if(data[0]>40) {
		digitalWrite(PIN_PELTIER, HIGH);
	} else {
		if(tempt_1 < (data[0]-10)){
			digitalWrite(PIN_PELTIER, HIGH);
			flag_peltier = false;
		}
		else if(tempt_1 < (tempt_2-30) ){
			digitalWrite(PIN_PELTIER, HIGH);
		} else {
			digitalWrite(PIN_PELTIER, LOW);
		}
	}

	// test hot


	count.num++;
	printf("-----------------------------------------------\n");
	// delay(1000);

}

void cancel() {
	printf("RESULT: Total reading %d times\n"
				"-\tDHT22 failed : %d times\n"
				"-\tDS18B20 [28-ffc613501604] failed : %d times\n"
				"-\tDS18B20 [28-ff0208441603] failed : %d times\n", count.num, count.dht, count.ds18b_1, count.ds18b_2);

	pinMode(PIN_PELTIER, INPUT);

	file_time.close();
	file_dht_tempt.close();
	file_dht_humy.close();
	file_ds18b_1.close();
	file_ds18b_2.close();
	exit(1);
}