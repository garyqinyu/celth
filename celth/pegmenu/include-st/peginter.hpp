/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*  peginter.h - Definition file for 7020peg.cpp                     */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#if !defined(__PegLibInterface_h)              /* Sentry, use file only if it's not already included.*/
#define __PegLibInterface_h

/* ----------------------------------------------------------------------------*/
/* C++ compatibility ----------------------------------------------------------*/
/* ----------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* every needed prototype and declaration are in sv_demo.h in order 
   to be included by other modules */
extern OSAL_TaskId_t	PegTaskId;

/* C++ compatibility ----------------------------------------------------------*/
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif                                      /* __PegLibInterface_h sentry.*/


