


#include <stdlib.h>   
#include <stdio.h>   
#include <errno.h>   
#include <pthread.h> 

/*#include <sys/syscall.h>*/

#include "bstd.h"
#include "bkni.h"

#include "celth_stddef.h"
#include "celth_rtos.h"

#include "celth_mutex.h"

#include "celth_message.h"

#include "celth_debug.h"




static CELTHOSL_MutexId_t  etaskMutex;



CELTHOSL_TaskInfo_t*		pHeadTaskInfo	= NULL;
/*
pid_t gettid(void)  
{  
  return syscall(__NR_gettid);  
} */ 



CELTH_CHAR* pVersion = "celth_v0.1"; 

extern	CELTH_INT	CELTHOSL_Init		( CELTH_VOID ) 
{

etaskMutex=CELTH_MutexCreate(CELTHOSL_FifoNoTimeOut);

if(etaskMutex==NULL)
{
return CELTH_ERROR_FAIL;
}
	
CELTHMessage_Queue_Initialize();


return CELTH_NO_ERROR;
}
extern	CELTH_INT	CELTHOSL_Terminate	( CELTH_VOID ) 
{
return CELTH_NO_ERROR;
}

extern	CELTH_CONST	CELTH_CHAR*	CELTHOSL_GetVersion( CELTH_VOID ) 
{
 return pVersion;

}





extern	CELTHOSL_TaskId_t		CELTHOSL_TaskCreate	( CELTH_CONST CELTH_CHAR*		pcName,
														CELTHOSL_EntryPoint_t	pfEntryPoint,
														CELTH_VOID*				pParam,
														CELTH_INT					iStackSize,
														CELTH_INT					iPriority,
														CELTH_INT                   iMsgSize,
														CELTH_INT                   iQueueSize
 )
 {
 
 
	pthread_attr_t attr;   

	int err;
 
	struct sched_param param;
   
	size_t stacksize;
/*	size_t guardsize=512;*/
   
   
	int Minpriority,Maxpriority;

   CELTHOSL_TaskInfo_t* pTaskInfo   = NULL;


   pTaskInfo   = (CELTHOSL_TaskInfo_t *)CELTHOSL_MemoryAlloc(sizeof(CELTHOSL_TaskInfo_t));

   

 CELTH_MutexLock(etaskMutex);
	
   
   if(pTaskInfo!=NULL)
   {

		if(iMsgSize*iQueueSize)
		{

		
			pTaskInfo->pMessageQueue	= CELTHMessage_Create_Queue( iMsgSize, iQueueSize );


			
			if( pTaskInfo->pMessageQueue != NULL )
			{
				
				pTaskInfo->iQueueSize	= iQueueSize;
				pTaskInfo->iMsgSize		= iMsgSize;
				
		
   
				{
				    pthread_attr_init(&attr);


				    pthread_attr_setschedpolicy(&attr, SCHED_RR);

				   Maxpriority = sched_get_priority_max( SCHED_RR );
				   Minpriority = sched_get_priority_min( SCHED_RR );

				   if((iPriority>Maxpriority)||(iPriority<Minpriority))
				   {
						iPriority = Minpriority;
				   }


				   
				   

					pthread_attr_setguardsize(&attr,512);

					if(iStackSize!=0)
					{
					
					
					stacksize=iStackSize;

							
					pthread_attr_setstacksize(&attr, stacksize);


					}

				    param.sched_priority = iPriority;

				    pthread_attr_setschedparam(&attr, &param);
				 
				     
				   	err=pthread_create (&pTaskInfo->taskid, &attr, pfEntryPoint, pParam);
					CelthApi_Debug_Msg("create the pTaskInfo[%x]with taskid[%x]\n",pTaskInfo,pTaskInfo->taskid);  

					if(err!=0)
						{
						
							CELTHMessage_Delete_Queue( (CELTH_MessageQueue_t *)pTaskInfo->pMessageQueue );
							CELTHOSL_MemoryFree(pTaskInfo);

							pTaskInfo=NULL;

						}
					else
						{

							

							pTaskInfo->pNext		= pHeadTaskInfo;
							pHeadTaskInfo			= pTaskInfo;
						}
		   		}
			}
			else
			{
			
				CELTHOSL_MemoryFree(pTaskInfo);
				pTaskInfo	= NULL;
			}

		
		}
		else
		{
			pthread_t tt;

			pTaskInfo->pMessageQueue	= NULL;
			pTaskInfo->iQueueSize		= iQueueSize;
			pTaskInfo->iMsgSize 		= iMsgSize;

			
			
			{
				err=pthread_attr_init(&attr);
				if(err!=0)
				{
					CelthApi_Debug_Msg("\ncall pthread_attr_init fail\n");
					CELTHOSL_MemoryFree(pTaskInfo);
					pTaskInfo=NULL;
					return NULL;
				}
			
			
				pthread_attr_setschedpolicy(&attr, SCHED_RR);
			
			   Maxpriority = sched_get_priority_max( SCHED_RR );
			   Minpriority = sched_get_priority_min( SCHED_RR );
			
			   if((iPriority>Maxpriority)||(iPriority<Minpriority))
			   {
					iPriority = Minpriority;
			   }
			
			
			   
			   
			
				pthread_attr_setguardsize(&attr,512);
			
				if(iStackSize!=0)
				{
				stacksize=iStackSize;
				if(stacksize< PTHREAD_STACK_MIN)
				{
					stacksize= PTHREAD_STACK_MIN;
				}
			/*	if(stacksize>PTHREAD_STACK_MAX)
				{
					stacksize=PTHREAD_STACK_MAX;
				}*/
				err=pthread_attr_setstacksize(&attr, stacksize);
				if(err!=0)
				{
					CelthApi_Debug_Msg("\n create the pthread fail for set stacksize[%x]\n",stacksize);
					CELTHOSL_MemoryFree(pTaskInfo);
					pTaskInfo=NULL;
					return NULL;
	
				}
			
			
				}
			
				param.sched_priority = iPriority;
			
				pthread_attr_setschedparam(&attr, &param);

				 
				err=pthread_create (&pTaskInfo->taskid,&attr, pfEntryPoint, pParam);  
	
		
				if(err!=0)
					{
					
						CELTHOSL_MemoryFree(pTaskInfo);
			
						pTaskInfo=NULL;
			
					}
				else
					{

						CelthApi_Debug_Msg("\n create the pthread success!\n");
			
						pTaskInfo->pNext		= pHeadTaskInfo;
						pHeadTaskInfo			= pTaskInfo;
					}
			}

		
		}
   
  
  }
  
  
  CELTH_MutexUnLock(etaskMutex);

CelthApi_Debug_Msg("pTaskInfo=%x\n",pTaskInfo);
  
 return (CELTHOSL_TaskId_t) pTaskInfo;
 }






 
