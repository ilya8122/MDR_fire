#ifndef __CHANNEL_H
#define __CHANNEL_H

#include <stdint.h>
#include <stdbool.h>
typedef enum{
	SIMUL_NONE,
	SIMUL_INPUT,
	SIMUL_OUTPUT
}simulType_s;

typedef struct{
	float scaledMin;
	float scaledMax;
	float inputMin;
	float inputMax;
}calibrKoef_s;

typedef struct{
	float WLag;
	float out;
	bool reset;
}LPF_s;

/*typedef struct{
	float a;
	float b;
}calibrKoef_s;
*/
typedef struct{
	float in_simul;
	float out_simul;
	float in;
	float out;
	calibrKoef_s calibr;
	LPF_s lpf;
	simulType_s simulType;
}channelRd_s;

typedef struct{
	float in;
	float out;
	calibrKoef_s calibr;
}channelWr_s;

void chSetLPF(channelRd_s* ch, float wlag);
void chSetCalibr(channelRd_s* ch, calibrKoef_s* calibr);
void chSetInputRAW(channelRd_s* ch, float val);
void chSetSimulInRAW(channelRd_s* ch, float val);
void chSetSimulIN(channelRd_s* ch, float val);
void chSetSimulType(channelRd_s* ch, simulType_s simulType);
void chProcessing(channelRd_s* ch);
float chGetOut(channelRd_s* ch);
float chGetOutLpf(channelRd_s* ch);
void chLpfReset(channelRd_s* ch);

#endif /* __CHANNEL_H */
