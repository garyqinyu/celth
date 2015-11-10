
#include<stdio.h>
#include<stdlib.h>


#include "celth_stddef.h"

#include "celth_rtos.h"

#include "celth_mutex.h"
#include "celth_message.h"

#include "celth_display.h"
#include "celth_avplay.h"

#include "urgbcastdef.h"

#include "urgbcastcontrol.h"

#include "urgbcastprocess.h"


CELTH_STATIC CELTHOSL_TaskId_t egbtprocessTaskId;

CELTH_STATIC CELTH_MessageQueue_t* pMsgQueue;


void* emergebcast_process_thread_fn(void* context)
{

	Emerge_Cmd_t  cmd;


	CELTH_MessageQueue_t* pMqueue;


	
	if(CELTH_Symbol_Enquire_Value("eb_process_msgbox",&pMqueue))
	{
		return TRUE;
	}

	

	while(1)
		{
		CELTH_MsgWait(pMqueue,(CELTH_VOID*)&cmd,0xFFFFFFFF);
		printf("\n**************Get Msg from box [%x]******\n",cmd.cmd_type);

		switch(cmd.cmd_type)
			{
			case EMERGE_CMD_TURN_OFF:
				{
					printf("\ncall turn off\n");
				/*	CelthApi_Av_SetAudioMute();*/
					break;
				}
			case EMERGE_CMD_TURN_ON:
				{
					CelthApi_Av_SetAudioUnmute();
					break;
				}
			case EMERGE_CMD_SET_PLAY_VOL:
				{
					printf("\ncall set vol\n");
				/*	CelthApi_Av_SetAudioVolume(cmd.EmergeCmdParam.EmergeSetVal.vol);*/
					break;
				}
			case EMERGE_CMD_PLAY_EMERGE_INFO_FROM_TS:
				{
					CelthApi_Av_Start_Audio(cmd.EmergeCmdParam.EmergePlayStream.AudioPid,baudio_format_mp3);
					break;
				}
			default:
				{
					printf("not implement yet\n");	
					break;
				}
						
						
				

				
				
			}
		}

}



CELTH_VOID emerge_BcastProcess_Start()
{
/*
	egbtprocessTaskId = CELTHOSL_TaskCreate(NULL,emergebcast_process_thread_fn,(CELTH_VOID*)NULL,1024*8,12,0,0);
*/
	
	pMsgQueue = CELTH_MsgCreate(sizeof(Emerge_Cmd_t),16);
	

	CELTH_Symbol_Register("eb_process_msgbox",pMsgQueue);

    egbtprocessTaskId = CELTHOSL_TaskCreate(NULL,emergebcast_process_thread_fn,(CELTH_VOID*)NULL,1024*8,12,0,0);




}

