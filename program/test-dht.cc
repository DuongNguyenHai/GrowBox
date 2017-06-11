// g++ -std=c++11 test-dht.cc lib/base/DHT22.cpp -o test-dht -lwiringPi

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>	
#include <time.h>
#include "lib/base/DHT22.h"

#define DELAY_TIME 1  // unit: second

struct count_times {
	int num;
	int dht;
	int ds18b_1;
	int ds18b_2;
};

void cancel();

struct count_times count = {0};

DHT22 dht(27);

int main(int argc, char const *argv[]) {

	struct timespec start, end;
   	uint32_t elasped_time_ms;
   	float data[2];

	if (wiringPiSetupGpio() == -1)
		return -1;

	if (signal(SIGINT, (void (*)(int)) cancel) == SIG_ERR) {
		perror("Unable to catch SIGINT");
		return -1;
	}

	while(1) {
		
		clock_gettime(CLOCK_MONOTONIC_RAW, &start);

		bool res = dht.read(data);
		sleep(DELAY_TIME);

		clock_gettime(CLOCK_MONOTONIC_RAW, &end);

		if(res==false) {
			count.dht++;
			printf("DHT22 read failed !\n");
		}
		else
			printf("DHT22 temperature: %.1f *C, humidity: %.1f %%\n", data[0], data[1]);

   		elasped_time_ms = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_nsec - start.tv_nsec) / 1000000;
   		printf("Execution time = %u ms\n", elasped_time_ms - DELAY_TIME*1000);

   		count.num++;
	}

	return 0;
}

void cancel() {
	printf("RESULT: Total reading %d times\n"
				"-\tDHT22 failed : %d times\n", count.num, count.dht);
	exit(1);
}