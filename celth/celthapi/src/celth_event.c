

#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#include "celth_stddef.h"
#include "celth_event.h"

#include "celth_doublelist.h"




#if defined(__mips__)
#include <linux/version.h>
/* Broadcom integrated NPTL with our Linux 2.6.18 release. */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,18) && 0
#define HAS_NPTL 1
#endif /* LINUX_VERSION */
#endif /* __mips__ */

#if !HAS_NPTL
#include <sys/time.h>
#endif



struct Celth_Event_GroupObj
{
    CELTHLST_D_HEAD(group, Celth_Event_Obj) members;
    pthread_mutex_t lock;            /* mutex for protecting signal and conditional variables */
    pthread_cond_t  cond;           /* condition to wake up from event*/
};

struct Celth_Event_Obj
{
    CELTHLST_D_ENTRY(Celth_Event_Obj) list;
    struct Celth_Event_GroupObj *group;
    pthread_mutex_t lock;            /* mutex for protecting signal and conditional variables */
    pthread_cond_t  cond;           /* condition to wake up from event*/
    CELTH_BOOL signal;
};



#if CELTH_DEBUG_BUILD

static pthread_t g_csOwner;

#define SET_CRITICAL() do { g_csOwner = pthread_self(); } while (0)
#define CLEAR_CRITICAL() do { g_csOwner = (pthread_t)0; } while (0)
#define CHECK_CRITICAL() ( g_csOwner == pthread_self() )

#define ASSERT_CRITICAL() do \
{\
    if ( !CHECK_CRITICAL() )\
    {\
        printf("Error, must be in critical section to call %s\n", __FUNCTION__);\
        CELTH_Fail();\
    }\
} while (0)

#define ASSERT_NOT_CRITICAL() do \
{\
    if ( CHECK_CRITICAL() )\
    {\
        printf("Error, must not be in critical section to call %s\n", __FUNCTION__);\
        CELTH_Fail();\
    }\
} while (0)

#else

#define ASSERT_CRITICAL() (void)0
#define ASSERT_NOT_CRITICAL() (void)0
#define SET_CRITICAL() (void)0
#define CLEAR_CRITICAL() (void)0
#define CHECK_CRITICAL() 0

#endif




CELTH_VOID CELTH_Fail(CELTH_VOID)
{
    /* Derefering 0 will cause a SIGSEGV will usually produce a core dump. */
 printf("CELTH_Fail is intentionally causing a segfault. Please inspect any prior error messages or get a core dump stack trace to determine the cause of failure.\n");
    *(volatile unsigned char *)0;
}





CELTH_Error_Code

CELTH_CreateEvent_tagged(CELTH_EventId_t *pEvent, const char *file, int line)
{
    CELTH_EventId_t event;
    int rc;
    CELTH_Error_Code result=CELTHERR_SUCCESS;
    /* coverity[var_decl: FALSE] */
    pthread_condattr_t attr;

    ASSERT_NOT_CRITICAL();

    event = CELTHOSL_MemoryAlloc(sizeof(*event));
    *pEvent = event;
    if ( !event) {
        result = CELTHERR_ERROR_OS;
        goto err_no_memory;
    }

    rc = pthread_mutex_init (&event->lock, NULL /* default attributes */);
    if (rc!=0) {
        result = CELTHERR_ERROR_OS;
        goto err_mutex;
    }

    /* coverity[uninit_use_in_call: FALSE] */
    rc = pthread_condattr_init(&attr);
    if (rc!=0) {
        result = CELTHERR_ERROR_OS;
        goto err_condvar;
    }

#if HAS_NPTL
    rc = pthread_condattr_setclock(&attr, CLOCK_MONOTONIC);
    if (rc!=0) {
        result = CELTHERR_ERROR_OS;
        goto err_condvar;
    }
#endif

    rc = pthread_cond_init( &event->cond, &attr);
    if (rc!=0) {
        result = CELTHERR_ERROR_OS;
        goto err_condvar;
    }
    event->signal = FALSE;
    event->group = NULL;

    return result;

err_condvar:
    pthread_mutex_destroy(&event->lock);
err_mutex:
    
    CELTHOSL_MemoryFree(event);
err_no_memory:
    return result;
}

