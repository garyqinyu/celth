/*#include <stdlib.h>*/
#include <stdio.h>


/*#include "celth_rtos.h"*/
#include "celth_mutex.h"

#include "celth_message.h"


CELTH_MessageQueue_t *celth_message_queue_list = 0;
CELTHOSL_SemId_t		celth_message_queue_list_semaphore;

/*{{{  static void message_half_add(message_half_t* MessageHalf, void* UsrMessage)*/
static void	celth_message_half_add(celth_message_half_t* MessageHalf, void* UsrMessage)
{
	celth_message_hdr_t* Message = (celth_message_hdr_t*)UsrMessage - 1;

/*	CELTHAssert(MessageHalf != NULL);
	CELTHAssert(UsrMessage != NULL);*/

if((MessageHalf == NULL)||(UsrMessage == NULL))
{
return ;
}
	/* other assertions defered to semaphore_signal */

	CELTH_MutexLock(MessageHalf->message_half_mutex);

	Message->message_hdr_next = NULL;
	if (MessageHalf->message_half_head == NULL)
	{
		MessageHalf->message_half_head = Message;
	}
	else
	{
		MessageHalf->message_half_tail->message_hdr_next = Message;
	}
	MessageHalf->message_half_tail = Message;

	CELTH_MutexUnLock(MessageHalf->message_half_mutex);

	CELTH_SemPost(MessageHalf->message_half_semid);
}
/*}}}  */

/*{{{  static void* message_half_remove(message_half_t* MessageHalf)*/
static  void*	celth_message_half_remove(celth_message_half_t* MessageHalf)
{
	celth_message_hdr_t* Message;

	/* other assertions defered to semaphore_wait */

	if(MessageHalf == NULL)
		{
		return NULL;
		}

	if (CELTH_SemWait (MessageHalf->message_half_semid, CELTHOSL_WaitForEver) != CELTH_NO_ERROR)
	{
		return NULL;
	}

	CELTH_MutexLock(MessageHalf->message_half_mutex);

	Message                        = MessageHalf->message_half_head;
	MessageHalf->message_half_head = Message->message_hdr_next;

	CELTH_MutexUnLock(MessageHalf->message_half_mutex);

	return Message + 1;
}
/*}}}  */


/*{{{  void message_send (FEI_MessageQueue_t*  MessageQueue, void* Message)*/
void CELTHMessage_Send(CELTH_MessageQueue_t*  MessageQueue, void* Message)
{

	if(MessageQueue ==NULL)
		{
		return;
		}
	/* other assertions defered to message_half_remove */
	celth_message_half_add(&MessageQueue->message_queue_queue, Message);
}
/*}}}  */

/*{{{  void* message_receive (FEI_MessageQueue_t* MessageQueue)*/
void* CELTHMessage_Receive(CELTH_MessageQueue_t* MessageQueue)
{

	
	if(MessageQueue ==NULL)
		{
		return NULL;
		}
	/* other assertions defered to message_half_remove */
	return celth_message_half_remove(&MessageQueue->message_queue_queue);
}
/*}}}  */

/*{{{  void* message_claim (FEI_MessageQueue_t* MessageQueue)*/
void* CELTHMessage_Claim (CELTH_MessageQueue_t* MessageQueue)
{

	if(MessageQueue ==NULL)
		{
		return NULL;
		}

	
	/* other assertions defered to message_half_remove */
	return celth_message_half_remove(&MessageQueue->message_queue_free);
}
/*}}}  */
/*{{{  void message_release (FEI_MessageQueue_t* MessageQueue, void* Message)*/
void CELTHMessage_Release (CELTH_MessageQueue_t* MessageQueue, void* Message)
{

	
	if(MessageQueue ==NULL)
		{
		return;
		}
	/* other assertions defered to message_half_add */
	celth_message_half_add(&MessageQueue->message_queue_free, Message);
}
/*}}}  */


