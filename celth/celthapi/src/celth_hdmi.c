#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_parser_band.h"
#include "nexus_video_decoder.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_video_input.h"
#include "nexus_audio_dac.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_output.h"
#include "nexus_audio_input.h"
#include "nexus_spdif_output.h"
#include "nexus_composite_output.h"
#include "nexus_component_output.h"
#include "nexus_hdmi_output.h"
#include "nexus_hdmi_output_hdcp.h"


#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>



#include "celth_stddef.h"
#include "celth_debug.h"
#include "celth_rtos.h"
#include "celth_event.h"

#include "celth_tuner.h"
#include "tspsisearch.h"

#include "celth_display.h"

#include "celth_hdmi.h"



/********************************************************************************************************
 *                                 Defines                                                                 *
 ********************************************************************************************************/
/* Define which audio device will be add to HDMI output, value will changed when call
   porting_hdmi_add_audio() and porting_hdmi_add_video() */
typedef struct
{
	void * pSourceHandle;       /* Device Handle */
	CELTH_INT iStatus;            /* 1:used; 0:not uesd */
	CELTH_HDMI_SOURCE_TYPE stiSourceType;    /* porting_hdmi_video_dispay,porting_hdmi_audio_decoder, */
                                              /* porting_hdmi_audio_playback */
}CELTH_HDMI_MANAGER;

#define CELTH_MAX_HDMI_INPUT_DEVICES 10

 typedef struct {
	CELTH_INT s_hdmi_inited;        /* HDMI module init flag */
	bool s_hdmi_connected;       /* hardware connect state */
	CELTH_HDMI_MANAGER stHdmiInputDevices[CELTH_MAX_HDMI_INPUT_DEVICES]; /* Input devices connect to HDMI */
}hdmi_mgr_t;

/********************************************************************************************************
 *                                        Global Variables                                              *
 ********************************************************************************************************/


/********************************************************************************************************
 *                                        Local Variables                                               *
 ********************************************************************************************************/

static hdmi_mgr_t m_hdmi_mgr[1] = {0};

/********************************************************************************************************
 *                                        Local Functions Declaring                                     *
 ********************************************************************************************************/
static void hdmi_hotplug_callback(void *pParam, CELTH_INT iParam)
{
	NEXUS_HdmiOutputStatus  status;
	NEXUS_HdmiOutputHandle  hdmi = pParam;

	INT32_T i;

	CelthApi_Debug_EnterFunction("hdmi_hotplug_callback");

	if (NEXUS_SUCCESS != NEXUS_HdmiOutput_GetStatus(hdmi, &status))
	{
		CelthApi_Debug_Err("NEXUS_HdmiOutput_GetStatus error.\n");
	}

	if( m_hdmi_mgr->s_hdmi_connected != status.connected )
	{
		if( m_hdmi_mgr->s_hdmi_connected )    /* if connected, then disconnect all device from HDMI */
		{
			CelthApi_Debug_Msg("Hotplug - Disconnecting HDMI");
			for( i=0;i<CELTH_MAX_HDMI_INPUT_DEVICES;i++ )
			{
				if( m_hdmi_mgr->stHdmiInputDevices[i].iStatus == 1 )
				{
					if( m_hdmi_mgr->stHdmiInputDevices[i].stiSourceType == Celth_Hdmi_video_display )
					{
						hdmi_disconnect_video(m_hdmi_mgr->stHdmiInputDevices[i].pSourceHandle, hdmi);
					}
					else
					{
						hdmi_disconnect_audio(m_hdmi_mgr->stHdmiInputDevices[i].pSourceHandle,
						m_hdmi_mgr->stHdmiInputDevices[i].stiSourceType,
						hdmi);
					}
				}
			}
			CelthApi_Debug_Msg("Shutdown HDMI device.\n");
			NEXUS_VideoOutput_Shutdown(NEXUS_HdmiOutput_GetVideoConnector(hdmi));
			NEXUS_AudioOutput_Shutdown(NEXUS_HdmiOutput_GetAudioConnector(hdmi));
		}
		else  /* if disconnect, then connect all input device to HDMI */
		{
			CelthApi_Debug_Msg("Hotplug - Connecting HDMI\n");

			for( i=0;i<CELTH_MAX_HDMI_INPUT_DEVICES;i++ )
			{
				if( m_hdmi_mgr->stHdmiInputDevices[i].iStatus == 1 )
				{
					if( m_hdmi_mgr->stHdmiInputDevices[i].stiSourceType == Celth_Hdmi_video_display )
					{
						hdmi_connect_video(m_hdmi_mgr->stHdmiInputDevices[i].pSourceHandle, hdmi);
					}
					else
					{
						hdmi_connect_audio(m_hdmi_mgr->stHdmiInputDevices[i].pSourceHandle,
							m_hdmi_mgr->stHdmiInputDevices[i].stiSourceType,
								hdmi);
					}
				}
			}
		}
		m_hdmi_mgr->s_hdmi_connected = status.connected;
	}

	CelthApi_Debug_LeaveFunction("hdmi_hotplug_callback");
	return ;


}





