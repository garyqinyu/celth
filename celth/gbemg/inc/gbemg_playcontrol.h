#ifndef _GBEMG_PLAYCONTROL_H_
#define _GBEMG_PLAYCONTROL_H_


/* 
copyright reserved by 
celtech company Co Ltd,
*/
 


#ifdef __cplusplus
extern "C" {
#endif


/*#include "gbemg_gendef.h"*/



/*

typedef enum GBEMG_MsgType_e{

GBEMG_MSG_eHandlerInit,
GBEMG_MSG_eSetupDataType =0x80,
GBEMG_MSG_eSetupIndxVersionChanged=0x80,
GBEMG_MSG_eSetupIndxList,
GBEMG_MSG_eSetupContGet,
GBEMG_MSG_eSetupContList,
GBEMG_MSG_eSetupContStart,

GBEMG_MSG_eActType = 0x8000,
GBEMG_MSG_eRunTimerArrial=0x8000,
GBEMG_MSG_eActArrialEvent,
GBEMG_MSG_eActTerminalEvent,

GBEMG_MSG_eMax,




}GBEMG_MsgType;

*/

typedef struct GBEMG_PlayBlk_s{
	
	void*               pContent;
}GBEMG_PlayBlk_t;

/*
typedef struct GBEMG_Play_Msg_s{
	GBEMG_MsgType PlayMsgType;
	
	GBEMG_URGENT_LEVEL	EmgLevel;
	U16                 EMGID;
	
	GBEMG_PlayBlk_t     blk;

}GBEMG_Play_Msg_t;
*/

/* RECORD THE PLAYED EMG AND WILL PLAY EMG */ 

typedef struct GBEMG_Play_Record_s{

	U8 playmask;
	U16 playemgid[8];
	
}GBEMG_Play_Record_t;

/* RECORD BEEN  STOPED EMG BY PLAY */

typedef struct GBEMG_Stop_Record_s{

	U8 stopmask;
	U16 stopemgid[8];
	
}GBEMG_Stop_Record_t;




#define EMG_PlayList(x) emgplay_list##x


#define Get_EmgPlayBlk(x) do{\
	GBEMG_PlayBlk_t blk;\
	blk=EMG_PlayList(x);\
	return blk;\
}while(0)


#define Set_EmgPlayBlk(x,blk) do{\
	EMG_PlayList(x)=blk;\
}while(0)

#define MSG_FROM_PLAY 1

#define MSG_FROM_STOP 2





U8  Start_GBEMG_PlayHandler(void* content);


U8 Start_GBEMG_AllPlayProcess(void* content);



#ifdef __cplusplus
}
#endif



#else
#error "Multiple inclusions of gbemg_playcontrol.h"

#endif



