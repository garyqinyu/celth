/********************************************************************************
COPYRIGHT (C) STMicroelectronics 2000.

File name		: peginter.cpp
Author			: Jacky BURBAN
Description	: Peg interface to interface with PEG protocol on one side
              and to follow Tvd Appli coding rules on the other side
					
Creation		: 18.10.2001
Modification	:
	Date		DDTS Number			Modification			Name
	----   -----------      ------------			----


********************************************************************************/
#include "assert.h"
#include "osal.h"
#include "peg.hpp"
#include "sv_peg.h"
#include "ap_mmi.h"
#include "pegidle.hpp"
#include "peginter.hpp"
#include "data_srv.h"

OSAL_TaskId_t	PegTaskId;

/* Private variables (static) --------------------------------------------- */
static HDL_ParentId_t	PegParentHandleId = HDL_kNoParentId ;
static HDL_Handle_t		PegChildHandleId = HDL_kNoParentId ;
static SVPEG_InitParams_t * pstSavedInitParams;

const char* PEGTask = "TASK PEG" ;

#define SVPEG_kStackSize 4096
#define SVPEG_kPriority MIN_USER_PRIORITY
#define SVPEG_kNbrOfMessage 10


/*---------------------------------------------------------------------------*/
// Define this if we are running to build a script for an auto running
// application.
/*---------------------------------------------------------------------------*/
//#define RECORD_AUTO_SCRIPT 

#ifdef RECORD_AUTO_SCRIPT
#include "stdio.h"
#include "time.h"
void PrintMessage(const PegMessage& Mesg,  char *pType);

FILE *pOutFile = NULL;
#endif


/***********************************************************************
 Function name: SVPEG_Init
 Description  :	Start of the PEG task
 Input param  : Peg init params
 Output param : None

 Return       : SVPEG_ErrorCode_t
***********************************************************************/
SVPEG_ErrorCode_t SVPEG_Init(SVPEG_InitParams_t stInitParams)
{
	STOSD_RegionParams_t stOSDParams;

	pstSavedInitParams = (SVPEG_InitParams_t *) OSAL_MemoryAlloc(sizeof(SVPEG_InitParams_t));
	*pstSavedInitParams = stInitParams; // need to use this no to lose structure when function calling SVPEG_InitParams ends

	/* test if component is already initiated */
	if ( PegParentHandleId == HDL_kNoParentId )
	{
		/* try to create parent handle */
	  if ( ( HDL_HandleCreate ( HDL_kNoParentId, &PegParentHandleId ) ) != HDL_kNoError )
		{
			return(SVPEG_kExternalError);
		}
 	}
	else
	{
    return(SVPEG_kInvalidCall);
	}

	if ( stInitParams.RegionHandle == NULL )
  {
   	return(SVPEG_kBadParameter);
  }

	/* check if presentation size fits in OSD region size */
	STOSD_RegionGetParams( stInitParams.RegionHandle ,(STOSD_RegionParams_t *)&stOSDParams);
	if ((stOSDParams.Width < stInitParams.xSize) || (stOSDParams.Height < stInitParams.ySize))
  {
   	return(SVPEG_kBadParameter);
  }
	
	PegTaskId = OSAL_TaskCreate (PEGTask,
							(OSAL_EntryPoint_t)PegMain,
							pstSavedInitParams,
							SVPEG_kStackSize,
							SVPEG_kPriority, 
							SVPEG_kNbrOfMessage,
							sizeof(PegMessage));  

	ap_mmi_GeometryConfSemID = OSAL_SemInit("mmi GeometryConf", &ap_mmi_GeometryConfSem, 
				OSAL_kFifoNoTimeOut, 1);
	ap_mmi_PictureConfSemID = OSAL_SemInit("mmi PictureConf", &ap_mmi_PictureConfSem, 
				OSAL_kFifoNoTimeOut, 1);
	ap_mmi_TuningConfSemID = OSAL_SemInit("mmi TuningConf", &ap_mmi_TuningConfSem, 
				OSAL_kFifoNoTimeOut, 1);
	ap_mmi_AvConfSemID = OSAL_SemInit("mmi AvConf", &ap_mmi_AvConfSem, 
				OSAL_kFifoNoTimeOut, 1);
	ap_mmi_VideoConfSemID = OSAL_SemInit("mmi VideoConf", &ap_mmi_VideoConfSem, 
				OSAL_kFifoNoTimeOut, 1);
	ap_mmi_CrtConfSemID = OSAL_SemInit("mmi CrtTypeConf", &ap_mmi_CrtConfSem, 
				OSAL_kFifoNoTimeOut, 1);
	ap_mmi_OsdConfSemID = OSAL_SemInit("mmi Osd Translucent", &ap_mmi_OsdConfSem, 
				OSAL_kFifoNoTimeOut, 1);
	ap_mmi_SoundConfSemID = OSAL_SemInit("mmi SoundConf", &ap_mmi_SoundConfSem, 
				OSAL_kFifoNoTimeOut, 1);
	ap_mmi_SoundSysConfSemID = OSAL_SemInit("mmi SoundSysConf", &ap_mmi_SoundSysConfSem, 
				OSAL_kFifoNoTimeOut, 1);
	return(SVPEG_kNoError);

}