static void hdmi_connect_video(void *pDisplayHandle, NEXUS_HdmiOutputHandle hdmi)
{
	NEXUS_DisplaySettings   displaySettings;
	NEXUS_HdmiOutputStatus  status;

	CelthApi_Debug_EnterFunction("hdmi_connect_video");
	NEXUS_HdmiOutput_GetStatus(hdmi, &status);

	NEXUS_Display_GetSettings(pDisplayHandle, &displaySettings);
	if( !status.videoFormatSupported[displaySettings.format] )
	{
		CelthApi_Debug_Msg("Unsupport video format.Change to preferred format %d", status.preferredVideoFormat);
		displaySettings.format = status.preferredVideoFormat;
		NEXUS_Display_SetSettings((NEXUS_DisplayHandle)pDisplayHandle, &displaySettings);
	}

	NEXUS_Display_AddOutput(pDisplayHandle, NEXUS_HdmiOutput_GetVideoConnector(hdmi));
	CelthApi_Debug_LeaveFunction("hdmi_connect_video");
}

/**
 * connect video to HDMI
 *
 * @param display display handle
 * @param hdmi HDMI handle
 */
static void hdmi_disconnect_video(void *pDisplayHandle, NEXUS_HdmiOutputHandle hdmi)
{
	CelthApi_Debug_EnterFunction("hdmi_disconnect_video");
	CelthApi_Debug_Msg("DisplayHandle=0x%x, HdmiHandle=0x%x",pDisplayHandle, hdmi);

	/*tmp for test vane_exit */
	if(NULL == pDisplayHandle) {
		CelthApi_Debug_Err(" display handle is NULL\n");
		return ;
	}


	NEXUS_Display_RemoveOutput((NEXUS_DisplayHandle)pDisplayHandle, NEXUS_HdmiOutput_GetVideoConnector(hdmi));
	CelthApi_Debug_LeaveFunction("hdmi_connect_video");
}








