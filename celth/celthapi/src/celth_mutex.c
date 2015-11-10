
#include <stdlib.h>   
#include <stdio.h>   
#include <errno.h>   

#include <semaphore.h>
#include <time.h>
#include <pthread.h>


#include "bstd.h"
#include "bkni.h"

#include "celth_stddef.h"
#include "celth_rtos.h"

#include "celth_mutex.h"







CELTHOSL_SemId_t CELTH_SemCreate	( char*	pcName,
											CELTHOSL_SemType_t	eSemType ,
											int				iInitValue )
{

	
	CELTHOSL_Sem_t*	pSem	= NULL;

	pSem	= (CELTHOSL_Sem_t *)CELTHOSL_MemoryAlloc(sizeof(CELTHOSL_Sem_t));
	if( pSem != NULL )
	{
		pSem->eSemCreate	= CELTHOSL_kCreate;
		pSem->eSemType		= eSemType;
		pSem->uiSignature	= iInitValue;
		if(sem_init(&pSem->sem, 0, iInitValue)!=0)
		{
			CELTHOSL_MemoryFree(pSem);
			pSem	= NULL;
		}
	}

	return (CELTHOSL_SemId_t)pSem;


											
}

CELTHOSL_SemId_t CELTH_SemInit		( char*    pcName,
											CELTHOSL_Sem_t*			pSem ,
											CELTHOSL_SemType_t		eSemType ,
											int					iInitValue ) 
{

		return NULL;

}

CELTH_Status_t CELTH_SemDestroy	( CELTHOSL_SemId_t SemId) 
{

CELTHOSL_Sem_t*	pSem	= (CELTHOSL_Sem_t *)SemId;

if( pSem != NULL )
{
	sem_destroy(&pSem->sem );
	if( pSem->eSemCreate == CELTHOSL_kCreate )
	{
		CELTHOSL_MemoryFree(pSem);
	}

	return	CELTH_NO_ERROR;
}

return CELTH_ERROR_BAD_PARAMETER;


}
CELTH_Status_t CELTH_SemWait	( CELTHOSL_SemId_t SemId, int iMilliseconds ) 
{

CELTHOSL_Sem_t*	pSem	= (CELTHOSL_Sem_t *)SemId;

struct timespec ts;

int ss=0;





if( pSem != NULL )
{
	if( iMilliseconds == CELTHOSL_WaitForEver )
	{
		 sem_wait(&pSem->sem );
	}
	else
	{

		
		if(clock_gettime(CLOCK_REALTIME, &ts) == -1)
		{
			return CELTH_ERROR_DEVICE_BUSY;
		}
		
		ts.tv_sec += iMilliseconds;
		ss=sem_timedwait(&pSem->sem,&ts);
	}

	if( ss == -1 )
	{
		return	CELTH_ERROR_TIMEOUT;
	}
	else
	{
		pSem->uiSignature--;
		
		return	CELTH_NO_ERROR;
	}
}

return CELTH_ERROR_BAD_PARAMETER;


}
CELTH_Status_t CELTH_SemPost	( CELTHOSL_SemId_t SemId ) 
{

CELTHOSL_Sem_t*		pSem	= (CELTHOSL_Sem_t *)SemId;

if( pSem != NULL )
{
	if( sem_post( &pSem->sem)==0 )
	{
		pSem->uiSignature++;
		
		return	CELTH_NO_ERROR;
	}
	else
	{
		return CELTH_ERROR_FAIL;
	}
}

return CELTH_ERROR_BAD_PARAMETER;



}

/* --- Mutex Functions --------------------------------------------------------*/
CELTHOSL_MutexId_t  CELTH_MutexCreate  ( CELTHOSL_MutexType_t eMutextype )
{

CELTHOSL_Mutex_t*	pMutex	= NULL;

pMutex	= (CELTHOSL_Mutex_t *)CELTHOSL_MemoryAlloc(sizeof(CELTHOSL_Mutex_t));
if( pMutex != NULL )
{
	pMutex->eMutexCreate	= CELTHOSL_kCreate;

	/*pMutex->hMutex = PTHREAD_MUTEX_INITIALIZER;*/

	pthread_mutex_init(&pMutex->hMutex,NULL);

}

return (CELTHOSL_MutexId_t)pMutex;

}
CELTHOSL_MutexId_t  CELTH_MutexInit    ( CELTHOSL_Mutex_t *pMutex, CELTHOSL_MutexType_t eMutextype )
{

	return NULL;
}

CELTH_Status_t    CELTH_MutexDestroy( CELTHOSL_MutexId_t MutexId )
{



CELTHOSL_Mutex_t*	pMutex	= (CELTHOSL_Mutex_t *)MutexId;

if( pMutex != NULL )
{
	pthread_mutex_destroy( &pMutex->hMutex );
	if( pMutex->eMutexCreate == CELTHOSL_kCreate )
	{
		CELTHOSL_MemoryFree(pMutex);
	}

	return	CELTH_NO_ERROR;
}

return CELTH_ERROR_BAD_PARAMETER;




}
CELTH_Status_t    CELTH_MutexLock  ( CELTHOSL_MutexId_t MutexId )
{

CELTHOSL_Mutex_t*	pMutex	= (CELTHOSL_Mutex_t *)MutexId;

if( pMutex != NULL )
{
	pthread_mutex_lock(&pMutex->hMutex);

	return CELTH_NO_ERROR;

}
return CELTH_ERROR_BAD_PARAMETER;


}
CELTH_Status_t    CELTH_MutexUnLock( CELTHOSL_MutexId_t MutexId)
{

CELTHOSL_Mutex_t*	pMutex	= (CELTHOSL_Mutex_t *)MutexId;

if( pMutex != NULL )
{
	pthread_mutex_unlock(&pMutex->hMutex);

	return CELTH_NO_ERROR;

}
return CELTH_ERROR_BAD_PARAMETER;

}