void
CELTH_DestroyEvent_tagged(CELTH_EventId_t event, const char *file, int line)
{
    int rc;
    CELTH_EventGroup_t group;

    ASSERT_NOT_CRITICAL();
    group = event->group;

    if (group) {
        printf("Event %#x still in the group %#x, removing it", (unsigned)(unsigned long)event, (unsigned)(unsigned long)group);
        rc = pthread_mutex_lock(&group->lock);
        if (rc!=0) {
            printf(("pthread_mutex_lock %d", rc));
        }
        /* if the group does not match, then the caller needs to fix their code. we can't have an event being added & removed from various
        groups and being destroyed at the same time. */

        CELTHLST_D_REMOVE(&group->members, event, list);
        pthread_mutex_unlock(&group->lock);
    }
    rc = pthread_mutex_destroy(&event->lock);
    if (rc!=0) {
        printf("pthread_mutex_destroy: %d", rc);
        
    }
    rc = pthread_cond_destroy(&event->cond);
    if (rc!=0) {
        printf("pthread_cond_destroy: %d", rc);
    }
    CELTHOSL_MemoryFree(event);
    return ;
}

#undef CELTH_CreateEvent
CELTH_Error_Code CELTH_CreateEvent(CELTH_EventId_t *pEvent)
{
    return CELTH_CreateEvent_tagged(pEvent, NULL, 0);
}

#undef CELTH_DestroyEvent
void CELTH_DestroyEvent(CELTH_EventId_t event)
{
    CELTH_DestroyEvent_tagged(event, NULL, 0);
}

/* return a timespec which is the current time plus an increment */
static int CELTH_P_SetTargetTime(struct timespec *target, int timeoutMsec)
{
    int rc;
#if !HAS_NPTL
    /* Unless pthread can set CLOCK_MONOTONIC, we cannot use clock_gettime(CLOCK_MONOTONIC). This is only available with NPTL linux. */
    struct timeval now;
    rc = gettimeofday(&now, NULL);
    if (rc!=0) {
        return CELTHERR_ERROR_OS ;
    }
    target->tv_nsec = now.tv_usec * 1000 + (timeoutMsec%1000)*1000000;
    target->tv_sec = now.tv_sec + (timeoutMsec/1000);
    if (target->tv_nsec >= 1000000000) {
        target->tv_nsec -=  1000000000;
        target->tv_sec ++;
    }
#else
    struct timespec now;
    rc = clock_gettime(CLOCK_MONOTONIC, &now);
    if (rc!=0) {
        return CELTHERR_ERROR_OS;
    }
    target->tv_nsec = now.tv_nsec + (timeoutMsec%1000)*1000000;
    target->tv_sec = now.tv_sec + (timeoutMsec/1000);
    if (target->tv_nsec >= 1000000000) {
        target->tv_nsec -=  1000000000;
        target->tv_sec ++;
    }
#endif
    return 0;
}

