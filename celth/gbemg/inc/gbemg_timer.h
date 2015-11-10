#ifndef _GBEMG_TIMER_H_
#define _GBEMG_TIMER_H_


/* 
copyright reserved by 
celtech company Co Ltd,
*/
 


#ifdef __cplusplus
extern "C" {
#endif






GBEMG_Time_t GetCurrentTime();

void  SyncGbEmgTime(GBEMG_Time_t* time);

bool StartGbTime(GBEMG_Time_t* time);

void StopGbTime();


/*return second*/

I32 GBTimeDiff(GBEMG_Time_t* starttime, GBEMG_Time_t* endtime);



bool GbTimeInit();


#ifdef __cplusplus
}
#endif

#endif


