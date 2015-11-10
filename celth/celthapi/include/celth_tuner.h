#ifndef __CELTH_TUNER_H
#define __CELTH_TUNER_H

#ifdef __cplusplus

extern "C" {

#endif


typedef enum Celth_DemodType{
CELTH_D_QAM,
CELTH_D_QPSK,
CELTH_D_VSB,
CELTH_D_OFDM
}Celth_DemodType;

typedef enum Celth_QamType{
CELTH_D_QAM16,
CELTH_D_QAM32,
CELTH_D_QAM64,
CELTH_D_QAM256,
CELTH_D_QAM1024
}Celth_QamType;

typedef enum Celth_QamAnnexType{
CELTH_D_ANNEX_A,
CELTH_D_ANNEX_B

}Celth_QamAnnexType;

typedef struct Celth_Demod_Qam_Param
{
 unsigned int symbolrate;
 Celth_QamType qammode;
 Celth_QamAnnexType annextype;
 
}Celth_Demod_Qam_Param;

typedef struct Celth_Demod_Qpsk_Param
{
}Celth_Demod_Qpsk_Param;

typedef struct Celth_Demod_Vsb_Param
{
}Celth_Demod_Vsb_Param;

typedef enum Celth_Demod_Ofdm_Mode{
	
    Celth_OfdmMode_eDvbt,       /* DVB-T */
    Celth_OfdmMode_eDvbt2,      /* DVB-T2 */
    Celth_OfdmMode_eDvbc2,      /* DVB-C2 */
    Celth_OfdmMode_eIsdbt,      /* ISDB-T */
    Celth_OfdmMode_eDtmbt,      /*china DVB-T*/
    Celth_OfdmMode_eMax
}Celth_Demod_Ofdm_Mode;

typedef struct Celth_Demod_Ofdm_Param
{
unsigned int bandwith;
Celth_Demod_Ofdm_Mode  ofdmmode;

}Celth_Demod_Ofdm_Param;





typedef enum Celth_Tuner_State{

CELTH_TUNER_UNLOCKED = 0,
CELTH_TUNER_LOCKED

}Celth_Tuner_State;

typedef struct Celth_TunerStatus{

Celth_Tuner_State lockstate;

}Celth_TunerStatus;




typedef	CELTH_VOID (*CELTHTuner_Callback_t)( CELTH_INT iParam, CELTH_VOID* pParam);




typedef struct Celth_Demod {

Celth_DemodType DemodType;

union {

Celth_Demod_Qam_Param Celth_Qam;
Celth_Demod_Qpsk_Param Celth_Qpsk;
Celth_Demod_Vsb_Param  Celth_Vsb;
Celth_Demod_Ofdm_Param Celth_Ofdm;

}DemodParam;

unsigned int freq;

CELTHTuner_Callback_t  fstatusfun;

CELTH_INT    iParam;

CELTH_VOID*  pParam;


}Celth_Demod;




int CelthApi_TunerInit(Celth_DemodType type);

void CelthApi_TunerSetFreq(unsigned int freq);

void CelthApi_TunerSetDemodParam(Celth_Demod demodparam);

void CelthApi_TunerSetDemodType(Celth_DemodType demodtype);

Celth_DemodType CelthApi_TunerGetDemodType();

void CelthApi_TunerGetDemodParam(Celth_Demod* pdemodparam);

void CelthApi_TunerSetCallback(CELTHTuner_Callback_t fcallbackfun);

void CelthApi_TunerSetCallbackParams(CELTH_INT iParam,CELTH_VOID* pParam);

Celth_FrontendBand CelthApi_TunerTune();

void CelthApi_TunerGetTunerStatus(Celth_TunerStatus* pstatus);

Celth_FrontendBand  CelthApi_GetFrontendBandFromTune();



#ifdef __cplusplus

}

#endif






#endif