static void hdmi_connect_audio(void *pAudioHandle, CELTH_HDMI_SOURCE_TYPE type, NEXUS_HdmiOutputHandle hdmi)
{
	NEXUS_AudioDecoderStatus audioStatus;
	NEXUS_AudioDecoderStartSettings audioStartSettings;
	NEXUS_AudioPlaybackStatus playbackStatus;
	NEXUS_AudioPlaybackStartSettings playbackStartSettings;

	CelthApi_Debug_EnterFunction("hdmi_connect_audio");

	switch( type )
	{
		case Celth_Hdmi_audio_decoder: /* audio input device is decoder */
			{
				NEXUS_AudioDecoder_GetDefaultStartSettings(&audioStartSettings);

				
				if( NEXUS_SUCCESS != NEXUS_AudioDecoder_GetStatus((NEXUS_AudioDecoderHandle)pAudioHandle, &audioStatus) )
				{
					CelthApi_Debug_Err("NEXUS_AudioDecoder_GetStatus failed\n");
				}
				if( audioStatus.started )
				{
					NEXUS_AudioDecoder_Stop((NEXUS_AudioDecoderHandle)pAudioHandle);
				}

				NEXUS_AudioOutput_AddInput(NEXUS_HdmiOutput_GetAudioConnector(hdmi),
				NEXUS_AudioDecoder_GetConnector((NEXUS_AudioDecoderHandle)pAudioHandle,
				NEXUS_AudioDecoderConnectorType_eStereo));

				if( audioStatus.started )
				{
					NEXUS_AudioDecoder_Start((NEXUS_AudioDecoderHandle)pAudioHandle, &audioStartSettings);
				}
			}
			break;

		case Celth_Hdmi_audio_playback: /* audio input device is playback */
			{
				NEXUS_AudioOutput_AddInput(NEXUS_HdmiOutput_GetAudioConnector(hdmi),
				NEXUS_AudioPlayback_GetConnector((NEXUS_AudioPlaybackHandle)pAudioHandle));
			}
			break;

		case Celth_Hdmi_audio_mixer: /* audio input device is mixer */
			{
				CelthApi_Debug_Msg("Audio from mixer.\n");
			/*	porting_mixer_stop();*/
				NEXUS_AudioOutput_AddInput(NEXUS_HdmiOutput_GetAudioConnector(hdmi),
				NEXUS_AudioMixer_GetConnector((NEXUS_AudioMixerHandle)pAudioHandle));
			/*	porting_mixer_start();*/
			}
			break;

		default:
			{
				CelthApi_Debug_Err("Unknow audio source type.\n");
			}
			break;
	}

	CelthApi_Debug_LeaveFunction("hdmi_connect_audio");
}

/**
 * disconnect audio from HDMI
 *
 * @param audio Audio decoder handle
 * @param type Audio Device type
 * @param hdmi HDMI device handle
 */
static void hdmi_disconnect_audio(void *pAudioHandle, CELTH_HDMI_SOURCE_TYPE type, NEXUS_HdmiOutputHandle hdmi)
{
	NEXUS_AudioDecoderStatus audioStatus;
	NEXUS_AudioDecoderStartSettings audioStartSettings;
	NEXUS_AudioPlaybackStatus playbackStatus;
	NEXUS_AudioPlaybackStartSettings playbackStartSettings;

	CelthApi_Debug_EnterFunction("hdmi_disconnect_audio");

	switch( type )
	{
		case Celth_Hdmi_audio_decoder: /* audio input device is decoder */
			{
				NEXUS_AudioDecoder_GetDefaultStartSettings(&audioStartSettings);

				if( NEXUS_SUCCESS != NEXUS_AudioDecoder_GetStatus(pAudioHandle, &audioStatus) )
				{
					CelthApi_Debug_Err("NEXUS_AudioDecoder_GetStatus\n");
				}
				if( audioStatus.started )
				{
					NEXUS_AudioDecoder_Stop((NEXUS_AudioDecoderHandle)pAudioHandle);
				}

				NEXUS_AudioOutput_RemoveInput(NEXUS_HdmiOutput_GetAudioConnector(hdmi),
				NEXUS_AudioDecoder_GetConnector((NEXUS_AudioDecoderHandle)pAudioHandle,
				NEXUS_AudioDecoderConnectorType_eStereo));

				if( audioStatus.started )
				{
					NEXUS_AudioDecoder_Start((NEXUS_AudioDecoderHandle)pAudioHandle, &audioStartSettings);
				}
			}
			break;

		case Celth_Hdmi_audio_playback: /* audio input device is playback */
			{
				NEXUS_AudioOutput_RemoveInput(NEXUS_HdmiOutput_GetAudioConnector(hdmi),
				NEXUS_AudioPlayback_GetConnector((NEXUS_AudioPlaybackHandle)pAudioHandle));
			}
			break;

		case Celth_Hdmi_audio_mixer:  /* audio input device is mixer */
			{
				/*porting_mixer_stop();*/
				NEXUS_AudioOutput_RemoveInput(NEXUS_HdmiOutput_GetAudioConnector(hdmi),
				NEXUS_AudioMixer_GetConnector((NEXUS_AudioMixerHandle)pAudioHandle));
			/*	porting_mixer_start();*/
			}
			break;

		default:
			{
				CelthApi_Debug_Err("Unknow audio source type.\n");
			}
			break;
	}

	CelthApi_Debug_LeaveFunction("hdmi_disconnect_audio");
}




