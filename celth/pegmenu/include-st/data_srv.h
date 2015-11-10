/********************************************************************************
COPYRIGHT (C) STMicroelectronics 2000.

File name       : mmi_stub.h 
Author          : David Meunier: 
Creation        : 24/07/2002
Modification    :
Date					DDTS Number       	Modification       	     Name 
===========				============        ===========              =====
	  
********************************************************************************/


/* Local Includes -------------------------------------------------------------*/
#include "ap_mmi.h"
#include "ap_cmd.h"
#include "osal.h"

/* ----------------------------------------------------------------------------*/
/* C++ compatibility ----------------------------------------------------------*/
/* ----------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Private constant -----------------------------------------------------------*/
/* Private Variables ----------------------------------------------------------*/

#ifndef __DATASRVMASTER /* to be defined only in data_srv.c file */
extern Ssb_Picture_t  stPictureMmi;
extern OSAL_Sem_t		ap_mmi_PictureConfSem;
extern OSAL_SemId_t		ap_mmi_PictureConfSemID;
extern Ssb_Tuning_t	stTuningMmi;
extern OSAL_Sem_t		ap_mmi_TuningConfSem;
extern OSAL_SemId_t		ap_mmi_TuningConfSemID;
extern Ssb_AVConf_t	stAVUserMmi;
extern OSAL_Sem_t		ap_mmi_AvConfSem;
extern OSAL_SemId_t		ap_mmi_AvConfSemID;
extern Ssb_CrtType_t	eCrtTypeMmi;
extern OSAL_Sem_t		ap_mmi_CrtConfSem;
extern OSAL_SemId_t		ap_mmi_CrtConfSemID;
extern unsigned char ucTranslucencyMmi;
extern OSAL_Sem_t		ap_mmi_OsdConfSem;
extern OSAL_SemId_t		ap_mmi_OsdConfSemID;
extern Ssb_Language_t  eLanguageMmi;
extern Ssb_Geometry_t stGeometryMmi;
extern OSAL_Sem_t		ap_mmi_GeometryConfSem;
extern OSAL_SemId_t		ap_mmi_GeometryConfSemID;
extern Ssb_VideoConf_t	stVideoUserMmi;
extern OSAL_Sem_t		ap_mmi_VideoConfSem;
extern OSAL_SemId_t		ap_mmi_VideoConfSemID;
extern Ssb_Sound_t stSoundMmi;
extern OSAL_Sem_t		ap_mmi_SoundConfSem;
extern OSAL_SemId_t	ap_mmi_SoundConfSemID;
extern Ssb_SoundService_t stSoundsystemMmi;
extern OSAL_Sem_t		ap_mmi_SoundSysConfSem;
extern OSAL_SemId_t	ap_mmi_SoundSysConfSemID;
extern Ssb_SoundStatusInfo_t stSoundInfoMmi;
extern int iRestoreBlueLink;
extern int iVideoNotGeometry; /* Is used to detect in the appli context whether the
							     geometry or video window is currently being displayed.	
							   */

#endif

typedef union {
	unsigned char ucValue;
} uPictureData_t;

typedef union {
	unsigned char ucValue;
} uGeometryData_t;

typedef union {
	unsigned char ucValue;
	signed char scValue;
} uVideoData_t;

typedef union {
	unsigned char ucValue;
	signed char scValue;
} uSoundData_t;

typedef union {
	unsigned char ucValue;
	signed char scValue;
} uServiceSoundData_t;

void	PictureDataSet(APCTRL_LinkId_t eEvent, uPictureData_t uValue);
void	GeometryDataSet(APCTRL_LinkId_t eEvent, uGeometryData_t uValue);
void	VideoDataSet(APCTRL_LinkId_t eEvent, uVideoData_t uValue);
void	SoundDataSet(APCTRL_LinkId_t eEvent, uSoundData_t uValue);
void	ServiceSoundDataSet(APCTRL_LinkId_t eEvent, uServiceSoundData_t uValue);

#ifdef __cplusplus
}
#endif /* __cplusplus */

