
#include "celth_stddef.h"
#include "celth_rtos.h"
#include "celth_mutex.h"
#include "celth_list.h"
#include "celth_debug.h"
#include "celth_message.h"

#include "gbemg_stddef.h"
#include "gbemg_gendef.h"


#include "gbemg_type.h"
#include "gbemg_index.h"
#include "gbemg_content.h"
#include "gbemg_emglist.h"



#include "gbemg_control.h"
#include "gbemg_playcontrol.h"


#define MAXPROIRY 4



static GBEMG_PlayBlk_t emgplay_list0,emgplay_list1,emgplay_list2,emgplay_list3,emgplay_list4;




CELTHOSL_TaskId_t playhandlerid,stopplayid,playid;

CELTHOSL_SemId_t  playmasksem;

CELTHOSL_SemId_t  stopmasksem;


U16 curplayemgid;

static GBEMG_Play_Record_t playRec;

static GBEMG_Stop_Record_t stopRec;

static bool exitplayflag,exitstopplayflag;






static CELTH_MessageQueue_t* pplayQueue;






static U8 getposofbit(GBEMG_URGENT_LEVEL level)
{
	U8 ret=4;
	
   switch(level)
	{
	case Level_One:
		ret=0;
		break;
	case Level_Two:
		ret=1;
		break;
        case Level_Three:
		ret=2;
		break;
	case Level_Four:
		ret=3;
		break;
	case Level_Test:
		ret=4;
		break;
	default:
		ret=4;	
	}
	return ret;

}



static void setplaymask(U8 playmaskbit, U16 playemgid)
{
	CELTH_SemWait( playmasksem, CELTHOSL_WaitForEver );
    	
  playRec.playmask|=((U8)(0x01<<playmaskbit));

  playRec.playemgid[playmaskbit]=playemgid;

  CELTH_SemPost(playmasksem);

}

static bool ishighplaypriority(U8 playmaskbit)
{
	U8 temp;

	temp=playRec.playmask;

	if(temp>=((U8)(0x01<<playmaskbit)))
		{
		return true;
		}
	return false;
}

static void clearplaymask(U8 playmaskbit)
{
   
	 CELTH_SemWait( playmasksem, CELTHOSL_WaitForEver );
		 
   playRec.playmask&=~((U8)(0x01<<playmaskbit));
   
   playRec.playemgid[playmaskbit]=0xFFFF;
   
   CELTH_SemPost(playmasksem);
}

static U8 gethighestpriorityplaymask()
{
	U8 bit=0x01;
	U8 i,temp=0x00;
	for(i=0;i<MAXPROIRY+1;i++)
		{
		 temp=(bit<<i);

		 if(temp==playRec.playmask)
		 	break;
		}
   

	return i;


	
}

static U16 getplayemgidfrommask(U8 mask)
{
	return playRec.playemgid[mask];
}


static void setstopmask(U8 stopmaskbit, U16 stopemgid)
{
	CELTH_SemWait( stopmasksem, CELTHOSL_WaitForEver );
    	
  stopRec.stopmask|=((U8)(0x01<<stopmaskbit));

  stopRec.stopemgid[stopmaskbit]=stopemgid;

  CELTH_SemPost(stopmasksem);

}


static void clearstopmask(U8 stopmaskbit)
{
   
	 CELTH_SemWait( stopmasksem, CELTHOSL_WaitForEver );
		 
   stopRec.stopmask&=~((U8)(0x01<<stopmaskbit));
   
   stopRec.stopemgid[stopmaskbit]=0xFFFF;
   
   CELTH_SemPost(stopmasksem);
}


static void resetplaymask()
{
	U8 i;
	
	  CELTH_SemWait( playmasksem, CELTHOSL_WaitForEver );
		  
	playRec.playmask=0x00;

	for(i=0;i<8;i++)
		{
	playRec.playemgid[i]=0xFFFF;
		}
	
	CELTH_SemPost(playmasksem);
}


static void resetstopmask()
{
	U8 i;
	
	  CELTH_SemWait( stopmasksem, CELTHOSL_WaitForEver );
		  
	stopRec.stopmask=0x00;

	for(i=0;i<8;i++)
		{
	stopRec.stopemgid[i]=0xFFFF;
		}
	
	CELTH_SemPost(stopmasksem);
}



bool SendPlayMsg2PlayHander(GBEMG_Msg_t  Msg)
{

	

	
	CELTH_MessageQueue_t* pMsgQueue;

	CELTH_INT      iMessageQCapability;

	CELTH_Status_t  eStatus;

	if(CELTH_Symbol_Enquire_Value("gb_play_msgbox",&pMsgQueue))
	{
		return true;
	}

	
	eStatus =  CELTH_MsgCapabilityGet( pMsgQueue, &iMessageQCapability);

	
	if (iMessageQCapability > 4)
	{
		/* Fill the user input message and sent it to the task ------------------*/

		eStatus = CELTH_MsgSend( pMsgQueue, &Msg );

		
		CelthApi_Debug_Msg("\n****send message msg[%x]********\n",Msg.msg_type);

		if(eStatus!=CELTH_NO_ERROR)
			{return true;}
			
			
	}
	else
	{
			return true;
	}

	return false;
}








