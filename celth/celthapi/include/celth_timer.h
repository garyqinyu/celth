

/*#include "celth_stddef.h"*/






typedef CELTH_VOID*	CELTH_TimerId_t ;
typedef CELTH_VOID   (*CELTH_TimerExpire_t) (CELTH_VOID* ) ;



typedef struct _Celth_TimerInfo_t	Celth_TimerInfo_t;
struct _Celth_TimerInfo_t
{
	CELTH_INT				iTimeValue;
	CELTH_VOID*				pClientData;
	CELTH_TimerExpire_t	pfClientCallBack;

};




/* --- Timer Management Functions ---------------------------------------------*/
CELTH_Status_t CELTH_TimerCreate	( CELTH_TimerId_t* pTimerId);
CELTH_Status_t CELTH_TimerConfigure	( CELTH_TimerId_t TimerId, CELTH_TimerExpire_t pfClientCallBack );
CELTH_Status_t CELTH_TimerStart		( CELTH_TimerId_t TimerId, CELTH_VOID* pClientData, CELTH_INT iTimerValue );
CELTH_Status_t CELTH_TimerStop      ( CELTH_TimerId_t TimerId ) ;

CELTH_Status_t CELTH_TimerInit(CELTH_VOID* pContext );
CELTH_Status_t CELTH_TimerUninit();