CELTH_Error_Code CelthApi_Hdmi_Init()
{

	
	NEXUS_PlatformConfiguration stPlatformConfig;
	NEXUS_HdmiOutputSettings  stHdmiSettings;

	int i;

	CelthApi_Debug_EnterFunction("CelthApi_Hdmi_Init");

	for( i=0;i<CELTH_MAX_HDMI_INPUT_DEVICES;i++ )
	{
		m_hdmi_mgr->stHdmiInputDevices[i].pSourceHandle = NULL;
		m_hdmi_mgr->stHdmiInputDevices[i].stiSourceType = Celth_Hdmi_null;
		m_hdmi_mgr->stHdmiInputDevices[i].iStatus = 0;
	}

	/* Registe HDMI callback functions */
	NEXUS_Platform_GetConfiguration(&stPlatformConfig);

	NEXUS_HdmiOutput_GetSettings(stPlatformConfig.outputs.hdmi[0], &stHdmiSettings);
	stHdmiSettings.hotplugCallback.callback   = hdmi_hotplug_callback;
	stHdmiSettings.hotplugCallback.context    = stPlatformConfig.outputs.hdmi[0];
	stHdmiSettings.hotplugCallback.param      = 0;  /* Not used */
	NEXUS_HdmiOutput_SetSettings(stPlatformConfig.outputs.hdmi[0], &stHdmiSettings);

	m_hdmi_mgr->s_hdmi_inited = 1;
	m_hdmi_mgr->s_hdmi_connected = false;

	/* Check if HDMI pluged in when bootup */
	hdmi_hotplug_callback(stHdmiSettings.hotplugCallback.context,stHdmiSettings.hotplugCallback.param);

	CelthApi_Debug_LeaveFunction("CelthApi_Hdmi_Init");

	
	return CELTHERR_SUCCESS;
}
CELTH_Error_Code CelthApi_Hdmi_Exit()
{

	
		CelthApi_Debug_EnterFunction("CelthApi_Hdmi_Exit");
	
		m_hdmi_mgr->s_hdmi_inited = 0;
		m_hdmi_mgr->s_hdmi_connected = false;
	
		CelthApi_Debug_LeaveFunction("CelthApi_Hdmi_Exit");

		
		return CELTHERR_SUCCESS;
	
}

