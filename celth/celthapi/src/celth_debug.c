


#include <unistd.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include <pthread.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>











#if defined(__mips__)
#include <linux/version.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,18) && 0
#define HAS_NPTL 1
#endif /* LINUX_VERSION */
#endif /* __mips__ */

#if !HAS_NPTL
#include <sys/time.h>
#endif


#include "celth_debug.h"



static struct timeval initTimeStamp;

static CELTHDBG_Level loclevel;




static int Celth_P_Vprintf(const char *fmt, va_list ap)
{
    /*return vfprintf(stderr, fmt, ap);*/
	return vprintf(fmt,ap);
}

static int Celth_P_Vsnprintf(char *s, size_t n, const char *fmt, va_list ap)
{
	return vsnprintf(s, n, fmt, ap);
}



static int Local_Snprintf(char *str, size_t len, const char *fmt, ...)
{
    va_list arglist;
    int rc;

    va_start( arglist, fmt );
  /*  rc = vsnprintf(str, len, fmt, arglist);*/
  Celth_P_Vsnprintf(str,len,fmt,arglist);
    va_end(arglist);

    return rc;
}


static int Local_Printf(const char *fmt, ...)
{
    va_list arglist;
    int rc;

    va_start( arglist, fmt );
 /*   rc = vfprintf(stderr, fmt, arglist);

	vprintf(fmt,arglist);*/
	Celth_P_Vprintf(fmt,arglist);
    va_end(arglist);

    return rc;
}



static void CelthApi_Debug_GetTimeStamp(char *timeStamp, int size_t)
{
    struct timeval currentTime;
    int hours, minutes, seconds;
    int milliseconds;
    int rc;

    rc = gettimeofday(&currentTime, NULL);
   
    if (currentTime.tv_usec < initTimeStamp.tv_usec)
    {
        milliseconds = (currentTime.tv_usec - initTimeStamp.tv_usec + 1000000)/1000;
        currentTime.tv_sec--;
    }
    else    {
        milliseconds = (currentTime.tv_usec - initTimeStamp.tv_usec)/1000;
    }

    /* Calculate the time   */
    hours = (currentTime.tv_sec - initTimeStamp.tv_sec)/3600;
    minutes = (((currentTime.tv_sec - initTimeStamp.tv_sec)/60))%60;
    seconds = (currentTime.tv_sec - initTimeStamp.tv_sec)%60;

    /* print the formatted time including the milliseconds  */
    rc = Local_Snprintf(timeStamp, size_t, "%02u:%02u:%02u.%03u", hours, minutes, seconds, milliseconds);
    return;

}


static void CelthApi_Debug_EnterLeaveFunction(const char *function, const char *kind)
{
   
      char timeStamp[20];
      CelthApi_Debug_GetTimeStamp(timeStamp, sizeof(timeStamp));

      Local_Printf(" %s %s: %s\n",  timeStamp, kind, function);
  
}

void CelthApi_Debug_EnterFunction( const char *function)
{
   CelthApi_Debug_EnterLeaveFunction(function, "Enter");
   return;
}

void CelthApi_Debug_LeaveFunction( const char *function)
{
   CelthApi_Debug_EnterLeaveFunction(function, "Leave");
   return;
}

void CelthApi_Debug_Msg(const char* fmt,...)
{
  if(loclevel>=CELTHDBG_eMsg)
{
    Local_Printf(fmt);
}
}

void CelthApi_Debug_Wrn(const char* fmt,...)
{
 if(loclevel>=CELTHDBG_eWrn)
{
   Local_Printf(fmt);
}
}

void CelthApi_Debug_Err(const char* fmt,...)
{
if(loclevel>=CELTHDBG_eErr)
{
  Local_Printf(fmt);
}
}

void CelthApi_Debug_Init()
{

 loclevel = CELTHDBG_eMsg;
}


void CelthApi_Debug_SetDebugLevel(CELTHDBG_Level level)
{
  loclevel = level;
}

CELTHDBG_Level CelthApi_Debug_GetDebugLevel()
{
 return loclevel;
}
