#ifndef __CELTH_RTOS_H
#define __CELTH_RTOS_H

#ifdef __cplusplus

extern "C" {

#endif

#include <stdio.h>

#include <pthread.h> 


#include "celth_stddef.h"

/*#include "bkni.h"*/


#define CELTHOSL_MSEC_PER_TICK			(1)
#define CELTHOSL_MSEC_TO_TICKS(msec)		(((msec) / CELTHOSL_MSEC_PER_TICK) + 1)




/* --- Tasks Typedef --------------------------------------------------------- */
typedef CELTH_VOID*	CELTHOSL_TaskId_t ;
typedef	CELTH_VOID*	(*CELTHOSL_EntryPoint_t)	( CELTH_VOID* );

/*typedef struct _CELTHOSL_TaskInfo_t	CELTHOSL_TaskInfo_t;*/




typedef struct _CELTHOSL_TaskInfo_t	CELTHOSL_TaskInfo_t;
struct _CELTHOSL_TaskInfo_t
{
	CELTH_INT				iStackSize ; /* Task Stack Size               */
	CELTH_INT				iPriority  ; /* Task Priority                  */
	CELTH_INT				iQueueSize ; /* Max number of message         */
	CELTH_INT				iMsgSize   ; /* Max size of a message in byte */

	CELTH_VOID*			pMessageQueue;

	pthread_t           taskid;
	CELTHOSL_TaskInfo_t*	pNext;
};








/* --- Timers Typedef ---------------------------------------------------------- */
typedef CELTH_VOID*	CELTHOSL_TimerId_t ;
typedef CELTH_VOID   (*CELTHOSL_TimerExpire_t) (CELTHOSL_TimerId_t , CELTH_VOID*, CELTH_VOID* ) ;

typedef enum
{
	CELTHOSL_kTimerStopped,  /* The timer is not started or is stopped                 */
	CELTHOSL_kTimerRunning,  /* The timer is started and is running                    */
	CELTHOSL_kTimerExpired   /* The timer has been started (without CB, and is expired */
} CELTHOSL_TimerState_t ;

typedef struct _CELTHOSL_TimerInfo_t	CELTHOSL_TimerInfo_t;
struct _CELTHOSL_TimerInfo_t
{
	CELTH_VOID*				pUserRef;
	CELTH_VOID*				pClientData;
	CELTHOSL_TimerExpire_t	pfClientCallBack;
	CELTHOSL_TimerState_t	TimerState;

	CELTHOSL_TimerInfo_t	*pNext;
};

/* --- General Functions ------------------------------------------------------*/
extern	CELTH_INT	CELTHOSL_Init		( CELTH_VOID ) ;
extern	CELTH_INT	CELTHOSL_Terminate	( CELTH_VOID ) ;

extern	CELTH_CONST	CELTH_CHAR*	CELTHOSL_GetVersion( CELTH_VOID ) ;

/* --- Tasks Functions ------------------------------------------------------*/
extern	CELTHOSL_TaskId_t		CELTHOSL_TaskCreate	( CELTH_CONST CELTH_CHAR*		pcName,
														CELTHOSL_EntryPoint_t	pfEntryPoint,
														CELTH_VOID*				pParam,
														CELTH_INT					iStackSize,
														CELTH_INT					iPriority,
														CELTH_INT                   iMsgSize,
														CELTH_INT                   iQueueSize
 );

														
extern	CELTHOSL_TaskId_t		CELTHOSL_TaskId        ( CELTH_CONST CELTH_CHAR*	pucName );
extern	CELTHOSL_TaskId_t		CELTHOSL_TaskIdSelf    ( CELTH_VOID );



extern	CELTH_VOID                CELTHOSL_TaskDelay     ( CELTH_INT iDelay );
extern	CELTH_INT                 CELTHOSL_TaskDelete    ( CELTHOSL_TaskId_t pTaskId );
extern	CELTH_INT                 CELTHOSL_TaskSuspend   ( CELTHOSL_TaskId_t pTaskId );
extern	CELTH_INT                 CELTHOSL_TaskResume    ( CELTHOSL_TaskId_t pTaskId );



/* --- Memory Management Functions --------------------------------------------*/
/* Memory allocation in system_partition = partition defined in cacheable -----*/
extern	CELTH_VOID*   CELTHOSL_MemoryAlloc		( CELTH_INT iSize ) ;
extern	CELTH_VOID	CELTHOSL_MemoryFree		( CELTH_VOID* pBlock ) ;

extern CELTH_VOID * CELTHOSL_Memset(CELTH_VOID *b, CELTH_INT c, CELTH_INT len);

extern CELTH_VOID * CELTHOSL_Memcpy(CELTH_VOID *dst, CELTH_CONST CELTH_VOID *src, CELTH_INT len);
extern CELTH_INT    CELTHOSL_Memcmp(CELTH_VOID* dst, CELTH_VOID* src, CELTH_INT len);

/* --- Reboot Functions --------------------------------------------*/
extern	CELTH_VOID   CELTHOSL_Reboot( CELTH_VOID );

/* --- Other Functions --------------------------------------------*/
extern	CELTH_Status_t	CELTHOSL_GetTick( CELTH_DWORD* pdwTicks);

extern 	CELTH_Status_t	CELTHOSL_Sleep(CELTH_INT imillisec);

extern  CELTH_VOID      CELTHOSL_Delay(CELTH_INT iMicrosec);


extern 	CELTH_VOID CELTH_Fail(CELTH_VOID);

  




#ifdef __cplusplus

}

#endif






#endif