/*{{{  void message_uninit_queue(FEI_MessageQueue_t* queue)*/
static	void celth_message_uninit_queue(CELTH_MessageQueue_t* MessageQueue)
{
	CELTH_MessageQueue_t* prev;

	CELTH_SemWait(celth_message_queue_list_semaphore, CELTHOSL_WaitForEver);

	if (celth_message_queue_list == MessageQueue) 
	{
		celth_message_queue_list = MessageQueue->message_queue_next;
	} 
	else 
	{
		for (prev = celth_message_queue_list; prev != 0; prev = prev->message_queue_next) 
		{
			if (prev->message_queue_next == MessageQueue) 
			{
				prev->message_queue_next = MessageQueue->message_queue_next;
				break;
			}
		}
	}
	
	/* if go through the queue, could not found the right one*/
	{
	
	}

	CELTH_SemDestroy(celth_message_queue_list_semaphore);

	CELTH_SemDestroy(MessageQueue->message_queue_free.message_half_semid);
	CELTH_MutexDestroy(MessageQueue->message_queue_free.message_half_mutex);
	CELTH_SemDestroy(MessageQueue->message_queue_queue.message_half_semid);
	CELTH_MutexDestroy(MessageQueue->message_queue_queue.message_half_mutex);
}
/*}}}  */


static	void celth_message_init_generic(CELTH_MessageQueue_t* MessageQueue, void* memory,
								int ElementSize, unsigned int NoElements)
{
	celth_message_hdr_t** PrevMsgP;
	celth_message_hdr_t*  CurrMsg;
	unsigned int			i;
	int			Needed;

	/*CELTHAssert(MessageQueue != NULL);
	CELTHAssert(memory != NULL);*/

if(MessageQueue==NULL)
{
return;
}
if(memory==NULL)
{
return;
}


	/*lis FATAL_ERROR_IF_NOT_TASK();
	FATAL_ERROR_IF_LOCKED();*/

	
	MessageQueue->message_size = ElementSize;

	ElementSize = (ElementSize + 3) & ~3;
	Needed = ElementSize + sizeof(celth_message_hdr_t);

	CurrMsg = (celth_message_hdr_t*)memory;  
	PrevMsgP = &MessageQueue->message_queue_free.message_half_head;

	for (i = 1; i < NoElements; i++) 
	{
		*PrevMsgP = CurrMsg;
		PrevMsgP = &CurrMsg->message_hdr_next;
		CurrMsg = (celth_message_hdr_t*)((char*)CurrMsg + Needed);
	}
	*PrevMsgP = CurrMsg;
	MessageQueue->message_queue_free.message_half_tail = CurrMsg;
	CurrMsg->message_hdr_next = NULL;

	MessageQueue->message_queue_queue.message_half_head = NULL;
	MessageQueue->message_queue_queue.message_half_tail = NULL;

	MessageQueue->message_queue_free.message_half_semid		= CELTH_SemCreate("sem", CELTHOSL_FifoTimeOut, NoElements);
	MessageQueue->message_queue_free.message_half_mutex		= CELTH_MutexCreate( CELTHOSL_FifoNoTimeOut );
	MessageQueue->message_queue_queue.message_half_semid	= CELTH_SemCreate("sem", CELTHOSL_FifoTimeOut, 0);
	MessageQueue->message_queue_queue.message_half_mutex	= CELTH_MutexCreate( CELTHOSL_FifoNoTimeOut );

	MessageQueue->message_queue_memory = memory;

	MessageQueue->message_queue_class = celth_message_uninit_queue;

	CELTH_SemWait(celth_message_queue_list_semaphore, CELTHOSL_WaitForEver);

	MessageQueue->message_queue_next = celth_message_queue_list;
	celth_message_queue_list = MessageQueue;

	CELTH_SemPost(celth_message_queue_list_semaphore);
}
/*}}}  */


void CELTHMessage_Init_Queue(CELTH_MessageQueue_t* MessageQueue, void* memory,
                        int ElementSize, unsigned int NoElements)
{
	/* assertions defered to message_init_queue */
	celth_message_init_generic(MessageQueue, memory, ElementSize, NoElements);
}