/***********************************************************************
 Function name: SVPEG_Open

 Description  :
	Connection to the PEG

 Input param  : connection handle

 Output param : None

 Return       : SVTWB_ErrorCode_t
***********************************************************************/
SVPEG_ErrorCode_t SVPEG_Open(HDL_Handle_t* pCnxHandle)
{
  OSAL_TaskLock() ;
	/* test if component is initiated */
	if ( PegParentHandleId == HDL_kNoParentId )
	{
	  OSAL_TaskUnlock() ;
		return(SVPEG_kNotInitiated);
	}

	if ( PegChildHandleId == HDL_kNoParentId ) 
    {
	    /* OK, try create connection handle */
	  if ( ( HDL_HandleCreate ( PegParentHandleId, pCnxHandle ) ) == HDL_kNoError )
		{
			/* child handle created */
		  PegChildHandleId = *pCnxHandle ;	/* store client handle */
		}
		else
		{
		  OSAL_TaskUnlock() ;
			return(SVPEG_kExternalError);
		}
	}
	else
	{
	  OSAL_TaskUnlock() ;
		return(SVPEG_kInvalidCall);		/* this connection is already opened */
	}

	OSAL_TaskUnlock() ;
	return(SVPEG_kNoError);
}


/******************************************************************************
Function name   : SVPEG_Close
Description     : Close a connection with PEG
Input param     : CnxHandle : Handle for the connection.

Output param   : None.

return         : SVPEG_ErrorCode_t
******************************************************************************/
SVPEG_ErrorCode_t SVPEG_Close( HDL_Handle_t CnxHandle )
{
  HDL_HandleStatus_t  eHdlStatus        ;

	/* test connection handle */
  OSAL_TaskLock() ;
	if ( !HDL_HandleIsValid ( PegParentHandleId, CnxHandle ) )
	{
		/* handle not valid */
		return(SVPEG_kInvalidHandle);
	}
  OSAL_TaskUnlock() ;

  /* Free resources allocated for the connection ----------------------------*/
  eHdlStatus = HDL_HandleDelete  ( (HDL_Handle_t) CnxHandle );
	PegChildHandleId = HDL_kNoParentId;
  assert( eHdlStatus == HDL_kNoError );

  return( SVPEG_kNoError );  
}


/***********************************************************************
 Function name: SVPEG_Terminate
 Description  :	Terminate PEG task
 Input param  : None
 Output param : None

 Return       : SVPEG_ErrorCode_t
***********************************************************************/
SVPEG_ErrorCode_t SVPEG_Terminate( void )
{
  ap_mmi_ChassisMessage_t	MsgChassis;

	MsgChassis.tMessageType = ap_mmi_kSystem;
	MsgChassis.tMessageParam.tSystem.SystemMessage = ap_mmi_kExit;
	MsgChassis.tMessageParam.tSystem.pChildObject = NULL;
	OSAL_MsgSend( PegTaskId, &MsgChassis, sizeof (MsgChassis));

	OSAL_MemoryFree(pstSavedInitParams);

	/* Wait for task deletion completed ----------------------*/
	while ( OSAL_TaskId( PEGTask ) != NULL )
	{
		OSAL_TaskDelay( 100 ) ;
	}

	return(SVPEG_kNoError);
}

