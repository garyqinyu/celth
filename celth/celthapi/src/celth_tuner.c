
#include <stdlib.h>
#include <stdio.h>
#include <errno.h> 
#include <pthread.h>

#include "celth_stddef.h"
#include "celth_tuner.h"

#include "celth_debug.h"
#include "nexus_platform.h"
#include "nexus_frontend.h"


NEXUS_FrontendHandle userfrontend=NULL;

static Celth_FrontendBand  globalband = Celth_Band_Invalid ;  


typedef struct CELTH_Frontend_Local_s{


BKNI_EventHandle hfrontendEvent;


pthread_t frontendthread;

bool frontendmonitorterminate;

Celth_Demod localdemodcontext;


	
}CELTH_Frontend_Local_s;


static CELTH_Frontend_Local_s  localfrontend; 
 




static void FrondendLock_Callback(void *context, int param)
{

	BKNI_SetEvent((BKNI_EventHandle)context);


}



static void *
frontendmonitor_thread(void *Local_)
{
	
	CELTH_Frontend_Local_s* local;

	local=(CELTH_Frontend_Local_s*)Local_;


	while(!local->frontendmonitorterminate)
	{

        BKNI_WaitForEvent(local->hfrontendEvent,-1); /* wait forever */
		if(local->localdemodcontext.fstatusfun)
			{
				local->localdemodcontext.fstatusfun(local->localdemodcontext.iParam,local->localdemodcontext.pParam);
			}
		  

	}
}




static int
frontendmonitor_thread_start(CELTH_Frontend_Local_s *local)
{
    int rc;
   local->frontendmonitorterminate=false;
    rc = pthread_create(&local->frontendthread, NULL, frontendmonitor_thread, local);
    return rc;
}

static void
frontendmonitor_thread_stop(CELTH_Frontend_Local_s *local)
{
    local->frontendmonitorterminate = true;
    pthread_join(local->frontendthread, NULL);
}



int CelthApi_TunerInit(Celth_DemodType type)
{

    NEXUS_PlatformConfiguration platformConfig;
	
	NEXUS_FrontendAcquireSettings settings;

	NEXUS_FrontendHandle frontend=NULL;
	
	
     
	int i;
	int ret = 1;
	
	unsigned char bFrontend = 0;

	
    

	/*
	
	NEXUS_Platform_GetConfiguration(&platformConfig);

    for ( i = 0; i < NEXUS_MAX_FRONTENDS; i++ )
    {
        NEXUS_FrontendCapabilities capabilities;
        frontend = platformConfig.frontend[i];
        if (frontend) {
            NEXUS_Frontend_GetCapabilities(frontend, &capabilities);
            if ( capabilities.qam )
            {
			
			    bFrontend=1;
				userfrontend = frontend;
                break;
            }
        }
    }
    */

	

    NEXUS_Frontend_GetDefaultAcquireSettings(&settings);

	if(type==CELTH_D_QAM)
		{
		
		settings.capabilities.qam = true;
		}
	else if(type==CELTH_D_OFDM)
		{
    settings.capabilities.ofdm = true;
		}
	else
		{
		CelthApi_Debug_Wrn("no suitable demod type,use qam!\n");
		settings.capabilities.qam = true;
		}
    frontend = NEXUS_Frontend_Acquire(&settings);
    if (!frontend) {
        CelthApi_Debug_Err("Unable to find %d type frontend!\n",type);

		ret=1;
        
    }
	else
		{
		userfrontend = frontend;
		bFrontend=1;
		}


	
	
	if(bFrontend==1)
	{

	CelthApi_Debug_Msg("Get tune, the type is %d!\n",type);
	localfrontend.localdemodcontext.DemodType = type;

	CelthApi_TunerSetDemodType(type);
	/*localfrontend.localdemodcontext.DemodParam.Celth_Qam.annextype = CELTH_D_ANNEX_A;
	localfrontend.localdemodcontext.DemodParam.Celth_Qam.qammode = CELTH_D_QAM64;
	localfrontend.localdemodcontext.DemodParam.Celth_Qam.symbolrate = 6875000;*/
	localfrontend.localdemodcontext.freq = 702000000;

	localfrontend.localdemodcontext.fstatusfun=NULL;
	
	ret = 0;

	BKNI_CreateEvent(&localfrontend.hfrontendEvent);

	frontendmonitor_thread_start(&localfrontend);
	
	}
	
	return ret;

}

void CelthApi_TunerSetFreq(unsigned int freq)
{
	localfrontend.localdemodcontext.freq  = freq;
}

void CelthApi_TunerSetDemodParam(Celth_Demod demodparam)
{

	localfrontend.localdemodcontext.DemodType = demodparam.DemodType;
	
	if(localfrontend.localdemodcontext.DemodType == CELTH_D_QAM)
	{
	localfrontend.localdemodcontext.DemodParam.Celth_Qam.annextype = demodparam.DemodParam.Celth_Qam.annextype;
	localfrontend.localdemodcontext.DemodParam.Celth_Qam.qammode = demodparam.DemodParam.Celth_Qam.qammode;
	localfrontend.localdemodcontext.DemodParam.Celth_Qam.symbolrate = demodparam.DemodParam.Celth_Qam.symbolrate;
	}
	else if(localfrontend.localdemodcontext.DemodType == CELTH_D_OFDM)
	{

	localfrontend.localdemodcontext.DemodParam.Celth_Ofdm.bandwith = demodparam.DemodParam.Celth_Ofdm.bandwith;
	localfrontend.localdemodcontext.DemodParam.Celth_Ofdm.ofdmmode = demodparam.DemodParam.Celth_Ofdm.ofdmmode;
	

	}
	else
	{
		CelthApi_Debug_Wrn("the tuner demod type is not implement yet, it is %d\n",localfrontend.localdemodcontext.DemodType);
	}
	
	localfrontend.localdemodcontext.freq = demodparam.freq;
	
	return;

}

