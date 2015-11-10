#ifndef _GBEMG_CONTROL_H_
#define _GBEMG_CONTROL_H_


/* 
copyright reserved by 
celtech company Co Ltd,
*/
 


#ifdef __cplusplus
extern "C" {
#endif




typedef struct GBEMG_BoxBasic_Info_t{

	CELTH_BOOL bvalid;

	U8  boxselfid[8];
	U8  areacode[6];
	

}GBEMG_BoxBasic_Info;





#define CONTRLMSGNUM 64




U8 Start_GBEMG_MonitorGBEMGIndxTableHandler(void* content);


U8 Start_GBEMG_MonitorBoxCtrlInfoSectionHandler(void* content);


U8 Start_GBEMG_ControlHandler(void* content);



/*U8 Start_GBEMG_SetupEmgListHandler(void* content);*/




void Start_GBEMG_EmrgTimerProcess(void* content);

void Stop_GBEMG_EmrgTimerProcess(void* content);





GBEMG_Err ResetPlayIndxHandler();






                                                        
GBEMG_Err  Start_GBEMG_PlayControlHandler(void* content);      
GBEMG_Err  Start_GBEMG_StopPlayControlHandler(void* content);  
                                                        
/*GBEMG_Err  Start_GBEMG_PlayHandler(void* content);*/             
                                                        
GBEMG_Err  Stop_GBEMG_PlayControlHandler(void* content);	   
GBEMG_Err  Stop_GBEMG_StopPlayControlHandler(void* content);  
																												
/*GBEMG_Err  Stop_GBEMG_PlayHandler(void* content);*/			   




GBEMG_Err ReadTheBoxBasicInfo(GBEMG_BoxBasic_Info* BoxInfo);


GBEMG_Err  GetTheBoxCoverAreaCode(U8* areacode);




                                                        
bool SendPlayMsg2PlayHander(GBEMG_Msg_t  Msg);              




void ResetPlayRecList();




U8 GBEMG_Init(void* content);

U8 GBEMG_Start(void* content);




#ifdef __cplusplus
}
#endif

#endif