extern	CELTHOSL_TaskId_t		CELTHOSL_TaskId        ( CELTH_CONST CELTH_CHAR*	pucName )
{
return NULL;
}
extern	CELTHOSL_TaskId_t		CELTHOSL_TaskIdSelf    ( CELTH_VOID )
{

		pthread_t tid;

		
		CELTHOSL_TaskInfo_t* pTaskInfo	= NULL;

		CELTH_MutexLock(etaskMutex);

		tid = pthread_self();


		
		pTaskInfo	= pHeadTaskInfo;
		while( pTaskInfo != NULL )
		{
			printf("get the taskinfo is [%x]with taskid[%x],tid[%x]\n",pTaskInfo,pTaskInfo->taskid,tid);
			if( pTaskInfo->taskid == tid )
			{
				printf("break , get the right one pTaskInfo =[%x]\n",pTaskInfo);
				break;
			}

			pTaskInfo = pTaskInfo->pNext;
		}


		
		CELTH_MutexUnLock(etaskMutex);
		printf("return pTaskInfo is [%x]\n",pTaskInfo);
		
		return	(CELTHOSL_TaskId_t)pTaskInfo;

		

}

extern	CELTH_VOID                CELTHOSL_TaskDelay     ( CELTH_INT iDelay )
{
return NULL;
}
extern	CELTH_INT                 CELTHOSL_TaskDelete    ( CELTHOSL_TaskId_t pTaskId )
{

CELTHOSL_TaskInfo_t* pTaskInfo		= (CELTHOSL_TaskInfo_t *)pTaskId;
CELTHOSL_TaskInfo_t* plocTaskInfo	= pHeadTaskInfo;

pthread_t tid;

	CELTH_MutexLock(etaskMutex);


	if(pTaskInfo)
	{
		tid=pTaskInfo->taskid;
	
		pthread_join(tid, NULL);

		if( pTaskInfo->pMessageQueue != NULL )
		{
					CELTHMessage_Delete_Queue((CELTH_MessageQueue_t *)pTaskInfo->pMessageQueue );
		}


		if( plocTaskInfo != NULL )
				{
					if( plocTaskInfo == pTaskInfo )
					{
						pHeadTaskInfo	= pTaskInfo->pNext;
						CELTHOSL_MemoryFree( pTaskInfo );
					}
					else
					{
						while( plocTaskInfo->pNext != NULL )
						{
							if( plocTaskInfo->pNext == pTaskInfo )
							{
								plocTaskInfo->pNext	= pTaskInfo->pNext;

								CELTHOSL_MemoryFree( pTaskInfo );
								break;
							}

							plocTaskInfo	= plocTaskInfo->pNext;
						}
					}
				}

		

	}
	
	CELTH_MutexUnLock(etaskMutex);


return 0;
}
extern	CELTH_INT                 CELTHOSL_TaskSuspend   ( CELTHOSL_TaskId_t pTaskId )
{
return 0;
}
extern	CELTH_INT                 CELTHOSL_TaskResume    ( CELTHOSL_TaskId_t pTaskId )
{
return 0;
}

 
 
 extern CELTH_VOID CELTHOSL_Reboot(CELTH_VOID)
{
return NULL;
}

