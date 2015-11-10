#include <stdlib.h>
#include <stdio.h>



#include "nexus_platform.h"
#include "celth_debug.h"
#include "celth_platform.h"

#include "celth_symbol.h"



void CelthApi_Platform_Init()
{

    NEXUS_PlatformSettings platformSettings;
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
	platformSettings.openFrontend = true;

    NEXUS_Platform_Init(&platformSettings);


	CELTH_Symbol_Init(NULL);

}
