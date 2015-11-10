#include "celth_stddef.h"

#include "celth_rtos.h"

#include "celth_mutex.h"
#include "celth_message.h"

#include "celth_demux.h"
#include "celth_event.h"

#include "celth_symbol.h"


#include "celth_debug.h"

#include "gbemg_stddef.h"
#include "gbemg_gendef.h"

#include "gbemg_type.h"
#include "gbemg_index.h"
#include "gbemg_content.h"
#include "gbemg_control.h"




#include "gbemg_configurefilter.h"



static filtermonitorparam_t indexfilterparam;


static void gbemg_indexsection_detect(int iparam,void* pparam)
{
	CELTH_EventId_t  event = (CELTH_EventId_t)pparam;
	printf("\n+++++++detect indx section+++++++++++++++++\n");
	CELTH_SetEvent(event);
		
}



filtermonitorparam_t* gbemg_begin_monitorindextable()
{


Celth_Demux_SectionFilter_s  indxFilterSettings;


Celth_DemuxSectionCallback_s indxSectionCallbackfun;



indxSectionCallbackfun.fDetectCallback=gbemg_indexsection_detect;


CelthApi_Demux_AllocateSectionFilter(NULL,&indexfilterparam.hf);

CELTH_CreateEvent(&indexfilterparam.ev);


if(indexfilterparam.hf)
{

	CelthApi_Demux_SetSectionFilterPid(indexfilterparam.hf,GBEMG_PID);		


	CelthApi_Demux_SectionSetDefaultFilterSettings(&indxFilterSettings);
					
	indxFilterSettings.mask[0]=0x00;


	indxFilterSettings.coefficient[0]=GBEMG_INDX_TABLEID;
	
	CelthApi_Demux_SetSectionFilter(indexfilterparam.hf,&indxFilterSettings);

	CelthApi_Demux_SetSectionFilterCallback(indexfilterparam.hf,&indxSectionCallbackfun);

	CelthApi_Demux_SetSectionFilterCallbackParam(indexfilterparam.hf,(void *)indexfilterparam.ev);


	CelthApi_Demux_EnableSectionFilter(indexfilterparam.hf);

	
}
return &indexfilterparam;


	
}






