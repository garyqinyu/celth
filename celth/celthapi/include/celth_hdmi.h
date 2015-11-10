#ifndef __CELTH_HDMI_H__
#define __CELTH_HDMI_H__


#ifdef __cplusplus

extern "C" {

#endif


#include "celth_stddef.h"


typedef enum
{
    Celth_Hdmi_null    = 0,
    Celth_Hdmi_video_display   = 1,    /* video display */
    Celth_Hdmi_audio_decoder  = 2,    /* from decoder */
    Celth_Hdmi_audio_playback = 3,    /* local playback */
    Celth_Hdmi_audio_mixer    = 4,    /* from mixer */
} CELTH_HDMI_SOURCE_TYPE;

/******************************************************************************
 *                                 Declar Functions                           *
 ******************************************************************************/
CELTH_Error_Code CelthApi_Hdmi_Init();
CELTH_Error_Code CelthApi_Hdmi_Exit();

CELTH_Error_Code CelthApi_Hdmi_Add_Video(void *pDisplayHandle);
CELTH_Error_Code CelthApi_Hdmi_Remove_Video(void *pDisplayHandle);

CELTH_Error_Code CelthApi_Hdmi_Add_Audio(void *iSourceHandle, CELTH_HDMI_SOURCE_TYPE type);
CELTH_Error_Code CelthApi_Hdmi_Remove_Audio(void *iSourceHandle, CELTH_HDMI_SOURCE_TYPE type);

#ifdef __cplusplus

}

#endif

#endif  /* __CELTH_HDMI_H__ */