void gbemg_playcontrolhandler_fn(void* content)/* this thread  send play and stop play message to playerhandler thread*/
{
	do{

		CELTHOSL_Sleep(10);

		}while(exitplayflag==false);
}
void gbemg_stopplaycontrolhandler_fn(void* content) /* this thread send stop play message to playerhandler thread*/
{
	
	do{

		CELTHOSL_Sleep(10);

		}while(exitstopplayflag==false);
}
void gbemg_playhandler_fn(void* content) /* this thread is  charged for play or stop play emg info*/
{

	CELTH_MessageQueue_t* pQueue;
	int iMessageQCapability;
	GBEMG_Msg_t msg;

	U16       playemgid;


	GBEMG_ProcParam_t procparam;

	
	
	if(CELTH_Symbol_Enquire_Value("gb_play_msgbox",&pQueue))
	{
		return ;
	}

	while(1)
		{
	CELTH_MsgWait(pQueue,(void*)&msg,0xFFFFFFFF);

	switch(msg.msg_type)
	{
	case GBEMG_MSG_eActArrial:
	{

		CELTHOSL_Memcpy(&procparam,(GBEMG_ProcParam_t*)msg.msg_packet,msg.msg_packet_length);
		if(ishighplaypriority(getposofbit(procparam.pcaseindex->emg_case_urgent_level)))
		{
			setplaymask(getposofbit(procparam.pcaseindex->emg_case_urgent_level),procparam.pcaseindex->emg_case_id);

			if(curplayemgid!=0xFFFF)
				{
				/* stop current play emgid*/
				}
			curplayemgid=procparam.pcaseindex->emg_case_id;
			/* play the emgid*/
		}
		else
		{
			
			setplaymask(getposofbit(procparam.pcaseindex->emg_case_urgent_level),procparam.pcaseindex->emg_case_id);
		}
		break;
	}

	case GBEMG_MSG_eActTerminal:
	{

		
		CELTHOSL_Memcpy(&procparam,(GBEMG_ProcParam_t*)msg.msg_packet,msg.msg_packet_length);
		if(curplayemgid==procparam.pcaseindex->emg_case_id)
			{
				/* stop the emg play*/
				curplayemgid=0xffff;

				U8 nextplayemgidpriority;

				clearplaymask(getposofbit(procparam.pcaseindex->emg_case_urgent_level));

				nextplayemgidpriority = gethighestpriorityplaymask();

				if(nextplayemgidpriority!=MAXPROIRY+1)
					{
					curplayemgid=playRec.playemgid[nextplayemgidpriority];
					/* play the emgid*/
					}
                 
				
			}
		else
			{
			if(ishighplaypriority(getposofbit(procparam.pcaseindex->emg_case_urgent_level)))
				{
				 clearplaymask(getposofbit(procparam.pcaseindex->emg_case_urgent_level));
				}
			else
				{
				 /* it is error condition ,should not happy*/
				 
				}
			}
		break;
	}
	case GBEMG_MSG_eActReset:
	{

			U8 playone;

			U16 emgid;

			/* just clear the highest play, if there are play*/

			playone=gethighestpriorityplaymask();

			if(playone!=0x00)
				{
			emgid=getplayemgidfrommask(playone);
				}

				/* set this play stop*/
			
			resetplaymask();
			resetstopmask();
			break;
	}

	case GBEMG_MSG_eBoxRegister:
		{
				break;
		}
	case GBEMG_MSG_ePowerSwitch:
		{
				break;
		}
	case GBEMG_MSG_eAmpSwitch:
		{
			break;
		}

	case GBEMG_MSG_eSetTime:
		{
			break;
			
		}
    case GBEMG_MSG_eValumeCtrl:
		{
			break;
    	}
	default:
       break;
		
	}

	CELTHOSL_Sleep(10);
		}
	

}
GBEMG_Err Start_GBEMG_PlayControlHandler(void* content)
{
	exitplayflag=false;
	playid =  CELTHOSL_TaskCreate(NULL,gbemg_playcontrolhandler_fn,content,2048,12,0,0);
}
GBEMG_Err Start_GBEMG_StopPlayControlHandler(void* content)
{
	exitstopplayflag=false;
	stopplayid =  CELTHOSL_TaskCreate(NULL,gbemg_stopplaycontrolhandler_fn,content,2048,12,0,0);
}


GBEMG_Err Stop_GBEMG_PlayControlHandler(void* content)
{
	
	exitplayflag=true;
	CELTHOSL_TaskDelete(playid);
}
GBEMG_Err Stop_GBEMG_StopPlayControlHandler(void* content)
{
	
	exitstopplayflag=true;
	CELTHOSL_TaskDelete(stopplayid);
}




U8 Start_GBEMG_PlayHandler(void* content)
{

	playhandlerid =  CELTHOSL_TaskCreate(NULL,gbemg_playhandler_fn,content,1024*32,12,0,0);
	
}
U8 Start_GBEMG_AllPlayProcess(void* content)
{


	


	
	playmasksem=CELTH_SemCreate( "emgplay", CELTHOSL_FifoNoTimeOut, 1 );

	if(playmasksem==NULL)
	{
		return 1;
	}


	
	stopmasksem=CELTH_SemCreate( "emgstop", CELTHOSL_FifoNoTimeOut, 1 );

	if(stopmasksem==NULL)
	{
		return 1;
	}
	
	Start_GBEMG_PlayHandler(NULL);

	Start_GBEMG_PlayControlHandler(NULL);

	Start_GBEMG_StopPlayControlHandler(NULL);

	return 0;
	

}