CELTH_Error_Code CelthApi_Hdmi_Add_Video(void *pDisplayHandle)
{


	
	NEXUS_PlatformConfiguration stPlatformConfig;
	NEXUS_HdmiOutputStatus  status;

	int i;

	
	CelthApi_Debug_EnterFunction("CelthApi_Hdmi_Add_Video");

	CelthApi_Debug_Msg("CelthApi_Hdmi_Add_Video Handle=0x%x",pDisplayHandle);

	for( i=0;i<CELTH_MAX_HDMI_INPUT_DEVICES;i++ )
	{
		if( m_hdmi_mgr->stHdmiInputDevices[i].stiSourceType == Celth_Hdmi_video_display && m_hdmi_mgr->stHdmiInputDevices[i].pSourceHandle == pDisplayHandle )
		{
			CelthApi_Debug_Msg("Added, Handle=0x%x",pDisplayHandle);

			goto CELTH_OK;
		}
	}

	for( i=0;i<CELTH_MAX_HDMI_INPUT_DEVICES;i++ )
	{
		if( m_hdmi_mgr->stHdmiInputDevices[i].iStatus == 0 )
		{
			m_hdmi_mgr->stHdmiInputDevices[i].iStatus = 1;
			m_hdmi_mgr->stHdmiInputDevices[i].pSourceHandle = pDisplayHandle;
			m_hdmi_mgr->stHdmiInputDevices[i].stiSourceType = Celth_Hdmi_video_display;

			NEXUS_Platform_GetConfiguration(&stPlatformConfig);

			NEXUS_HdmiOutput_GetStatus(stPlatformConfig.outputs.hdmi[0], &status);

			/* add device to HDMI output when HDMI pluged in */
			if( status.connected )
			{
				CelthApi_Debug_Msg("Connect HDMI video when add input.\n");
				hdmi_connect_video(m_hdmi_mgr->stHdmiInputDevices[i].pSourceHandle,stPlatformConfig.outputs.hdmi[0]);
			}

			CelthApi_Debug_Msg("Added, Handle=0x%x",pDisplayHandle);
			goto CELTH_OK;
		}
	}

	CELTH_ERR:
		
	CelthApi_Debug_Err("Can't add, Handle=0x%x\n",pDisplayHandle);
	CelthApi_Debug_LeaveFunction("CelthApi_Hdmi_Add_Video");
	return CELTHERR_ERROR_FAIL;


	
	CELTH_OK:
	
	CelthApi_Debug_LeaveFunction("CelthApi_Hdmi_Add_Video");

	
	return CELTHERR_SUCCESS;
}
CELTH_Error_Code CelthApi_Hdmi_Remove_Video(void *pDisplayHandle)
{

	
	int i;

	NEXUS_PlatformConfiguration stPlatformConfig;
	NEXUS_HdmiOutputStatus  status;

	
	CelthApi_Debug_EnterFunction("CelthApi_Hdmi_Remove_Video");

	CelthApi_Debug_Msg("CelthApi_Hdmi_Remove_Video. Handle=0x%x",pDisplayHandle);

	if ( pDisplayHandle == NULL )
	{
		CelthApi_Debug_Err("[Error] Handle == NULL,return Error.\n");

		goto CELTH_ERR;
	}
	
	for( i=0;i<CELTH_MAX_HDMI_INPUT_DEVICES;i++ )
	{
		if( m_hdmi_mgr->stHdmiInputDevices[i].stiSourceType == Celth_Hdmi_video_display && m_hdmi_mgr->stHdmiInputDevices[i].pSourceHandle == pDisplayHandle )
		{
			NEXUS_Platform_GetConfiguration(&stPlatformConfig);

			NEXUS_HdmiOutput_GetStatus(stPlatformConfig.outputs.hdmi[0], &status);

			/* remove device from HDMI output when HDMI pluged in */
			if( status.connected )
			{
				CelthApi_Debug_Msg("Disonnect HDMI video when remove input.\n");
				hdmi_disconnect_video(m_hdmi_mgr->stHdmiInputDevices[i].pSourceHandle,stPlatformConfig.outputs.hdmi[0]);
			}

			m_hdmi_mgr->stHdmiInputDevices[i].pSourceHandle = NULL;
			m_hdmi_mgr->stHdmiInputDevices[i].stiSourceType = Celth_Hdmi_null;
			m_hdmi_mgr->stHdmiInputDevices[i].iStatus = 0;
			CelthApi_Debug_Msg("Removed, Handle=0x%x\n",m_hdmi_mgr->stHdmiInputDevices[i].pSourceHandle);
			goto CELTH_OK;
		}
	}

	CELTH_OK:
	
	CelthApi_Debug_LeaveFunction("CelthApi_Hdmi_Remove_Video");
	

	
	return CELTHERR_SUCCESS;


CELTH_ERR:
	
CelthApi_Debug_Err("Can't remove, Handle=0x%x\n",pDisplayHandle);
CelthApi_Debug_LeaveFunction("CelthApi_Hdmi_Add_Video");
return CELTHERR_ERROR_FAIL;



	
}