CELTH_Error_Code CELTH_WaitForEvent(CELTH_EventId_t event, CELTH_INT timeoutMsec)
{
    int rc;
    CELTH_Error_Code result = CELTHERR_SUCCESS;
    struct timespec target;

    if ( timeoutMsec != 0 )
    {
        ASSERT_NOT_CRITICAL();
    }

    if (timeoutMsec!=0 && timeoutMsec!=CELTH_INFINITE) {
        if (timeoutMsec<0) {
            /* If your app is written to allow negative values to this function, then it's highly likely you would allow -1, which would
            result in an infinite hang. We recommend that you only pass positive values to this function unless you definitely mean BKNI_INFINITE. */
            printf("CELTH_WaitForEvent given negative timeout. Possible infinite hang if timeout happens to be -1 (BKNI_INFINITE).");
        }
        if (timeoutMsec<100) {
            timeoutMsec=100; /* Wait at least 100 msec. If the kernel is busy servicing interrupts (e.g. heavy network traffic), it could starve
                                even the highest priority user mode thread. BKNI_WaitForEvent is used inside Magnum and much of that code cannot survive a timeout.
                                BKNI_WaitForGroup can have a smaller timeout because it's used outside of Magnum (e.g. Nexus) and timeouts are normal. */
        }
        rc = CELTH_P_SetTargetTime(&target, timeoutMsec);
        if (rc) {
            return CELTHERR_ERROR_OS;
        }
    }

    rc = pthread_mutex_lock(&event->lock);
    if (rc!=0) {
        return CELTHERR_ERROR_OS;
    }
    if (event->signal) {
        event->signal = FALSE;
        goto done;
    }
    if (timeoutMsec == 0) { /* wait without timeout */
        /* It is normal that BKNI_WaitForEvent could time out. Do not use BERR_TRACE. */
        result = CELTHERR_ERROR_TIMEOUT;
        goto done;
    }
    do {
        if (timeoutMsec == CELTH_INFINITE) {
            rc = pthread_cond_wait(&event->cond, &event->lock);
        } else {
            rc = pthread_cond_timedwait(&event->cond, &event->lock, &target);
            if (rc==ETIMEDOUT) {
               /* printf("CELTH_WaitForEvent(%#x): timeout", (unsigned)(unsigned long)event);*/
                result = CELTHERR_ERROR_TIMEOUT;
                goto done;
            }
        }
        if(rc==EINTR) {
            printf("CELTH_WaitForEvent(%#x): interrupted", (unsigned)(unsigned long)event);
            continue;
        }
        if (rc!=0) {
            result = CELTHERR_ERROR_OS;
            goto done;
        }
    } while(!event->signal);  /* we might have been wokenup and then event has been cleared */

    event->signal = FALSE;
done:
    pthread_mutex_unlock(&event->lock);
    return result;
}

CELTH_VOID CELTH_SetEvent(CELTH_EventId_t event)
{
    int rc;
    CELTH_EventGroup_t group;

    group = event->group;
    /* At this point, we may have been removed from the group and event->group is NULL.
    This is a real possibility because BKNI_SetEvent can be called from an ISR.
    Caching the group pointer allows us to safely unlock still. */

    if (group) {
        rc = pthread_mutex_lock(&group->lock);
        if(rc!=0)
        	{
        	
			printf("pthread_mutex_lock: %d", rc);
        	return ;
        	}
    }
    rc = pthread_mutex_lock(&event->lock);
    if (rc!=0) {
        printf("pthread_mutex_lock: %d", rc);
        return;
    }
    event->signal = TRUE;
    rc = pthread_cond_signal(&event->cond);
    if (rc!=0) {
        printf("pthread_cond_signal: %d", rc);
        return;
    }
    if (group) {
        rc = pthread_cond_signal(&group->cond);
        if (rc!=0) {
            printf("pthread_cond_signal: %d, ignored", rc);
        }
    }
    rc = pthread_mutex_unlock(&event->lock);
    if (rc!=0) {
        printf("pthread_mutex_unlock: %d", rc);
        return;
    }
    if (group) {
        pthread_mutex_unlock(&group->lock);
    }
    return ;
}

void CELTH_ResetEvent(CELTH_EventId_t event)
{
    int rc;

    rc = pthread_mutex_lock(&event->lock);
    if (rc!=0) {
        printf("pthread_mutex_lock: %d", rc);
        return;
    }
    event->signal = FALSE ;
    rc = pthread_mutex_unlock(&event->lock);
    if (rc!=0) {
        printf("pthread_mutex_unlock: %d", rc);
        return;
    }
    return ;
}




CELTH_Error_Code CELTH_CreateEventGroup(CELTH_EventGroup_t *pGroup)
{
    int rc;
    CELTH_EventGroup_t group;
    CELTH_Error_Code result;
    /* coverity[var_decl: FALSE] */
    pthread_condattr_t attr;

    ASSERT_NOT_CRITICAL();

    group = CELTHOSL_MemoryAlloc(sizeof(*group));
    if (!group) {
        result = CELTHERR_ERROR_NO_MEMORY;
        goto err_no_memory;
    }

    CELTHLST_D_INIT(&group->members);
    rc = pthread_mutex_init (&group->lock, NULL /* default attributes */);
    if (rc!=0) {
        result = CELTHERR_ERROR_OS;
        goto err_mutex;
    }

    /* coverity[uninit_use_in_call: FALSE] */
    rc = pthread_condattr_init(&attr);
    if (rc!=0) {
        result = CELTHERR_ERROR_OS;
        goto err_condvar;
    }

#if HAS_NPTL
    rc = pthread_condattr_setclock(&attr, CLOCK_MONOTONIC);
    if (rc!=0) {
        result = CELTHERR_ERROR_OS;
        goto err_condvar;
    }
#endif

    rc = pthread_cond_init( &group->cond, &attr);
    if (rc!=0) {
        result = CELTHERR_ERROR_OS;
        goto err_condvar;
    }
    *pGroup = group;

    return CELTHERR_SUCCESS;

err_condvar:
    pthread_mutex_destroy(&group->lock);
err_mutex:

    CELTHOSL_MemoryFree(group);
err_no_memory:
    return result;
}

