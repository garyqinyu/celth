/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// Notes: J. Burban for PEG test
//
// 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifndef _PEGOS20_
#define _PEGOS20_
#include "task.h"
#include "message.h"
#include "osal.h"
#include "stosd.h"
//#include "pmessage.hpp"

//#define OS20_MSG_ELEMENT_SIZE      ((size_t) sizeof(struct PegMessage))
//#define OS20_MSG_NB_ELEMENT        ((unsigned int)  100) /*to be adjusted */

//#define	PEG_QUEUE_TYPE		           message_queue_t *
//#define CREATE_MESG_QUEUE	           (message_create_queue( OS20_MSG_ELEMENT_SIZE, OS20_MSG_NB_ELEMENT))
//#define DELETE_MESSAGE_QUEUE(pstQUEUE) (message_delete_queue(pstQUEUE))
//#define ENQUEUE_TASK_MESSAGE(pstMESSAGE,pstQUEUE) (message_send(pstQUEUE, (void*) pstMESSAGE))
//#define MSG							   PEG_QUEUE_TYPE/*to be done retrieve task pointeur given at creation*/

//#define PEG_TASK_TYPE	               OSAL_TaskId_t
//#define CURRENT_TASK                   (OSAL_TaskIdSelf ())
//#define PEG_TASK_PTR                   NULL/*to be done retrieve task pointeur given at creation*/


//#define LOCK_MESSSAGE_QUEUE            /*nothing to do*/
//#define UNLOCK_MESSSAGE_QUEUE          /*nothing to do*/
#define LOCK_PEG                       /*nothing to do?*/
#define UNLOCK_PEG                     /*nothing to do?*/
#define LOCK_TIMER_LIST                /*nothing to do?*/
#define UNLOCK_TIMER_LIST              /*nothing to do?*/


#endif