static	void celth_message_uncreate_queue(CELTH_MessageQueue_t* MessageQueue)
{
	celth_message_uninit_queue(MessageQueue);
	/*BKNI_Free(MessageQueue->message_queue_memory);
	BKNI_Free(MessageQueue);*/
	
	CELTHOSL_MemoryFree(MessageQueue->message_queue_memory);
	CELTHOSL_MemoryFree(MessageQueue);
	
	
}


static	CELTH_MessageQueue_t* celth_message_create_generic (int ElementSize, unsigned int NoElements)
{
	CELTH_MessageQueue_t* MessageQueue;
	void* messages;
	int Needed;

	/*lis FATAL_ERROR_IF_NOT_TASK();
	FATAL_ERROR_IF_LOCKED();*/

	/* This calculation needs to be the same as the one performed in
	 * message_queue_init() */
	ElementSize = (ElementSize + 3) & ~3;
	Needed = ElementSize + (int)sizeof(celth_message_hdr_t);

/*	messages = BKNI_Malloc(Needed * NoElements);*/
	messages = CELTHOSL_MemoryAlloc(Needed * NoElements);
	
	if (messages == NULL)
		return NULL;

	/*MessageQueue = (CELTH_MessageQueue_t *)BKNI_Malloc(sizeof(CELTH_MessageQueue_t));*/
	
		MessageQueue = (CELTH_MessageQueue_t *)CELTHOSL_MemoryAlloc(sizeof(CELTH_MessageQueue_t));
	if (MessageQueue == NULL) 
	{
		/*BKNI_Free(messages);*/
		CELTHOSL_MemoryFree(messages);
	} 
	else 
	{
		celth_message_init_generic(MessageQueue, messages, ElementSize, NoElements);
		MessageQueue->message_queue_class = celth_message_uncreate_queue;
	}

	return MessageQueue;
}


CELTH_MessageQueue_t* CELTHMessage_Create_Queue(int ElementSize, unsigned int NoElements)
{
	/* assertions defered to message_create_generic */
	return celth_message_create_generic(ElementSize, NoElements);
}


static void* celth_message_half_remove_timeout(celth_message_half_t* MessageHalf, const int time)
{
	celth_message_hdr_t*	Message;
	int	result;


	
	if(MessageHalf ==NULL)
		{
		return NULL;
		}
	/* other assertions defered to semaphore_wait_timeout */

	result = CELTH_SemWait(MessageHalf->message_half_semid, time);
	if (result != CELTH_NO_ERROR) 
	{
		return NULL;
	}

	CELTH_MutexLock(MessageHalf->message_half_mutex);

	Message                        = MessageHalf->message_half_head;
	MessageHalf->message_half_head = Message->message_hdr_next;

	CELTH_MutexUnLock(MessageHalf->message_half_mutex);

	return Message + 1;
}

void* CELTHMessage_Receive_timeout (CELTH_MessageQueue_t* MessageQueue, const unsigned int time)
{

	
	if(MessageQueue ==NULL)
		{
		return NULL;
		}
	/* other assertions defered to message_half_remove_timeout */
	return celth_message_half_remove_timeout(&MessageQueue->message_queue_queue, time);
}


void* CELTHMessage_Claim_Timeout (CELTH_MessageQueue_t* MessageQueue, const unsigned int time)
{

  if(MessageQueue ==NULL)
	  {
	  return NULL;
	  }

  
  /* other assertions defered to message_half_remove_timeout */
  return celth_message_half_remove_timeout(&MessageQueue->message_queue_free, time);
}


void CELTHMessage_Init_Queue_Timeout(CELTH_MessageQueue_t* MessageQueue, void* memory,
                        int ElementSize, unsigned int NoElements)
{
  /* assertions defered to message_init_generic */
  celth_message_init_generic(MessageQueue, memory, ElementSize, NoElements);
}

CELTH_MessageQueue_t* CELTHMessage_Create_Queue_Timeout(int ElementSize, unsigned int NoElements)
{
  /* assertions defered to message_create_generic */
  return celth_message_create_generic(ElementSize, NoElements);
}

