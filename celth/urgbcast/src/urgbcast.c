#include "celth_stddef.h"


#include "urgbcastdef.h"

#include "urgbcastcontrol.h"

#include "urgbcast.h"

#include "urgbcastprocess.h"





CELTH_VOID emerge_Platform_Init(CELTH_VOID* context)
{

	EBcastDataInfoInit();
	emerge_PsipmtControlInit();


	emerge_egbtChangHandle_start();
	
	emerge_BcastProcess_Start();

	



	return;
}






