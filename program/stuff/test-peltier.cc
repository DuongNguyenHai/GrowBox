// g++ -Wall -std=c++11 test-peltier.cc lib/peltier.cpp -o test-peltier

#include <stdio.h>
#include "lib/peltier.h"

Peltier peltier;

int main(int argc, char const *argv[]) {

	printf("Qc at 27: %lf\n", peltier.Qc(27,27));
	printf("Qh at 27: %lf\n", peltier.Qh(27,27));
	// printf("Qcmax at 27: %lf\n", peltier.Qcmax(27));
	return 0;
}