/***********************************************************************
 Function name: SVPEG_TimerTick
 Description  :	send timer info to PEG task
 Input param  : None
 Output param : None

 Return       : SVPEG_ErrorCode_t
***********************************************************************/
SVPEG_ErrorCode_t SVPEG_TimerTick(HDL_Handle_t CnxHandle, const int iTimerRef)
{
	ap_mmi_ChassisMessage_t		Msg;

		/* test if component is initiated */
	if ( PegParentHandleId == HDL_kNoParentId )
	{
	  return(SVPEG_kNotInitiated);
	}

	/* test connection handle */
	if ( !HDL_HandleIsValid ( PegParentHandleId, CnxHandle ) )
	{
		/* handle not valid */
		return(SVPEG_kInvalidHandle);
	}

	if ( CnxHandle == PegChildHandleId )
	{

		Msg.tMessageType = ap_mmi_kSystem;
		Msg.tMessageParam.tSystem.SystemMessage = ap_mmi_kTimerTick;
		Msg.tMessageParam.tSystem.pChildObject = NULL;
		
		OSAL_MsgSend ( PegTaskId, &Msg, sizeof (Msg));
	}
	return(SVPEG_kNoError);
}


/******************************************************************************
Function name	: ap_mmi_CloseWindow

Description		: Function called when user wants to close last opened window
				  
Input param		: HDL_Handle_t CnxHandle

Output param	: None

return			: ap_mmi_ErrorCode_t
******************************************************************************/
ap_mmi_ErrorCode_t ap_mmi_CloseWindow(HDL_Handle_t CnxHandle)
{
	ap_mmi_ChassisMessage_t		Msg;

		/* test if component is initiated */
	if ( PegParentHandleId == HDL_kNoParentId )
	{
	  return(ap_mmi_kNotInitiated);
	}

	/* test connection handle */
	if ( !HDL_HandleIsValid ( PegParentHandleId, CnxHandle ) )
	{
		/* handle not valid */
		return(ap_mmi_kInvalidHandle);
	}

	if ( CnxHandle == PegChildHandleId )
	{

		Msg.tMessageType = ap_mmi_kSystem;
		Msg.tMessageParam.tSystem.SystemMessage = ap_mmi_kCloseWindow;
		Msg.tMessageParam.tSystem.pChildObject = NULL;
		
		OSAL_MsgSend ( PegTaskId, &Msg, sizeof (Msg));
	}
	return(ap_mmi_kNoError);
}


/******************************************************************************
Function name	: ap_mmi_KeyStrike

Description		: Function called when displaying PEG demo, key  pressed, 
                ucKeyValue to be the Digital value
				  
Input param		: HDL_Handle_t CnxHandle

Output param	: None

return			: ap_mmi_ErrorCode_t
******************************************************************************/
ap_mmi_ErrorCode_t ap_mmi_KeyStrike(HDL_Handle_t CnxHandle, ap_mmi_KeyStroke_t stKeyStroke,  int ucKeyValue)
{
	ap_mmi_ChassisMessage_t		Msg;

		/* test if component is initiated */
	if ( PegParentHandleId == HDL_kNoParentId )
	{
	  return(ap_mmi_kNotInitiated);
	}

	/* test connection handle */
	if ( !HDL_HandleIsValid ( PegParentHandleId, CnxHandle ) )
	{
		/* handle not valid */
		return(ap_mmi_kInvalidHandle);
	}

	if ( CnxHandle == PegChildHandleId )
	{

		Msg.tMessageType = ap_mmi_kRcKeyPressed;
		Msg.tMessageParam.tRcParam.iKeyPressed = stKeyStroke;
		Msg.tMessageParam.tRcParam.ucKeyValueParam = ucKeyValue;
		
		OSAL_MsgSend ( PegTaskId, &Msg, sizeof (Msg));
	}
	return(ap_mmi_kNoError);
}