CELTH_VOID CELTH_DestroyEventGroup(CELTH_EventGroup_t group)
{
    int rc;
    CELTH_EventId_t event;

    ASSERT_NOT_CRITICAL();

    rc = pthread_mutex_lock(&group->lock);
    if (rc<0) {
        printf("pthread_mutex_lock failed, rc=%d", rc);
        return;
    }

    while(NULL != (event=CELTHLST_D_FIRST(&group->members)) ) {
        event->group = NULL;
        CELTHLST_D_REMOVE_HEAD(&group->members, list);
    }
    pthread_mutex_unlock(&group->lock);
    /* NOTE: to avoid this race condition, app must ensure that no SetEvent for this group is pending at this time */
    pthread_mutex_destroy(&group->lock);
    pthread_cond_destroy(&group->cond);
    CELTHOSL_MemoryFree(group);
    return;
}


CELTH_Error_Code CELTH_AddEventGroup(CELTH_EventGroup_t group,CELTH_EventId_t event )
{
    int rc;
    CELTH_Error_Code result = CELTHERR_SUCCESS;

    ASSERT_NOT_CRITICAL();

    /* IMPORTANT: group lock shall be acquired before event lock */
    rc = pthread_mutex_lock(&group->lock);
    if (rc!=0) {
        printf("pthread_mutex_lock failed, rc=%d", rc);
        return rc;
    }
    rc = pthread_mutex_lock(&event->lock);
    if (rc!=0) {
        printf("pthread_mutex_lock failed, rc=%d", rc);
        return rc;
    }
    if (event->group != NULL) {
        printf("Event %#x already connected to the group %#x", (unsigned)(unsigned long)event, (unsigned)(unsigned long)group);
        result = CELTHERR_ERROR_OS;
    } else {
        CELTHLST_D_INSERT_HEAD(&group->members, event, list);
        event->group = group;
        if (event->signal) {
            /* signal condition if signal already set */
            pthread_cond_signal(&group->cond);
        }
    }
    rc = pthread_mutex_unlock(&event->lock);
    if (rc!=0) {
        printf("pthread_mutex_unlock failed, rc=%d", rc);
        return rc;
    }
    rc = pthread_mutex_unlock(&group->lock);
    if (rc!=0) {
        printf("pthread_mutex_unlock failed, rc=%d", rc);
        return rc;
    }
    return result;
}

CELTH_Error_Code CELTH_RemoveEventGroup(CELTH_EventGroup_t group,CELTH_EventId_t event)
{
    int rc;
    CELTH_Error_Code result = CELTHERR_SUCCESS;

    ASSERT_NOT_CRITICAL();

    rc = pthread_mutex_lock(&group->lock);
    if (rc!=0) {
        printf("pthread_mutex_lock failed, rc=%d", rc);
        return rc;
    }
    rc = pthread_mutex_lock(&event->lock);
    if (rc!=0) {
        printf("pthread_mutex_lock failed, rc=%d", rc);
        return rc;
    }
    if (event->group != group) {
        printf("Event %#x doesn't belong to the group %#x", event, group);
        result = CELTHERR_ERROR_OS;
    } else {
        CELTHLST_D_REMOVE(&group->members, event, list);
        event->group = NULL;
    }
    rc = pthread_mutex_unlock(&event->lock);
    if (rc!=0) {
        printf("pthread_mutex_lock failed, rc=%d", rc);
        return rc;
    }
    rc = pthread_mutex_unlock(&group->lock);
    if (rc!=0) {
        printf("pthread_mutex_lock failed, rc=%d", rc);
        return rc;
    }
    return result;
}

