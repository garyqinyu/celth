


#ifndef _MODULATION_M_
#define _MODULATION_M_

ROMDATA PEGCHAR tunerQPSK[]= { 0x0051, 0x0050, 0x0053, 0x004b, 0};
ROMDATA PEGCHAR tuner16QAM[] = { 0x0031, 0x0036, 0x0051, 0x0041, 0x004d, 0};
ROMDATA PEGCHAR tuner32QAM[] = { 0x0033, 0x0032, 0x0051, 0x0041, 0x004d, 0};
ROMDATA PEGCHAR tuner64QAM[] = { 0x0036, 0x0034, 0x0051, 0x0041, 0x004d, 0};
ROMDATA PEGCHAR tuner128QAM[] = { 0x0031, 0x0032, 0x0038, 0x0051, 0x0041, 0x004d, 0};
ROMDATA PEGCHAR tuner256QAM[] = { 0x0032, 0x0035, 0x0036, 0x0051, 0x0041, 0x004d, 0};

ROMDATA PEGCHAR *PegModulation_s[] = {
	tunerQPSK,
	tuner16QAM,
	tuner32QAM,
	tuner64QAM,
	tuner128QAM,
	tuner256QAM
};

#endif



