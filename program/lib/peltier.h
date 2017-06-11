
#ifndef PELTIER_H
#define PELTIER_H

#include <stdio.h>
#include <stdint.h>

#define S1 1.33450e-2
#define S2 -5.37574e-5
#define S3 7.42731e-7
#define S4 -1.27141e-9

#define R1 2.08317
#define R2 -1.98763e-2
#define R3 8.53832e-5
#define R4 -9.03143e-8

#define K1 4.76218e-1
#define K2 -3.89821e-6
#define K3 -8.64864e-6
#define K4 2.20869e-8

#define COUPLE 127
#define U 16
#define I 15

enum Seebeck {
	SEB_TM = 1,
	SEB_RM = 2, 
	SEB_KM = 3
};

class Peltier {

public:
	Seebeck Seb;
	Peltier();
	~Peltier();
	void begin(float tempt_hot, float tempt_cold);
	double Qc(float tempt_hot, float tempt_cold);
	double Qh(float tempt_hot, float tempt_cold);
	double Qcmax(float tempt);
private:
	float tempt_hot_;
	float tempt_cold_;
};


#endif
