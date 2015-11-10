
#include "celth_stddef.h"
#include "celth_rtos.h"

#include "celth_debug.h"

#include "celth_message.h"

#include "celth_remotekey.h"

/* following header file is brcm nexus header file*/

#include "nexus_ir_input.h"



CELTH_STATIC CELTH_MessageQueue_t* pRcMsgQueue;


CELTH_STATIC CELTH_U8 CelthApi_FoundRcMapValue(CELTH_U32 keyvalue);

CELTH_STATIC NEXUS_IrInputMode RcProtl2BrcmProtl(CELTH_RemoteProtl protl)
{

	NEXUS_IrInputMode  mode=NEXUS_IrInputMode_eRemoteA;
	switch(protl)
		{
		case CELTH_REMOTE_NEC:
			mode=NEXUS_IrInputMode_eCirNec;
			break;
		case CELTH_REMOTE_RC5:
			mode=NEXUS_IrInputMode_eCirRc5;
			break;
		case CELTH_REMOTE_RC6:
			mode=NEXUS_IrInputMode_eCirRC6;
			break;
		default:
			break;
			
		}

	return mode;

}



void irCallback(void *pParam, int iParam)
{

	CELTH_U8 keyvalue;
	
    size_t numEvents = 1;
	
    NEXUS_Error rc = 0;
    bool overflow;

	
    NEXUS_IrInputHandle irHandle = *(NEXUS_IrInputHandle *)pParam;
    
    while (numEvents && !rc) {
        NEXUS_IrInputEvent irEvent;
        rc = NEXUS_IrInput_GetEvents(irHandle,&irEvent,1,&numEvents,&overflow);
        if (numEvents)
        	{
            CelthApi_Debug_Msg("irCallback: rc: %d, code: %08x, repeat: %s\n", rc, irEvent.code, irEvent.repeat ? "true" : "false");
			keyvalue=CelthApi_FoundRcMapValue(irEvent.code);

			CELTH_MsgSend(pRcMsgQueue,(CELTH_VOID*)&keyvalue);

			
        	}
    }
}





CELTH_VOID CelthApi_RemoteKey_Init(CELTH_VOID* context)
{

pRcMsgQueue = CELTH_MsgCreate(sizeof(CELTH_U8),32);


CELTH_Symbol_Register("rckey_msgbox",pRcMsgQueue);



}

CELTH_VOID CelthApi_RemoteKey_SetKeyMap(CELTH_HANDLE RcHandle,Celth_RemoteKeyMap* KeyMap)
{
}

CELTH_HANDLE* CelthApi_RemoteKey_Open(CELTH_RemoteProtl protl)
{

NEXUS_IrInputHandle irHandle;
NEXUS_IrInputSettings irSettings;




NEXUS_IrInput_GetDefaultSettings(&irSettings);
irSettings.mode = RcProtl2BrcmProtl(protl);
irSettings.dataReady.callback = irCallback;
irSettings.dataReady.context = &irHandle;
irHandle = NEXUS_IrInput_Open(0, &irSettings);

return &irHandle;


}




