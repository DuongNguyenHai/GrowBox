// g++ -std=c++11 test-light.cc lib/base/BH1750.cpp lib/base/Wire.cpp -o test-light

#include <stdio.h>
#include <time.h>
#include "lib/base/BH1750.h"

#define DELAY_TIME 1

BH1750 lightMeter;

int main(int argc, char const *argv[]) {

	struct timespec start, end;
   	uint32_t elasped_time_ms;

	lightMeter.begin();
		
	while(1) {

		clock_gettime(CLOCK_MONOTONIC_RAW, &start);

		int lux = lightMeter.readLightLevel();
		// its too fast. so add 1 second to slow monitor
		sleep(DELAY_TIME);

		clock_gettime(CLOCK_MONOTONIC_RAW, &end);

		printf("BH1750 light meter: %d\n", lux);

   		elasped_time_ms = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_nsec - start.tv_nsec) / 1000000;
   		printf("Execution time = %u ms\n", elasped_time_ms - DELAY_TIME*1000);
	}

	return 0;
}