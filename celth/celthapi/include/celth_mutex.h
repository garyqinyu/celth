#ifndef	_CELTH_MUTEX_H_
#define	_CELTH_MUTEX_H_

/* Includes -------------------------------------------------------------------*/


#ifdef __cplusplus /* celth mutex  */
extern "C" {
#endif


#include <semaphore.h>

#include <pthread.h>

#include "celth_stddef.h"


/* --- Semaphores Typedef ----------------------------------------------------- */
typedef enum
{
	CELTHOSL_kCreate,
	CELTHOSL_kInit
} CELTHOSL_SemCreate_t;


typedef enum
{
	CELTHOSL_NoWait      =  0 ,
	CELTHOSL_WaitForEver = -1
} CELTHOSL_Wait_t ;




typedef enum
{
	CELTHOSL_FifoTimeOut ,             /* Fifo mode with time-out */
	CELTHOSL_FifoNoTimeOut ,           /* Fifo mode without time-out */
	CELTHOSL_PriorityTimeOut ,         /* Priority mode with time-out */
	CELTHOSL_PriorityNoTimeOut         /* Priority mode without time-out*/
} CELTHOSL_SemType_t ;

typedef  struct
{
  char*	pcSemName   ; /* Semaphore Name                           */
  CELTHOSL_SemType_t		eSemType    ;  /* Semaphore Type : Fifo, Priority          */
  CELTHOSL_SemCreate_t	eSemCreate  ;  /* informs Type of creation : Init or Create*/
  CELTH_DWORD				uiSignature ;  /* Signature informations during Creation   */
  
  sem_t                 sem;
  
} CELTHOSL_Sem_t;

typedef	CELTHOSL_Sem_t*			CELTHOSL_SemId_t;

/* --- MUTEX Typedef ----------------------------------------------------------- */
#define	CELTHOSL_MutexType_t		CELTHOSL_SemType_t 
#define CELTHOSL_MutexCreate_t	CELTHOSL_SemCreate_t




typedef struct
{
	CELTHOSL_MutexCreate_t	eMutexCreate  ;  /* informs Type of creation : Init or Create*/
	CELTHOSL_MutexType_t    eMutexType; 
	CELTH_INT                 iMutexCounter ;  /* Mutex Counter                            */
	pthread_mutex_t 			hMutex;
	CELTH_DWORD				uiSignature   ;  /* Signature informations during Creation   */
} CELTHOSL_Mutex_t;


typedef CELTHOSL_Mutex_t*	CELTHOSL_MutexId_t ;




extern	CELTHOSL_SemId_t CELTH_SemCreate	( char*	pcName,
											CELTHOSL_SemType_t	eSemType ,
											int				iInitValue );

extern	CELTHOSL_SemId_t CELTH_SemInit		( char*    pcName,
											CELTHOSL_Sem_t*			pSem ,
											CELTHOSL_SemType_t		eSemType ,
											int					iInitValue ) ;

extern	CELTH_Status_t CELTH_SemDestroy	( CELTHOSL_SemId_t SemId) ;
extern	CELTH_Status_t CELTH_SemWait	( CELTHOSL_SemId_t SemId, int iMilliseconds ) ;
extern	CELTH_Status_t CELTH_SemPost	( CELTHOSL_SemId_t SemId ) ;

/* --- Mutex Functions --------------------------------------------------------*/
extern	CELTHOSL_MutexId_t  CELTH_MutexCreate  ( CELTHOSL_MutexType_t eMutextype );
extern	CELTHOSL_MutexId_t  CELTH_MutexInit    ( CELTHOSL_Mutex_t *pMutex, CELTHOSL_MutexType_t eMutextype );

extern	CELTH_Status_t    CELTH_MutexDestroy( CELTHOSL_MutexId_t MutexId );
extern	CELTH_Status_t    CELTH_MutexLock  ( CELTHOSL_MutexId_t MutexId );
extern	CELTH_Status_t    CELTH_MutexUnLock( CELTHOSL_MutexId_t MutexId);









#ifdef __cplusplus /* celth mutex */
}
#endif

#endif


