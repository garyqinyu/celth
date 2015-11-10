#ifndef __CELTH_REMOTEKEY_H
#define __CELTH_REMOTEKEY_H

#ifdef __cplusplus

extern "C" {

#endif

typedef enum CELTH_RemoteProtl{

CELTH_REMOTE_NEC,
CELTH_REMOTE_RC6,
CELTH_REMOTE_RC5,

}CELTH_RemoteProtl;

typedef struct Celth_RemoteKeyMap{


CELTH_U32 KeyValue;

CELTH_U8 MapValue;

}Celth_RemoteKeyMap;



CELTH_VOID CelthApi_RemoteKey_Init(CELTH_VOID* context);

CELTH_VOID CelthApi_RemoteKey_SetKeyMap(CELTH_HANDLE RcHandle,Celth_RemoteKeyMap* KeyMap);


CELTH_HANDLE* CelthApi_RemoteKey_Open(CELTH_RemoteProtl protl);







#ifdef __cplusplus

}

#endif






#endif/*__CELTH_REMOTEKEY_H*/


