
#include <stdio.h>
#include <stdlib.h>



#include "nexus_base.h"
#include "celth_stddef.h"
#include "celth_timer.h"
/*
#include "celth_timer_module.h"*/
/*#include "celth_timer.h"*/



#ifndef NEXUS_MODULE_SELF

#define NEXUS_MODULE_SELF g_NEXUS_astmModule
#endif

extern NEXUS_ModuleHandle g_NEXUS_astmModule;

/*
NEXUS_ModuleHandle g_NEXUS_celthtimerModule;*/

typedef struct Celthtimelocal{

Celth_TimerInfo_t timeinfo;

NEXUS_TimerHandle  nexustimehandle;


}Celthtimelocal;


CELTH_Status_t CELTH_TimerCreate	( CELTH_TimerId_t* pTimerId)
{
	CELTH_Status_t	Status	= CELTH_NO_ERROR;

	Celthtimelocal*	pTimerInfo	= NULL;

	pTimerInfo	= (Celthtimelocal *)CELTHOSL_MemoryAlloc(sizeof(Celthtimelocal));
	if( pTimerInfo != NULL )
	{
		pTimerInfo->timeinfo.pfClientCallBack	= NULL;
		pTimerInfo->timeinfo.iTimeValue=0xFFFFFFFF;
	}
	else
	{
		Status	= CELTH_ERROR_NO_MEMORY;
	}
	*pTimerId	= (CELTH_TimerId_t)pTimerInfo;

	return Status;


}
CELTH_Status_t CELTH_TimerConfigure	( CELTH_TimerId_t TimerId, CELTH_TimerExpire_t pfClientCallBack )
{

		CELTH_Status_t	Status	= CELTH_NO_ERROR;
	
		Celthtimelocal*	pTimerInfo	= (Celthtimelocal *)TimerId;
	
		pTimerInfo->timeinfo.pfClientCallBack	= pfClientCallBack;
	
		return Status;

}
CELTH_Status_t CELTH_TimerStart		( CELTH_TimerId_t TimerId, CELTH_VOID* pClientData, CELTH_INT iTimerValue )
{
	
	CELTH_Status_t	Status	= CELTH_NO_ERROR;
	Celthtimelocal*	pTimerInfo	= (Celthtimelocal *)TimerId;
	pTimerInfo->timeinfo.iTimeValue=iTimerValue;

	pTimerInfo->timeinfo.pClientData = pClientData;
	
	pTimerInfo->nexustimehandle=NEXUS_ScheduleTimer(iTimerValue,pTimerInfo->timeinfo.pfClientCallBack,pTimerInfo->timeinfo.pClientData);

	return Status;
}
CELTH_Status_t CELTH_TimerStop      ( CELTH_TimerId_t TimerId )
{
	CELTH_Status_t	Status	= CELTH_NO_ERROR;
	Celthtimelocal*	pTimerInfo	= (Celthtimelocal *)TimerId;
	
	NEXUS_CancelTimer(pTimerInfo->nexustimehandle);

	return Status;

}



CELTH_Status_t CELTH_TimerInit(CELTH_VOID* pContext )
{
/*	g_NEXUS_celthtimerModule=NEXUS_Module_Create("celthtimer",NULL);*/

	return CELTH_NO_ERROR;
}

CELTH_Status_t CELTH_TimerUninit()
{
/*	NEXUS_Module_Destroy(g_NEXUS_celthtimerModule);*/
	return CELTH_NO_ERROR;
}



