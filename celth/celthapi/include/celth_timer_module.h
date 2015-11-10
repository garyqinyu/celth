#ifndef CELTH_TIMER_MODULE_H_
#define CELTH_TIMER_MODULE_H_

/*#include "nexus_base.h"*/

#ifdef __cplusplus
extern "C" {
#endif

#ifdef NEXUS_MODULE_SELF
#error Can be in two modules as the same time
#endif

extern NEXUS_ModuleHandle g_NEXUS_celthtimerModule
#define NEXUS_MODULE_NAME celthtimer
#define NEXUS_MODULE_SELF g_NEXUS_celthtimerModule

/*extern NEXUS_ModuleHandle g_NEXUS_celthtimerModule*/

#ifdef __cplusplus
}
#endif

#endif