CELTH_Error_Code CelthApi_Hdmi_Add_Audio(void *iSourceHandle, CELTH_HDMI_SOURCE_TYPE type)
{

	
	int i=0;

	NEXUS_PlatformConfiguration stPlatformConfig;
	NEXUS_HdmiOutputStatus  status;

	
	CelthApi_Debug_EnterFunction("CelthApi_Hdmi_Add_Audio");


	CelthApi_Debug_Msg("CelthApi_Hdmi_Remove_Video. Handle=0x%x, type=%d",iSourceHandle, type);

	for( i=0;i<CELTH_MAX_HDMI_INPUT_DEVICES;i++ )
	{
		if( m_hdmi_mgr->stHdmiInputDevices[i].stiSourceType == type && m_hdmi_mgr->stHdmiInputDevices[i].pSourceHandle == iSourceHandle )
		{
			CelthApi_Debug_Msg("Added, Handle=0x%x\n",iSourceHandle);
			goto CELTH_OK;
		}
	}

	for( i=0;i<CELTH_MAX_HDMI_INPUT_DEVICES;i++ )
	{
		if( m_hdmi_mgr->stHdmiInputDevices[i].iStatus == 0 )
		{
			m_hdmi_mgr->stHdmiInputDevices[i].iStatus = 1;
			m_hdmi_mgr->stHdmiInputDevices[i].pSourceHandle = iSourceHandle;
			m_hdmi_mgr->stHdmiInputDevices[i].stiSourceType = type;

			NEXUS_Platform_GetConfiguration(&stPlatformConfig);

			NEXUS_HdmiOutput_GetStatus(stPlatformConfig.outputs.hdmi[0], &status);

			/* add device to HDMI output when HDMI pluged in */
			if( status.connected )
			{
				CelthApi_Debug_Msg("Connect HDMI audio when add input.\n");
				hdmi_connect_audio(m_hdmi_mgr->stHdmiInputDevices[i].pSourceHandle,
				m_hdmi_mgr->stHdmiInputDevices[i].stiSourceType,
				stPlatformConfig.outputs.hdmi[0]);
			}
			CelthApi_Debug_Msg("Added, Handle=0x%x\n",iSourceHandle);
			
			goto CELTH_OK;
		}
	}

	CELTH_OK:

		
		CelthApi_Debug_LeaveFunction("CelthApi_Hdmi_Add_Audio");

		
		return CELTHERR_SUCCESS;
		
		
		CELTH_ERR:
			
		CelthApi_Debug_Err("Can't add, Handle=0x%x\n",iSourceHandle);
		CelthApi_Debug_LeaveFunction("CelthApi_Hdmi_Add_Audio");
		return CELTHERR_ERROR_FAIL;
		

}
CELTH_Error_Code CelthApi_Hdmi_Remove_Audio(void *iSourceHandle, CELTH_HDMI_SOURCE_TYPE type)
{

int i=0;

NEXUS_PlatformConfiguration stPlatformConfig;
NEXUS_HdmiOutputStatus	status;

CelthApi_Debug_EnterFunction("CelthApi_Hdmi_Remove_Audio");


CelthApi_Debug_Msg("Handle=0x%x, type=%d\n",iSourceHandle, type);

for( i=0;i<CELTH_MAX_HDMI_INPUT_DEVICES;i++ )
{
	if( m_hdmi_mgr->stHdmiInputDevices[i].stiSourceType == type && m_hdmi_mgr->stHdmiInputDevices[i].pSourceHandle == iSourceHandle )
	{
		NEXUS_Platform_GetConfiguration(&stPlatformConfig);

		NEXUS_HdmiOutput_GetStatus(stPlatformConfig.outputs.hdmi[0], &status);

		/* remove device from HDMI output when HDMI pluged in */
		if( status.connected )
		{
			CelthApi_Debug_Msg("Disconnect HDMI audio when remove input.\n");
			hdmi_disconnect_audio(m_hdmi_mgr->stHdmiInputDevices[i].pSourceHandle,
			m_hdmi_mgr->stHdmiInputDevices[i].stiSourceType,
			stPlatformConfig.outputs.hdmi[0]);
		}

		m_hdmi_mgr->stHdmiInputDevices[i].pSourceHandle = NULL;
		m_hdmi_mgr->stHdmiInputDevices[i].stiSourceType = Celth_Hdmi_null;
		m_hdmi_mgr->stHdmiInputDevices[i].iStatus = 0;
		CelthApi_Debug_Msg("Removed, Handle=0x%x\n",m_hdmi_mgr->stHdmiInputDevices[i].pSourceHandle);
		goto CELTH_OK;
	}
}

CELTH_OK:

	
	CelthApi_Debug_LeaveFunction("CelthApi_Hdmi_Remove_Audio");

	
	return CELTHERR_SUCCESS;
	
	
	CELTH_ERR:
		
	CelthApi_Debug_Err("Can't add, Handle=0x%x\n",iSourceHandle);
	CelthApi_Debug_LeaveFunction("CelthApi_Hdmi_Remove_Audio");
	return CELTHERR_ERROR_FAIL;
	

}




