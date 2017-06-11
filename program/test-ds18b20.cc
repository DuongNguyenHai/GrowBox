// g++ -std=c++11 test-ds18b20.cc lib/base/OneWire.cpp lib/base/DS18B20.cpp lib/base/Wire.cpp -o test-ds18b20 -lwiringPi

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>	
#include <time.h>
#include "lib/base/DS18B20.h"

#define DELAY_TIME 1

const uint8_t ds18b_1_rom[8] = {0x28, 0xff, 0xc6, 0x13, 0x50, 0x16, 0x04, 0x1a};
const uint8_t ds18b_2_rom[8] = {0x28, 0xff, 0x02, 0x08, 0x44, 0x16, 0x03, 0xdc};

struct count_times {
	int num;
	int dht;
	int ds18b_1;
	int ds18b_2;
};

void cancel();

struct count_times count = {0};

DS18B20 ds18b_1(17);
DS18B20 ds18b_2(17);

int main(int argc, char const *argv[]) {
	
	struct timespec start, end;
   	uint32_t elasped_time_ms;
   	float tempt_1, tempt_2;
	
	if (wiringPiSetupGpio() == -1)
		return -1;

	if (signal(SIGINT, (void (*)(int)) cancel) == SIG_ERR) {
		perror("Unable to catch SIGINT");
		return -1;
	}

	ds18b_1.setAddr(ds18b_1_rom);
	ds18b_2.setAddr(ds18b_2_rom);

	ds18b_1.resolution(11);
	ds18b_2.resolution(11);

	while(1) {

		clock_gettime(CLOCK_MONOTONIC_RAW, &start);

		tempt_1 = ds18b_1.tempt();
		tempt_2 = ds18b_2.tempt();
		// its too fast. so add 1 second to slow monitor
		sleep(DELAY_TIME);

		clock_gettime(CLOCK_MONOTONIC_RAW, &end);

		if(tempt_1==VAL_FAIL) {
			count.ds18b_1++;
			printf("DS18B20 [28-ffc613501604] read failed !\n");
		} else {
			printf("DS18B20 [28-ffc613501604] temperature: %.1f °C\n", tempt_1);
		}

		if(tempt_2==VAL_FAIL) {
			count.ds18b_2++;
			printf("DS18B20 [28-ff0208441603] read failed !\n");
		} else {
			printf("DS18B20 [28-ff0208441603] temperature: %.1f °C\n", tempt_2);
		}

   		elasped_time_ms = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_nsec - start.tv_nsec) / 1000000;
   		printf("Execution time = %u ms\n", elasped_time_ms - DELAY_TIME*1000);
		count.num++;
	}

	return 0;
}

void cancel() {
	printf("RESULT: Total reading %d times\n"
				"-\tDS18B20 [28-ffc613501604] failed : %d times\n"
				"-\tDS18B20 [28-ff0208441603] failed : %d times\n", count.num, count.ds18b_1, count.ds18b_2);
	exit(1);
}