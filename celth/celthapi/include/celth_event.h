#ifndef _CELTH_EVENT_H_
#define _CELTH_EVENT_H_

#ifdef __cplusplus

extern "C" {

#endif


#include "celth_stddef.h"



/* --- Event Typedef ------------------------------------------------------ */
typedef enum
{
	CELTHOSL_EVENT_OP_AND,
	CELTHOSL_EVENT_OP_OR
} CELTH_EventOperation_t;
/*
typedef struct Event_Struct{
CELTH_UINT  dummy;
}Event_Struct;
*/
typedef struct Celth_Event_Obj*	CELTH_EventId_t ;

typedef struct Celth_Event_GroupObj *CELTH_EventGroup_t;


/* --- Event Functions --------------------------------------------------------*/
extern	CELTH_Error_Code	CELTH_CreateEvent		( CELTH_EventId_t *pevent );
extern	CELTH_VOID	CELTH_DestroyEvent		( CELTH_EventId_t event );
extern	CELTH_VOID	CELTH_ResetEvent	( CELTH_EventId_t event );
extern	CELTH_VOID	CELTH_SetEvent		( CELTH_EventId_t event );
extern  CELTH_Error_Code  CELTH_WaitForEvent(CELTH_EventId_t event, CELTH_INT timeoutMsec);
extern CELTH_Error_Code CELTH_CreateEventGroup(CELTH_EventGroup_t *pGroup);
extern CELTH_Error_Code CELTH_AddEventGroup(	CELTH_EventGroup_t group,CELTH_EventId_t event );
extern CELTH_Error_Code CELTH_RemoveEventGroup(CELTH_EventGroup_t group,CELTH_EventId_t event);

extern CELTH_Error_Code CELTH_WaitForGroup(
		CELTH_EventGroup_t group, /* event group */
		CELTH_INT timeoutMsec,  /* timeout in milliseconds, use BKNI_INFINITE to wait without timeout */
		CELTH_EventId_t *events, /* [out] pass out the events that were triggered
							(specified by *nevents) */
		CELTH_UNSIGNED max_events,  /* the maximum number of events which can be passed out using
							the events parameter. Generally this should be equal to the
							number of events in the BKNI_EventGroup. */
		CELTH_UNSIGNED *nevents /* [out] number of events which were stored in the user specified area */
		);

#ifdef __cplusplus

}

#endif



#endif