void CelthApi_TunerSetDemodType(Celth_DemodType demodtype)
{
localfrontend.localdemodcontext.DemodType = demodtype;
}

Celth_DemodType CelthApi_TunerGetDemodType()
{
   return localfrontend.localdemodcontext.DemodType;   
}

void CelthApi_TunerGetDemodParam(Celth_Demod* pdemodparam)
{
  pdemodparam = &localfrontend.localdemodcontext;
}

void CelthApi_TunerSetCallback(CELTHTuner_Callback_t fcallbackfun)
{

	localfrontend.localdemodcontext.fstatusfun=fcallbackfun;
}

void CelthApi_TunerSetCallbackParams(CELTH_INT iParam,CELTH_VOID* pParam)
{

	
	localfrontend.localdemodcontext.iParam=iParam;
	localfrontend.localdemodcontext.pParam=pParam;

}



Celth_FrontendBand CelthApi_TunerTune()
{

   
    NEXUS_FrontendQamSettings qamSettings;

	
    NEXUS_FrontendOfdmSettings ofdmSettings;
    NEXUS_FrontendOfdmMode mode = NEXUS_FrontendOfdmMode_eDvbt;
	
	NEXUS_FrontendUserParameters userParams;

	Celth_DemodType t;
	
	Celth_FrontendBand  band = Celth_Band_Invalid;

	globalband = Celth_Band_Invalid;
	
	if(userfrontend!=NULL)
	{

		t=CelthApi_TunerGetDemodType();

		CelthApi_Debug_Msg("the tuner type=%d\n",t);	

		if(t==CELTH_D_QAM)
			{
		NEXUS_Frontend_GetDefaultQamSettings(&qamSettings);
  		qamSettings.frequency = localfrontend.localdemodcontext.freq ;
    	qamSettings.mode = localfrontend.localdemodcontext.DemodParam.Celth_Qam.qammode;
    	qamSettings.symbolRate = localfrontend.localdemodcontext.DemodParam.Celth_Qam.symbolrate;
    	qamSettings.annex = NEXUS_FrontendQamAnnex_eA;
    	qamSettings.bandwidth = NEXUS_FrontendQamBandwidth_e8Mhz;
    	qamSettings.lockCallback.callback = FrondendLock_Callback;
    	qamSettings.lockCallback.context = localfrontend.hfrontendEvent;
	
	
		NEXUS_Frontend_GetUserParameters(userfrontend, &userParams);
	
		/*	band =  userParams.param1;*/

		CelthApi_Debug_Msg("the tuner freq=%d,symbol =%d,\n",qamSettings.frequency,qamSettings.symbolRate);	
	
		
		NEXUS_Frontend_TuneQam(userfrontend, &qamSettings);
			}
		else if(t==CELTH_D_OFDM)
			{


				
			NEXUS_Frontend_GetDefaultOfdmSettings(&ofdmSettings);
			
			

				
			
			ofdmSettings.bandwidth = localfrontend.localdemodcontext.DemodParam.Celth_Ofdm.bandwith;
			ofdmSettings.manualTpsSettings = false;
			ofdmSettings.pullInRange = NEXUS_FrontendOfdmPullInRange_eWide;
			ofdmSettings.cciMode = NEXUS_FrontendOfdmCciMode_eNone;

			ofdmSettings.frequency = localfrontend.localdemodcontext.freq*1000000;

			
			NEXUS_Frontend_GetUserParameters(userfrontend, &userParams);

			
			CelthApi_Debug_Msg("the tuner freq=%d,bandwith=%d,\n",ofdmSettings.frequency,ofdmSettings.bandwidth);

		NEXUS_Frontend_TuneOfdm(userfrontend, &ofdmSettings);	
			
			}
			else
			{

				CelthApi_Debug_Err("no suitable demod to lock!!!\n");
			}

	
	}
	{

		if(t==CELTH_D_QAM)
			{
 		NEXUS_FrontendQamStatus qamStatus;
 		unsigned count=0;
 		NEXUS_Frontend_GetQamStatus(userfrontend,&qamStatus);

		while(qamStatus.fecLock==false || count<=20)
		{
  			count++;
  			BKNI_Sleep(10);


		}

 		if(qamStatus.fecLock)
		{
  			band=userParams.param1;

  			globalband = userParams.param1;
		CelthApi_Debug_Msg("user band is [%x]\n", userParams.param1);

		}
			}
		else if(t==CELTH_D_OFDM)
			{
			
	/*		NEXUS_FrontendDvbtStatus dvbtStatus;
			unsigned count=0;
			NEXUS_Frontend_GetDvbtStatus(userfrontend,&dvbtStatus);*/
		band=userParams.param1;

  			globalband = userParams.param1;
		CelthApi_Debug_Msg("user band is [%x]\n", userParams.param1);

	
			}
		else
			{
			
			CelthApi_Debug_Err("user band is Null\n");
			}
	}

	CelthApi_Debug_Msg("return band is [%x]\n",band);
	return band;

}




Celth_FrontendBand  CelthApi_GetFrontendBandFromTune()
{

	return globalband;

}

void CelthApi_TunerGetTunerStatus(Celth_TunerStatus* pstatus)
{
    NEXUS_FrontendQamStatus qamStatus;

	
	Celth_DemodType t;

	
	t=CelthApi_TunerGetDemodType();

	if(userfrontend!=NULL)
	{
	if(t==CELTH_D_QAM)
		{
    NEXUS_Frontend_GetQamStatus(userfrontend,&qamStatus);
	pstatus->lockstate = qamStatus.fecLock;
		}
	
	}
}

