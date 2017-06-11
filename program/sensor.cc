#include <stdio.h>
#include <sys/time.h>
#include <signal.h>	
#include <stdlib.h>
#include <time.h>
#include "DHT22.h"
#include "DS18B20.h"
#include "BH1750.h"

#define DELAY_TIME 1

const uint8_t ds18b_1_rom[8] = {0x28, 0xff, 0xc6, 0x13, 0x50, 0x16, 0x04, 0x1a};
const uint8_t ds18b_2_rom[8] = {0x28, 0xff, 0x02, 0x08, 0x44, 0x16, 0x03, 0xdc};
// const uint8_t ds18b_2_rom[8] = {0x28, 0xff, 0x55, 0xa8, 0xb3, 0x16, 0x04, 0xc5};
// const uint8_t ds18b_3_rom[8] = {0x28, 0xff, 0xbd, 0xb7, 0x44, 0x16, 0x03, 0xf7};

// 0x28, 0xff, 0xc6, 0x13, 0x50, 0x16, 0x04, 0x1a
// 0x28, 0xff, 0x02, 0x08, 0x44, 0x16, 0x03, 0xdc
// 0x28, 0xff, 0xbd, 0xb7, 0x44, 0x16, 0x03, 0xf7
// 0x28, 0xff, 0x55, 0xa8, 0xb3, 0x16, 0x04, 0xc5

void cancel();

DHT22 dht(27);
DS18B20 ds18b_1(17);
DS18B20 ds18b_2(17);
BH1750 lightMeter;

struct count_times {
	int num;
	int dht;
	int ds18b_1;
	int ds18b_2;
	int ds18b_3;
};

struct count_times count = {0};

int main(int argc, char const *argv[]) {

	int lux;
	bool res_dht;
	float data[2], tempt_1, tempt_2;
	struct timespec start, end;
   	uint32_t elasped_time_ms;

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

	lightMeter.begin();

	while (1) {

		printf("Test %d: \n", count.num+1);

		clock_gettime(CLOCK_MONOTONIC_RAW, &start);

		res_dht = dht.read(data);
		tempt_1 = ds18b_1.tempt();
		tempt_2 = ds18b_2.tempt();
		lux = lightMeter.readLightLevel();

		clock_gettime(CLOCK_MONOTONIC_RAW, &end);

		if(res_dht==false) {
			count.dht++;
			printf("DHT22 read failed !\n");
		}
		else
			printf("DHT22 temperature: %.1f *C, humidity: %.1f %%\n", data[0], data[1]);

		if(tempt_1==VAL_FAIL) {
			count.ds18b_1++;
			printf("DS18B20 [28-04*] read failed !\n");
		}
		else
			printf("DS18B20 [28-04*] temperature: %.1f\n", tempt_1);

		if(tempt_2==VAL_FAIL) {
			count.ds18b_2++;
			printf("DS18B20 [28-03*] read failed !\n");
		}
		else
			printf("DS18B20 [28-03*] temperature: %.1f\n", tempt_2);

		
		printf("BH1750 light meter: %d\n", lux);

		elasped_time_ms = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_nsec - start.tv_nsec) / 1000000;
   		printf("Execution time = %u ms\n", elasped_time_ms);
   		
		count.num++;
	}
	
	return 0;
}


void cancel() {
	printf("RESULT: Total reading %d times\n"
				"-\tDHT22 failed : %d times\n"
				"-\tDS18B20 [28-ffc613501604] failed : %d times\n"
				"-\tDS18B20 [28-ff0208441603] failed : %d times\n", count.num, count.dht, count.ds18b_1, count.ds18b_2);
	exit(1);
}