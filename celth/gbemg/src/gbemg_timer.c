#include "celth_stddef.h"

#include "celth_rtos.h"

#include "celth_mutex.h"

#include "celth_debug.h"

#include "gbemg_stddef.h"
#include "gbemg_gendef.h"

#include "gbemg_timer.h"


const static U8 normalmonthset[12]={31,28,31,30,31,30,31,31,30,31,30,31};

const static U8 specialmonthset[12]={31,29,31,30,31,30,31,31,30,31,30,31};


static bool stopgbtime=false;

static  GBEMG_Time_t basetime;

static CELTHOSL_SemId_t	gbtime_lock;

static CELTHOSL_TaskId_t  timetaskid;


static void IncreaseTime(GBEMG_Time_t* pTime);



bool GbTimeInit()
{
/*	gbtime_lock = CELTH_SemCreate( "gbtime", CELTHOSL_FifoNoTimeOut, 1 );

	if(gbtime_lock==NULL)
	{
		return false;
	}*/
	return true;
}



GBEMG_Time_t GetCurrentTime()
{
	return basetime;
}

void  SyncGbEmgTime(GBEMG_Time_t* time)
{
	CelthApi_Debug_EnterFunction("SyncGbEmgTime");
	CELTH_SemWait( gbtime_lock, CELTHOSL_WaitForEver );
	CelthApi_Debug_Msg("m:%d;s:%d\n",time->minute,time->second);
	CELTHOSL_Memcpy(&basetime,time, sizeof(GBEMG_Time_t));
	CelthApi_Debug_Msg("basetime m:%d;s:%d\n",basetime.minute,basetime.second);
	CELTH_SemPost(gbtime_lock);
	CelthApi_Debug_LeaveFunction("SyncGbEmgTime");
}




void* gbemg_time_fn(void* context)
{
	
	
	GBEMG_Time_t* ptime=(GBEMG_Time_t*)context;

	CelthApi_Debug_EnterFunction("gbemg_time_fn");
	

	
		
	
	if(ptime!=NULL)
	{
		CelthApi_Debug_Msg("Sync time with m:%d,s:%d\n",ptime->minute,ptime->second);
		SyncGbEmgTime(ptime);
	}
	do{


	
		CELTH_SemWait( gbtime_lock, CELTHOSL_WaitForEver );

	
		IncreaseTime(&basetime);

		
		

		CELTH_SemPost(gbtime_lock);

CelthApi_Debug_Msg("basetime :m:%d,s:%d\n",basetime.minute,basetime.second);

		
		
		CELTHOSL_Sleep(1000);
	}while(stopgbtime!=true);

CelthApi_Debug_LeaveFunction("gbemg_time_fn");
	
}
/* noticed:
 time should be NULL ,if have not any time send to the function.
 
 otherwise send the time to sync.

*/


bool StartGbTime(GBEMG_Time_t* time)
{
	
	CelthApi_Debug_EnterFunction("StartGbTime");
	gbtime_lock = CELTH_SemCreate( "gbtime", CELTHOSL_FifoNoTimeOut, 1 );
	
	/*CelthApi_Debug_Msg("Create gbtime sem=0x%08x\n",gbtime_lock);*/
	if(gbtime_lock==NULL)
	{
		return false;
	}
	
	stopgbtime=false;

	/*CelthApi_Debug_Msg("in startgbtime time=0x%x\n",time);

	CelthApi_Debug_Msg("in startgbtime time:m=%d,time:s=%d\n",time->minute,time->second);*/

	SyncGbEmgTime(time);
	
	timetaskid=CELTHOSL_TaskCreate(NULL,gbemg_time_fn,NULL,1024*2,13,0,0);

	/*CelthApi_Debug_Msg("Create Task gbemg_time_fn\n");*/
	CelthApi_Debug_LeaveFunction("StartGbTime");
	
	return true;
}
	
void StopGbTime()
{
	stopgbtime=true;
	
	CELTHOSL_Sleep(2000);
	
	CELTHOSL_TaskDelete(timetaskid);
	
	CELTH_SemDestroy(gbtime_lock);
	
	
}

void IncreaseTime( GBEMG_Time_t* pTime )
{
	
        U8* monthset;
/*CelthApi_Debug_EnterFunction("IncreaseTime");*/
     
        if(pTime->year/4)
     	{
     		monthset=&normalmonthset;
	}
        else
     	{
     		monthset=&specialmonthset;
	}	
       
        pTime->second +=1;
	pTime->second=pTime->second%60;
 /*CelthApi_Debug_Msg("time.second=%d\n",pTime->second);*/  
	
        if(pTime->second==0)
	{
		/*pTime->second=1;*/
		pTime->minute+=1;
       		pTime->minute=pTime->minute%60;
	}
	/*CelthApi_Debug_Msg("time.minute=%d\n",pTime->minute);*/  
        if(pTime->minute==0&&pTime->second==0)
     	{
     		/*pTime->minute=1;*/
		pTime->hour+=1;

     		pTime->hour=pTime->hour%24;	
	}
	/*CelthApi_Debug_Msg("time.hour=%d\n",pTime->hour);  */
	if(pTime->hour==0&&pTime->minute==1)
	{
		pTime->day+=1;
		pTime->day=pTime->day%monthset[pTime->month];
	}
	if(pTime->day==0&&pTime->hour==0)
	{
		pTime->day=1;
		pTime->month+=1;	
		pTime->month=pTime->month%12;
				
	}
	if(pTime->month==0)
	{
		pTime->month=1;
		pTime->year+=1;
	} 

/*CelthApi_Debug_LeaveFunction("IncreaseTime");*/
     	
}



I32 GBTimeDiff(GBEMG_Time_t* starttime, GBEMG_Time_t* endtime)
{
	int daydif,hourdif,mindif,secdif;
	U32 retsecond;
	if(starttime==NULL||endtime==NULL)
	{
		return 0;
	}


	daydif=endtime->day-starttime->day;

	hourdif = endtime->hour-starttime->hour;

	if(hourdif<0)
		{
			if(daydif>0)
				{
				daydif-=1;
			hourdif = 24+hourdif;
				}
		}

	mindif = endtime->minute-starttime->minute;

	 if(mindif<0)
	 	{
	 		if(hourdif>0)
	 			{
	 				hourdif-=1;
					mindif=60+mindif;
	 			}
	 	}

	secdif = endtime->second-starttime->second;

		if(secdif<0)
			{
				if(mindif>0)
					{
						mindif-=1;
						secdif=60+secdif;
					}
			}
		

	retsecond = daydif*24*3600;

	retsecond +=hourdif*3600;

	retsecond +=mindif*60;

	retsecond +=secdif;

	return retsecond;

}