extern CELTH_Status_t CELTHOSL_GetTick(CELTH_DWORD* pdwTicks)
{
return 0;
}
 
/* 
extern	CELTH_Status_t	CELTHOSL_EventCreate(CELTHOSL_EventId_t* peEventId )
{


return BKNI_CreateEvent(peEventId);  

}
extern	CELTH_VOID	CELTHOSL_EventDelete( CELTHOSL_EventId_t  eEventId )
{
 return BKNI_DestroyEvent((BKNI_EventHandle)(eEventId));
}
extern	CELTH_VOID	CELTHOSL_ResetEvent( CELTHOSL_EventId_t  eEventId )
{
return BKNI_ResetEvent((BKNI_EventHandle)(eEventId));


}
extern	CELTH_VOID	CELTHOSL_SetEvent(CELTHOSL_EventId_t    eEventId )
{
return BKNI_SetEvent((BKNI_EventHandle)(eEventId));

}

extern  CELTH_Status_t  CELTHOSL_WaitForEvent(CELTHOSL_EventId_t eEventId, CELTH_INT iTimeoutMsec)
{
return BKNI_WaitForEvent((BKNI_EventHandle)(eEventId),iTimeoutMsec);
}
 
*/ 
 
 
 
/* memory function */

CELTH_VOID*   CELTHOSL_MemoryAlloc		( CELTH_INT iSize ) 
{


CELTH_VOID*	pvoid	= NULL;

pvoid	= malloc(iSize);

return	pvoid;


}
CELTH_VOID	CELTHOSL_MemoryFree		( CELTH_VOID* pBlock ) 
{

  free(pBlock);

}


CELTH_VOID * CELTHOSL_Memset(CELTH_VOID *b, CELTH_INT c, CELTH_INT len)
{
    return memset(b, c, (size_t)len);
}

CELTH_VOID * CELTHOSL_Memcpy(CELTH_VOID *dst, CELTH_CONST CELTH_VOID *src, CELTH_INT len)
{
    return memcpy(dst, src, (size_t)len);
}

CELTH_INT  CELTHOSL_Memcmp(CELTH_VOID* dst, CELTH_VOID* src, CELTH_INT len)
{
	return memcmp(dst,src,len);
}

CELTH_Status_t  CELTHOSL_Sleep(CELTH_INT imillisec)
{

#if 1
	
    struct timespec delay;
    struct timespec rem;
    int rc;



    delay.tv_sec = imillisec/1000;
    delay.tv_nsec = 1000 * 1000 * (imillisec%1000);

    for(;;) {
        rc = nanosleep(&delay, &rem); /* [u]sleep can't be used because it uses SIGALRM */
        if (rc!=0) {
            if (errno==EINTR) {
                delay = rem; /* sleep again */
                continue;
            }
            return CELTH_ERROR_OS_FAIL;
        }
        break; /* done */
    }

#endif

	



    return CELTH_NO_ERROR;
	
}


static unsigned long CELTH_P_GetMicrosecondTick(void)
{
#if !HAS_NPTL
    int rc;
    struct timeval now;
    rc = gettimeofday(&now, NULL);
    if (rc) {
        rc = CELTH_ERROR_OS_FAIL;
        return 0;
    }
    return now.tv_sec * 1000000 + now.tv_usec;
#else
    int rc;
    struct timespec now;
    /* It's ok to use clock_gettime even without NPTL. */
    rc = clock_gettime(CLOCK_MONOTONIC, &now);
    if (rc) {
        rc = CELTH_ERROR_OS_FAIL;
        return 0;
    }
    return now.tv_sec * 1000000 + now.tv_nsec / 1000;
#endif
}


CELTH_VOID CELTHOSL_Delay(CELTH_INT iMicrosec)
{

	
    unsigned long start;
    unsigned long diff;
    start = CELTH_P_GetMicrosecondTick();
    do {
        diff = CELTH_P_GetMicrosecondTick() - start;
    } while (diff < iMicrosec);
    return;
}






 
 
 
 
 