/*{{{  void message_delete_queue (message_queue_t* MessageQueue)*/
void CELTHMessage_Delete_Queue (CELTH_MessageQueue_t* MessageQueue)
{


	if(MessageQueue ==NULL)
		{
		return ;
		}

	
	/*lis FATAL_ERROR_IF_NOT_TASK();
	FATAL_ERROR_IF_LOCKED();*/

  MessageQueue->message_queue_class(MessageQueue);
}
/*}}}  */

void CELTHMessage_Queue_Initialize(void)
{
	celth_message_queue_list_semaphore	= CELTH_SemCreate("queue", CELTHOSL_FifoNoTimeOut, 1);
}


/***************************************************************************************/

/* the extern interface of message queue;
/****************************************************************************************/

 extern CELTH_MessageQueue_t*	CELTH_MsgCreate(int ElementSize, unsigned int NoElements)
 {
 	return CELTHMessage_Create_Queue( ElementSize, NoElements );
 }


 extern  CELTH_Status_t  CELTH_MsgSend( CELTH_MessageQueue_t* pMsgQueue,  CELTH_VOID* pMsg ) 
{


CELTH_Status_t	Status	= CELTH_ERROR_INVALID_HANDLE;

CELTH_VOID			*pBuf		= NULL;


if(pMsgQueue)
{

pBuf	= CELTHMessage_Claim( pMsgQueue );
CELTHOSL_Memcpy( pBuf, pMsg, pMsgQueue->message_size );
CELTHMessage_Send( pMsgQueue, pBuf );
Status = CELTH_NO_ERROR;

}

return Status;

}
 extern  CELTH_Status_t  CELTH_MsgSendWithTimeOut ( CELTH_MessageQueue_t* pMsgQueue, CELTH_VOID* pMsg,int iMilliseconds )
{

CELTH_Status_t	Status	= CELTH_ERROR_INVALID_HANDLE;

CELTH_VOID			*pBuf		= NULL;

if(pMsgQueue)
{

if( iMilliseconds == CELTHOSL_WaitForEver )
{
	pBuf	= CELTHMessage_Claim( pMsgQueue );
}
else
{
	pBuf	= CELTHMessage_Claim_Timeout( pMsgQueue, iMilliseconds );
	if( pBuf == NULL )
	{
		return CELTH_ERROR_TIMEOUT;
	}
}


CELTHOSL_Memcpy( pBuf, pMsg, pMsgQueue->message_size );
CELTHMessage_Send( pMsgQueue, pBuf );
Status = CELTH_NO_ERROR;


}
return Status;


}
 extern  CELTH_Status_t  CELTH_MsgCapabilityGet (  CELTH_MessageQueue_t* pMsgQueue, int* piFreeMsgNbr )
{

CELTH_Status_t	Status	= CELTH_NO_ERROR;

*piFreeMsgNbr	= 8;

return Status;

}
 extern  CELTH_Status_t  CELTH_MsgWait	(  CELTH_MessageQueue_t* pMsgQueue,CELTH_VOID* pMsg, int iMilliseconds )
{


CELTH_Status_t	Status	= CELTH_ERROR_INVALID_HANDLE;


CELTH_VOID			*pBuf		= NULL;


if(pMsgQueue)
{
	
if( iMilliseconds == CELTHOSL_WaitForEver )
{
	pBuf	= CELTHMessage_Receive( pMsgQueue );
	CELTHOSL_Memcpy( pMsg, pBuf, pMsgQueue->message_size );
	CELTHMessage_Release( pMsgQueue, pBuf );
}
else
{
	pBuf	= CELTHMessage_Receive_timeout( pMsgQueue, (int )iMilliseconds );
	if( pBuf == NULL )
	{
		Status	= CELTH_ERROR_TIMEOUT;
	}
	else
	{
		CELTHOSL_Memcpy( pMsg, pBuf, pMsgQueue->message_size );
		CELTHMessage_Release( pMsgQueue, pBuf );
		Status = CELTH_NO_ERROR;
	}
}
}
else
{
	Status = CELTH_ERROR_INVALID_HANDLE;
}

return Status;

}




