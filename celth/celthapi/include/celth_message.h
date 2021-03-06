#ifndef	_CELTH_MESSAGE_H_
#define	_CELTH_MESSAGE_H_

/* Includes -------------------------------------------------------------------*/


#ifdef __cplusplus /* celth message queue */
extern "C" {
#endif

typedef struct CELTH_MessageQueue_s	CELTH_MessageQueue_t;

typedef	void (*celth_message_queue_class_t)(CELTH_MessageQueue_t *queue);

typedef struct celth_message_hdr_s
{
	struct celth_message_hdr_s* message_hdr_next;
} celth_message_hdr_t;

typedef struct celth_message_half_s
{
	celth_message_hdr_t*		message_half_head;
	celth_message_hdr_t*		message_half_tail;
	CELTHOSL_SemId_t			message_half_semid;
	CELTHOSL_MutexId_t		message_half_mutex;
} celth_message_half_t;

struct CELTH_MessageQueue_s
{
	celth_message_queue_class_t	message_queue_class;
	struct CELTH_MessageQueue_s*	message_queue_next;
	celth_message_half_t			message_queue_free;
	celth_message_half_t			message_queue_queue;
	int                     message_size;
	void*					message_queue_memory;
};

#define CELTH_MESSAGE_MEMSIZE_QUEUE(maxMessageSize, maxMessages) \
  (((((maxMessageSize) + 3) & ~3) + sizeof(celth_message_hdr_t)) * (maxMessages))

extern	void         CELTHMessage_Send			(CELTH_MessageQueue_t* MessageQueue, void* Message);
extern	void*        CELTHMessage_Receive			(CELTH_MessageQueue_t* MessageQueue);
extern	void*        CELTHMessage_Receive_timeout (CELTH_MessageQueue_t* MessageQueue, const unsigned int time);
extern	void*        CELTHMessage_Claim			(CELTH_MessageQueue_t* MessageQueue);
extern	void*        CELTHMessage_Claim_Timeout	(CELTH_MessageQueue_t* MessageQueue, const  unsigned int time);
extern	void         CELTHMessage_Release			(CELTH_MessageQueue_t* MessageQueue, void* Message);

extern	void			CELTHMessage_Init_Queue			(CELTH_MessageQueue_t* MessageQueue, void* memory, int ElementSize, unsigned int NoElements);
extern	CELTH_MessageQueue_t* CELTHMessage_Create_Queue			(int ElementSize, unsigned int NoElements);
extern	void			CELTHMessage_Init_Queue_Timeout	(CELTH_MessageQueue_t* MessageQueue, void* memory, int ElementSize, unsigned int NoElements);
extern	CELTH_MessageQueue_t* CELTHMessage_Create_Queue_Timeout	(int ElementSize, unsigned int NoElements);
extern	void			CELTHMessage_Delete_Queue			(CELTH_MessageQueue_t* MessageQueue);

void CELTHMessage_Queue_Initialize(void);




/*****************************************************************/
/* extern  message function*/
/*****************************************************************/

extern  CELTH_MessageQueue_t*  CELTH_MsgCreate(int ElementSize, unsigned int NoElements);
extern	CELTH_Status_t	CELTH_MsgSend				(CELTH_MessageQueue_t* pMsgQueue, CELTH_VOID* pMsg ) ;
extern	CELTH_Status_t	CELTH_MsgSendWithTimeOut	( CELTH_MessageQueue_t* pMsgQueue, CELTH_VOID* pMsg, int iMilliseconds ) ;
extern	CELTH_Status_t	CELTH_MsgCapabilityGet	( CELTH_MessageQueue_t* pMsgQueue, int* piFreeMsgNbr );
extern	CELTH_Status_t	CELTH_MsgWait				( CELTH_MessageQueue_t* pMsgQueue,CELTH_VOID* pMsg, int iMilliseconds );





#ifdef __cplusplus /* celth message queue */
}
#endif

#endif