static CELTH_UNSIGNED group_get_events(CELTH_EventGroup_t group, CELTH_EventId_t *events, CELTH_UNSIGNED max_events)
{
    CELTH_EventId_t ev;
    int rc;
    unsigned event;


    for(event=0, ev=CELTHLST_D_FIRST(&group->members); ev && event<max_events ; ev=CELTHLST_D_NEXT(ev, list)) {
        rc = pthread_mutex_lock(&ev->lock);
        if (rc!=0) {
            printf("pthread_mutex_lock failed, rc=%d", rc);
            return 0;
        }
        if (ev->signal) {
            ev->signal = FALSE;
            events[event] = ev;
            event++;
        }
        rc = pthread_mutex_unlock(&ev->lock);
        if (rc!=0) {
            printf("pthread_mutex_unlock failed, rc=%d", rc);
            return 0;
        }
    }
    return event;
}

CELTH_Error_Code CELTH_WaitForGroup(
		CELTH_EventGroup_t group, /* event group */
		CELTH_INT timeoutMsec,  /* timeout in milliseconds, use BKNI_INFINITE to wait without timeout */
		CELTH_EventId_t *events, /* [out] pass out the events that were triggered
							(specified by *nevents) */
		CELTH_UNSIGNED max_events,  /* the maximum number of events which can be passed out using
							the events parameter. Generally this should be equal to the
							number of events in the BKNI_EventGroup. */
		CELTH_UNSIGNED *nevents /* [out] number of events which were stored in the user specified area */
		)
{
    int rc;
    struct timespec target;
    CELTH_Error_Code result = CELTHERR_SUCCESS;

    ASSERT_NOT_CRITICAL();

    if (max_events<1) {
        return CELTHERR_ERROR_BAD_PARAMETER;
    }
    if (timeoutMsec!=0 && timeoutMsec!=CELTH_INFINITE) {
        if (timeoutMsec<0) {
            /* If your app is written to allow negative values to this function, then it's highly likely you would allow -1, which would
            result in an infinite hang. We recommend that you only pass positive values to this function unless you definitely mean BKNI_INFINITE. */
            printf("CELTH_WaitForGroup given negative timeout. Possible infinite hang if timeout happens to be -1 (CELTH_INFINITE).");
        }
        if (timeoutMsec<10) {
            timeoutMsec=10; /* wait at least 10 msec */
        }
        rc = CELTH_P_SetTargetTime(&target, timeoutMsec);
        if (rc) {
            return CELTHERR_ERROR_OS;
        }
    }
    rc = pthread_mutex_lock(&group->lock);
    if (rc!=0) {
        printf("pthread_mutex_lock failed, rc=%d", rc);
        return CELTHERR_ERROR_OS;
    }
    for(;;) {
        *nevents = group_get_events(group, events, max_events);
        if (*nevents) {
            goto done;
        }
        if (timeoutMsec == 0) {
            result = CELTHERR_ERROR_TIMEOUT;
            goto done;
        }
        else if (timeoutMsec == CELTH_INFINITE) {
            rc = pthread_cond_wait(&group->cond, &group->lock);
        }
        else {
            rc = pthread_cond_timedwait(&group->cond, &group->lock, &target);
            if (rc==ETIMEDOUT) {
                printf("CELTH_WaitForGroup(%#x): timeout", (unsigned)(unsigned long)group);
                result = CELTHERR_ERROR_TIMEOUT;
                goto done;
            }
        }
        if(rc==EINTR) {
            printf("CELTH_WaitForGroup(%#x): interrupted", (unsigned)(unsigned long)group);
            continue;
        }
        if (rc!=0) {
            printf("%s() returned %d",(timeoutMsec == CELTH_INFINITE) ? "pthread_cond_wait":"pthread_cond_timedwait",rc);
            result = CELTHERR_ERROR_OS;
            goto done;
        }
    }

done:
    rc = pthread_mutex_unlock(&group->lock);
    if (rc!=0) {
        printf("pthread_mutex_unlock failed, rc=%d", rc);
        return CELTHERR_ERROR_OS;
    }
    return result;
}







