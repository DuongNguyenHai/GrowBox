#include "peltier.h"

#define Kelvin(tempt) tempt + 273.15

Peltier::Peltier() {}
Peltier::~Peltier() {}

static double seebeck(uint8_t type, float Th, float Tc) {

	switch(type) {

		case SEB_TM: {
			if(Th==Tc)
				return (S1 + S2*Th + S3*Th*Th + S4*Th*Th*Th)*COUPLE/71;
			else
				return (S1*(Th-Tc) + S2*(Th*Th-Tc*Tc)/2 + S3*(Th*Th*Th-Tc*Tc*Tc)/3 + S4*(Th*Th*Th*Th-Tc*Tc*Tc*Tc)/4)*(Th-Tc)*COUPLE/71;
		} break;

		case SEB_RM: {
			if(Th==Tc)
				return (R1 + R2*Th + R3*Th*Th + R4*Th*Th*Th)*6/I*COUPLE/71;
			else
				return (R1*(Th-Tc) + R2*(Th*Th-Tc*Tc)/2 + R3*(Th*Th*Th-Tc*Tc*Tc)/3 + R4*(Th*Th*Th*Th-Tc*Tc*Tc*Tc)/4)*(Th-Tc)*6/I*COUPLE/71;
		} break;

		case SEB_KM: {
			if(Th==Tc)
				return (K1 + K2*Th + K3*Th*Th + K4*Th*Th*Th)*I/6*COUPLE/71;
			else
				return (K1*(Th-Tc) + K2*(Th*Th-Tc*Tc)/2 + K3*(Th*Th*Th-Tc*Tc*Tc)/3 + K4*(Th*Th*Th*Th-Tc*Tc*Tc*Tc)/4)*(Th-Tc)*I/6*COUPLE/71;
		}

	}

	return 0;
}

void Peltier::begin(float tempt_hot, float tempt_cold) {
	tempt_hot_ = tempt_hot;
	tempt_cold_ = tempt_cold;
}

double Peltier::Qc(float tempt_hot, float tempt_cold) {

	double Sm, Rm, Km;
	float Th = Kelvin(tempt_hot);
	float Tc = Kelvin(tempt_cold);
	float DT;
	if((int)tempt_hot==(int)tempt_cold) {
		Sm = seebeck(SEB_TM, (int)Th, (int)Tc);
		Rm = seebeck(SEB_RM, (int)Th, (int)Tc);
		// Km = seebeck(3, (int)Th, (int)Tc);
		DT = 0;
		// printf("Sm = %lf\n", Sm);
		// printf("Rm = %lf\n", Rm);
	} else {
		Sm = seebeck(SEB_TM, Th, Tc);
		Rm = seebeck(SEB_RM, Th, Tc);
		Km = seebeck(SEB_KM, Th, Tc);
	}

	return (Sm*Tc*I) - (0.5*I*I*Rm) - (Km*DT);
}


double Peltier::Qh(float tempt_hot, float tempt_cold) {
	// return U*I + Qc(tempt_hot, tempt_cold);

	double Sm, Rm, Km;
	float Th = Kelvin(tempt_hot);
	float Tc = Kelvin(tempt_cold);
	float DT;
	if((int)tempt_hot==(int)tempt_cold) {
		Sm = seebeck(SEB_TM, (int)Th, (int)Tc);
		Rm = seebeck(SEB_RM, (int)Th, (int)Tc);
		// Km = seebeck(3, (int)Th, (int)Tc);
		DT = 0;
		// printf("Sm = %lf\n", Sm);
		// printf("Rm = %lf\n", Rm);
	} else {
		Sm = seebeck(SEB_TM, Th, Tc);
		Rm = seebeck(SEB_RM, Th, Tc);
		Km = seebeck(SEB_KM, Th, Tc);
	}

	return (Sm*Tc*I) + (0.5*I*I*Rm) - (Km*DT);
}

double Peltier::Qcmax(float tempt) {
	float T = Kelvin(tempt);
	double Sm = seebeck(SEB_TM, T, T);
	double Rm = seebeck(SEB_RM, T, T);
	return (Sm*Sm * T*T) / (2*Rm);
}