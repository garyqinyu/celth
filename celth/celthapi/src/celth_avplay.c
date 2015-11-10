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
#include "celth_rtos.h"
#include "celth_event.h"

#include "celth_tuner.h"
#include "tspsisearch.h"

#include "celth_display.h"

#include "celth_avplay.h"



typedef struct Loc_Audio_Cfg_t{

CELTH_BOOL    bvalid;

CELTH_BOOL   bStart;


unsigned short audio_pid;

baudio_format_type   audio_type;


CELTH_UINT adata_len;	
CELTH_CHAR	 adata_buf[128];

}Loc_Audio_Cfg;



typedef struct Loc_Video_Cfg_t{

CELTH_BOOL    bvalid;

CELTH_BOOL    bStart;


unsigned short video_pid;

bvideo_codec_type   video_type;

CELTH_UINT height;
CELTH_UINT width;

CELTH_UINT vdata_len;	
CELTH_CHAR	 vdata_buf[128];

}Loc_Video_Cfg;



typedef struct Celth_AV_Private_Param_t{



	NEXUS_StcChannelHandle celth_stcChannel;

	NEXUS_PidChannelHandle celth_vpidChannel;

	NEXUS_PidChannelHandle celth_apidChannel;

/*	NEXUS_DisplayHandle display*/


    NEXUS_VideoDecoderHandle celth_vdec;
	NEXUS_AudioDecoderHandle celth_cadec;
	NEXUS_AudioDecoderHandle celth_padec;
	NEXUS_ParserBand         celth_parserband;


	
	
	CELTH_EventId_t        celth_adecPlaypumpEvent;
	NEXUS_PlaypumpHandle    celth_adecPlaypumpHandle;

	

	

	
	Loc_Video_Cfg  lvCfg;
	Loc_Audio_Cfg  laCfg;
	
  
	
}Celth_AV_Private_Param;



static Celth_AV_Private_Param celth_av_param;


static    NEXUS_DisplayHandle display=NULL;
static    NEXUS_VideoWindowHandle window=NULL;

static NEXUS_PlatformConfiguration platformConfig;


static bool compressedHdmi = false, audioStarted = false;



#define NUM_REVOKED_KSVS 3
static uint8_t NumRevokedKsvs = NUM_REVOKED_KSVS ;
static const NEXUS_HdmiOutputHdcpKsv RevokedKsvs[NUM_REVOKED_KSVS] =  
{
	/* MSB ... LSB */
	{{0xa5, 0x1f, 0xb0, 0xc3, 0x72}},
	{{0x65, 0xbf, 0x04, 0x8a, 0x7c}},
	{{0x65, 0x65, 0x1e, 0xd5, 0x64}}
} ;



CELTHOSL_TaskId_t  pHdmiThreadTaskId;

CELTH_EventId_t  hdmiEvent;


static void thr_hdmifun(void* arg)
{


}



static void disconnect_hdmi_audio(NEXUS_HdmiOutputHandle hdmi,NEXUS_AudioDecoderStartSettings audioProgram)
{
    NEXUS_AudioDecoderHandle decoder;

    if ( compressedHdmi )
    {
        decoder = compressedDecoder;
    }
    else
    {
        decoder = pcmDecoder;
    }

    if ( audioStarted )
        NEXUS_AudioDecoder_Stop(decoder);

    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_HdmiOutput_GetAudioConnector(hdmi));

    if ( audioStarted )
        NEXUS_AudioDecoder_Start(decoder, &audioProgram);
}

static void connect_hdmi_audio(NEXUS_HdmiOutputHandle hdmi, NEXUS_AudioDecoderStartSettings audioProgram)
{
    NEXUS_AudioDecoderHandle decoder;
    NEXUS_AudioInput connector;

    if ( compressedHdmi )
    {
        decoder = compressedDecoder;
        connector = NEXUS_AudioDecoder_GetConnector(decoder, NEXUS_AudioDecoderConnectorType_eCompressed);
    }
    else
    {
        decoder = pcmDecoder;
        connector = NEXUS_AudioDecoder_GetConnector(decoder, NEXUS_AudioDecoderConnectorType_eStereo);
    }

    if ( audioStarted )
        NEXUS_AudioDecoder_Stop(decoder);

    NEXUS_AudioOutput_AddInput(NEXUS_HdmiOutput_GetAudioConnector(hdmi), connector);

    if ( audioStarted )
        NEXUS_AudioDecoder_Start(decoder, &audioProgram);
}







static void hotplug_callback(void *pParam, int iParam)
{
    NEXUS_HdmiOutputStatus status;
    NEXUS_HdmiOutputHandle hdmi = pParam;
    NEXUS_DisplayHandle display = (NEXUS_DisplayHandle)iParam;

    NEXUS_HdmiOutput_GetStatus(hdmi, &status);
    fprintf(stderr, "HDMI hotplug event: %s\n", status.connected?"connected":"not connected");

    /* the app can choose to switch to the preferred format, but it's not required. */
    if ( status.connected )
    {
        NEXUS_DisplaySettings displaySettings;
        NEXUS_Display_GetSettings(display, &displaySettings);
        if ( !status.videoFormatSupported[displaySettings.format] )
        {
            fprintf(stderr, "\nCurrent format not supported by attached monitor. Switching to preferred format %d\n", status.preferredVideoFormat);
            displaySettings.format =status.preferredVideoFormat;
            NEXUS_Display_SetSettings(display, &displaySettings);
        }
    }

}



static void hdcp_callback(void *pContext, int param)
{
    bool success = (bool)param;
    NEXUS_HdmiOutputHandle handle = pContext;
    NEXUS_HdmiOutputHdcpStatus hdcpStatus;

    NEXUS_HdmiOutput_GetHdcpStatus(handle, &hdcpStatus);

    if ( success )
    {
        fprintf(stderr, "\nHDCP Authentication Successful\n");
    }
    else
    {
        fprintf(stderr, "\nHDCP Authentication Failed.  Current State %d\n", hdcpStatus.hdcpState);
    }
}

static NEXUS_AudioCodec CelthApi_AV_ConvertAudioFormat(baudio_format_type atype)
{
   NEXUS_AudioCodec acodec;
   
 
   switch(atype)
   {
   case baudio_format_unknown:
    acodec = NEXUS_AudioCodec_eUnknown;
    break;
   case baudio_format_mpeg:
   acodec = NEXUS_AudioCodec_eMpeg;
   break;
   case baudio_format_mp3:
   acodec = NEXUS_AudioCodec_eMp3;
   break;
   case baudio_format_aac:
      acodec = NEXUS_AudioCodec_eAac;
   break;
   case baudio_format_aac_plus:
      acodec = NEXUS_AudioCodec_eAacPlus;
   break;
   case baudio_format_aac_plus_adts:
      acodec = NEXUS_AudioCodec_eAacPlusAdts;
   break;
   /*case baudio_format_aac_plus_loas:
      acodec = NEXUS_AudioCodec_eAacLoas;
   break;*/
   case baudio_format_ac3:
      acodec = NEXUS_AudioCodec_eAc3;
   break;
   case baudio_format_ac3_plus:
      acodec = NEXUS_AudioCodec_eAc3Plus;
   break;
   case baudio_format_dts:
      acodec = NEXUS_AudioCodec_eDts;
   break;
   case baudio_format_lpcm_hddvd:
      acodec = NEXUS_AudioCodec_eLpcmHdDvd;
   break;
   case baudio_format_lpcm_bluray:
      acodec = NEXUS_AudioCodec_eLpcmBluRay;
   break;
   case baudio_format_dts_hd:
      acodec = NEXUS_AudioCodec_eDtsHd;
   break;
   case baudio_format_wma_std:
      acodec = NEXUS_AudioCodec_eWmaStd;
   break;
   case baudio_format_wma_pro:
      acodec = NEXUS_AudioCodec_eWmaPro;
   break;
   case baudio_format_lpcm_dvd:
      acodec = NEXUS_AudioCodec_eLpcmDvd;
   break;
   case baudio_format_avs:
      acodec = NEXUS_AudioCodec_eAvs;
   break;
   case baudio_format_pcm:
      acodec = NEXUS_AudioCodec_ePcm;
   break;
   case baudio_format_amr:
      acodec = NEXUS_AudioCodec_eAmr;
   break;
   case baudio_format_dra:
      acodec = NEXUS_AudioCodec_eDra;
   break;
   case baudio_format_cook:
      acodec = NEXUS_AudioCodec_eUnknown;
    break;
   }
   
   return acodec;
   
}



CELTH_VOID CelthApi_AV_Init()
{

CELTH_CHAR* pThreadName ="hdmi_thread";  

    display=NULL;
    window=NULL;
	
	NEXUS_HdmiOutputHdcpSettings *pHdcpSettings;
    pHdcpSettings = CELTHOSL_MemoryAlloc(sizeof(*pHdcpSettings));
    
	
	
	
	
		vparserBand = NEXUS_ParserBand_e0;
		
		
		CELTH_CreateEvent(&hdmiEvent);
		
		pHdmiThreadTaskId = CELTHOSL_TaskCreate(pThreadName,thr_hdmifun,hdmiEvent, 2048,16,512,16);

      NEXUS_Platform_GetConfiguration(&platformConfig);
	  
	  
	#if 0
	  
	              if ( pHdcpSettings )
                {
                    NEXUS_HdmiOutput_GetHdcpSettings(platformConfig.outputs.hdmi[0], pHdcpSettings);
                    /* If you want to use a custom key, insert that here */
                    pHdcpSettings->successCallback.callback = hdcp_callback;
                    pHdcpSettings->successCallback.context = platformConfig.outputs.hdmi[0];
                    pHdcpSettings->successCallback.param = true;
                    pHdcpSettings->failureCallback.callback = hdcp_callback;
                    pHdcpSettings->failureCallback.context = platformConfig.outputs.hdmi[0];
                    pHdcpSettings->failureCallback.param = false;
                    NEXUS_HdmiOutput_SetHdcpSettings(platformConfig.outputs.hdmi[0], pHdcpSettings);
					
                    NEXUS_HdmiOutput_SetHdcpRevokedKsvs(platformConfig.outputs.hdmi[0], 
                        RevokedKsvs, NumRevokedKsvs) ;
				 
                    NEXUS_HdmiOutput_StartHdcpAuthentication(platformConfig.outputs.hdmi[0]);
                    free(pHdcpSettings);
                }
                else
                    fprintf(stderr, "Out of memory\n");

	 #endif
	  
	  
	  
	  
	  
 return;
}

CELTH_VOID CelthApi_Av_ChangeVideoFormat()
{
   NEXUS_DisplaySettings displaySettings;

   NEXUS_Display_GetSettings(display,&displaySettings);

   displaySettings.format = NEXUS_VideoFormat_e720p;

   NEXUS_Display_SetSettings(display,&displaySettings);

   return;
}


CELTH_VOID CelthApi_Av_Stop()
{

    if(pcmDecoder!=NULL)
    NEXUS_AudioDecoder_Stop(pcmDecoder);
    
    if(compressedDecoder!=NULL)
    NEXUS_AudioDecoder_Stop(compressedDecoder);
    
  /*  NEXUS_AudioOutput_RemoveAllInputs(NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]));
    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]));
    NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(compressedDecoder, NEXUS_AudioDecoderConnectorType_eCompressed));*/
    NEXUS_AudioDecoder_Close(pcmDecoder);
    NEXUS_AudioDecoder_Close(compressedDecoder);
    NEXUS_VideoDecoder_Stop(vdecode);
    NEXUS_VideoWindow_RemoveAllInputs(window);
    NEXUS_VideoInput_Shutdown(NEXUS_VideoDecoder_GetConnector(vdecode));
    NEXUS_VideoWindow_Close(window);
    NEXUS_Display_Close(display);
    NEXUS_VideoDecoder_Close(vdecode);
    NEXUS_StcChannel_Close(stcChannel);
    NEXUS_PidChannel_Close(videoPidChannel);
    NEXUS_PidChannel_Close(audioPidChannel);

	return ;
  

}



CELTH_VOID CelthApi_AV_Play(unsigned char iDecoder,unsigned short VPID, unsigned short APID, unsigned short PCRPID, bvideo_codec_type vcodec,baudio_format_type aformat)
{

    NEXUS_InputBand inputBand;
  
    NEXUS_ParserBandSettings parserBandSettings;
 
    NEXUS_StcChannelSettings stcSettings;
 
 
    NEXUS_DisplaySettings displaySettings;
 
 
    NEXUS_VideoDecoderStartSettings videoProgram;
 
    NEXUS_AudioDecoderStartSettings audioProgram;
	
	
	
	NEXUS_HdmiOutputSettings hdmiSettings;
	
	
	NEXUS_AudioCodec  acodec;
	
	inputBand = (NEXUS_InputBand) CelthApi_GetFrontendBandFromTune();
	
	acodec = CelthApi_AV_ConvertAudioFormat(aformat);
	
	
printf("vpid=%02x,apid=%02x,pcrpid=%02x,vtype=%02x,atype=%02x,acodec=%02x\n",VPID,APID,PCRPID,vcodec,aformat,acodec);	
	

    NEXUS_ParserBand_GetSettings(vparserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    parserBandSettings.sourceTypeSettings.inputBand = inputBand;
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
    NEXUS_ParserBand_SetSettings(vparserBand, &parserBandSettings);

    /* Open the audio and video pid channels */
	if(videoPidChannel==NULL)
		videoPidChannel = NEXUS_PidChannel_Open(vparserBand, VPID, NULL);
    if(audioPidChannel==NULL)
	audioPidChannel = NEXUS_PidChannel_Open(vparserBand, APID, NULL);

    /* Open the StcChannel to do lipsync with the PCR */
	if(stcChannel==NULL)
	{
	
    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
    stcSettings.modeSettings.pcr.pidChannel = videoPidChannel; /* PCR happens to be on video pid */
    stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);
	}

    /* Set up decoder Start structures now. We need to know the audio codec to properly set up
    the audio outputs. */
    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = vcodec;
    videoProgram.pidChannel = videoPidChannel;
    videoProgram.stcChannel = stcChannel;
    NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
    audioProgram.codec = acodec;
    audioProgram.pidChannel = audioPidChannel;
    audioProgram.stcChannel = stcChannel;

    /* Bring up audio decoders and outputs */
	if(pcmDecoder==NULL)
    pcmDecoder = NEXUS_AudioDecoder_Open(0, NULL);
	if(compressedDecoder==NULL)
    compressedDecoder = NEXUS_AudioDecoder_Open(1, NULL);
    NEXUS_AudioOutput_AddInput(
        NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
        NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    if ( audioProgram.codec == NEXUS_AudioCodec_eAc3 )
    {
        /* Only pass through AC3 */
        NEXUS_AudioOutput_AddInput(
            NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
            NEXUS_AudioDecoder_GetConnector(compressedDecoder, NEXUS_AudioDecoderConnectorType_eCompressed));
    }
    else
    {
        NEXUS_AudioOutput_AddInput(
            NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
            NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    }

    /* Bring up display and outputs */
if(display==NULL)
{
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.format = NEXUS_VideoFormat_eNtsc;
    display = NEXUS_Display_Open(0, &displaySettings);
#if NEXUS_NUM_COMPONENT_OUTPUTS
   if(platformConfig.outputs.component[0]){
    	NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
   }
#endif 
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
#endif
#if NEXUS_NUM_SVIDEO_OUTPUTS    
    if (platformConfig.outputs.svideo[0]) {
        NEXUS_Display_AddOutput(display, NEXUS_SvideoOutput_GetConnector(platformConfig.outputs.svideo[0]));
    }
#endif
   NEXUS_Display_AddOutput(display,NEXUS_HdmiOutput_GetVideoConnector(platformConfig.outputs.hdmi[0]));    
}

if(window==NULL)
    window = NEXUS_VideoWindow_Open(display, 0);

    
	NEXUS_HdmiOutput_GetSettings(platformConfig.outputs.hdmi[0], &hdmiSettings);
    hdmiSettings.hotplugCallback.callback = hotplug_callback;
    hdmiSettings.hotplugCallback.context = platformConfig.outputs.hdmi[0];
    hdmiSettings.hotplugCallback.param = (int)display;
    NEXUS_HdmiOutput_SetSettings(platformConfig.outputs.hdmi[0], &hdmiSettings);
	
	
	
	
	/* bring up decoder and connect to display */
	
	
	   NEXUS_AudioOutput_AddInput(
        NEXUS_HdmiOutput_GetAudioConnector(platformConfig.outputs.hdmi[0]),
        NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
	
	
	
	    if ( audioProgram.codec == NEXUS_AudioCodec_eAc3 )
    {
        /* Only pass through AC3 */
        NEXUS_AudioOutput_AddInput(
            NEXUS_HdmiOutput_GetAudioConnector(platformConfig.outputs.hdmi[0]),
            NEXUS_AudioDecoder_GetConnector(compressedDecoder, NEXUS_AudioDecoderConnectorType_eCompressed));
    }
    else
    {
        NEXUS_AudioOutput_AddInput(
           NEXUS_HdmiOutput_GetAudioConnector(platformConfig.outputs.hdmi[0]),
            NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    }

	
	if(vdecode==NULL)
    vdecode = NEXUS_VideoDecoder_Open(0, NULL); /* take default capabilities */
    NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(vdecode));
	
	hotplug_callback(platformConfig.outputs.hdmi[0], (int)display);

    /* CelApi_Av_ChangeVideoFormat();*/


    /* Start Decoders */
    NEXUS_VideoDecoder_Start(vdecode, &videoProgram);
    NEXUS_AudioDecoder_Start(pcmDecoder, &audioProgram);
    if ( audioProgram.codec == NEXUS_AudioCodec_eAc3 )
    {
        /* Only pass through AC3 */
        NEXUS_AudioDecoder_Start(compressedDecoder, &audioProgram);
    }


    return ;
	
	



}




CELTH_VOID CelthApi_Av_Stop_Audio()
{


  if(pcmDecoder!=NULL)
  NEXUS_AudioDecoder_Stop(pcmDecoder);
  
  if(compressedDecoder!=NULL)
  NEXUS_AudioDecoder_Stop(compressedDecoder);
  
  NEXUS_AudioDecoder_Close(pcmDecoder);
  NEXUS_AudioDecoder_Close(compressedDecoder);


return ;
}

CELTH_VOID CelthApi_Av_Stop_Video(unsigned char iDecoder)
{


NEXUS_VideoDecoder_Stop(vdecode);
NEXUS_StcChannel_Close(stcChannel);
NEXUS_PidChannel_Close(videoPidChannel);


return;
}

CELTH_VOID CelthApi_Av_Start_Audio(unsigned short APID,baudio_format_type aformat)
{

    NEXUS_InputBand inputBand;
  
    NEXUS_ParserBandSettings parserBandSettings;
 
    NEXUS_StcChannelSettings stcSettings;
 
 
 
 

 
    NEXUS_AudioDecoderStartSettings audioProgram;
	
	
	
	NEXUS_HdmiOutputSettings hdmiSettings;
	
	
	NEXUS_AudioCodec  acodec;
	
	inputBand = (NEXUS_InputBand) CelthApi_GetFrontendBandFromTune();
	
	acodec = CelthApi_AV_ConvertAudioFormat(aformat);
	
	
printf("apid=%02x,atype=%02x,acodec=%02x\n",APID,aformat,acodec);	
	

    NEXUS_ParserBand_GetSettings(vparserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    parserBandSettings.sourceTypeSettings.inputBand = inputBand;
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
    NEXUS_ParserBand_SetSettings(vparserBand, &parserBandSettings);

    if(audioPidChannel==NULL)
	audioPidChannel = NEXUS_PidChannel_Open(vparserBand, APID, NULL);

    /* Open the StcChannel to do lipsync with the PCR */
	if(stcChannel==NULL)
	{
	
    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_eAuto; /* live */
    stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);
	}

    NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
    audioProgram.codec = acodec;
    audioProgram.pidChannel = audioPidChannel;
    audioProgram.stcChannel = stcChannel;

    /* Bring up audio decoders and outputs */
	if(pcmDecoder==NULL)
    pcmDecoder = NEXUS_AudioDecoder_Open(0, NULL);
	if(compressedDecoder==NULL)
    compressedDecoder = NEXUS_AudioDecoder_Open(1, NULL);
    NEXUS_AudioOutput_AddInput(
        NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
        NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    if ( audioProgram.codec == NEXUS_AudioCodec_eAc3 )
    {
        /* Only pass through AC3 */
        NEXUS_AudioOutput_AddInput(
            NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
            NEXUS_AudioDecoder_GetConnector(compressedDecoder, NEXUS_AudioDecoderConnectorType_eCompressed));
    }
    else
    {
        NEXUS_AudioOutput_AddInput(
            NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
            NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    }

    /* Bring up display and outputs */
    
	NEXUS_HdmiOutput_GetSettings(platformConfig.outputs.hdmi[0], &hdmiSettings);
    hdmiSettings.hotplugCallback.callback = hotplug_callback;
    hdmiSettings.hotplugCallback.context = platformConfig.outputs.hdmi[0];
    NEXUS_HdmiOutput_SetSettings(platformConfig.outputs.hdmi[0], &hdmiSettings);
	
	
	
	
	/* bring up decoder and connect to display */
	
	
	   NEXUS_AudioOutput_AddInput(
        NEXUS_HdmiOutput_GetAudioConnector(platformConfig.outputs.hdmi[0]),
        NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
	
	
	
	    if ( audioProgram.codec == NEXUS_AudioCodec_eAc3 )
    {
        /* Only pass through AC3 */
        NEXUS_AudioOutput_AddInput(
            NEXUS_HdmiOutput_GetAudioConnector(platformConfig.outputs.hdmi[0]),
            NEXUS_AudioDecoder_GetConnector(compressedDecoder, NEXUS_AudioDecoderConnectorType_eCompressed));
    }
    else
    {
        NEXUS_AudioOutput_AddInput(
           NEXUS_HdmiOutput_GetAudioConnector(platformConfig.outputs.hdmi[0]),
            NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    }

	
	
	
	
	
	
	
	
	
	
    /* Start Decoders */
    NEXUS_AudioDecoder_Start(pcmDecoder, &audioProgram);
    if ( audioProgram.codec == NEXUS_AudioCodec_eAc3 )
    {
        /* Only pass through AC3 */
        NEXUS_AudioDecoder_Start(compressedDecoder, &audioProgram);
    }


    return ;



return;
}


CELTH_VOID CelthApi_Av_Start_Video(unsigned char iDecoder,unsigned short VPID,unsigned short PCRPID, bvideo_codec_type vcodec)
{


    NEXUS_InputBand inputBand;
  
    NEXUS_ParserBandSettings parserBandSettings;
 
    NEXUS_StcChannelSettings stcSettings;
 
 
    NEXUS_DisplaySettings displaySettings;
 
 
    NEXUS_VideoDecoderStartSettings videoProgram;
 
	
	
	
	NEXUS_HdmiOutputSettings hdmiSettings;
	
	
	
	inputBand = (NEXUS_InputBand) CelthApi_GetFrontendBandFromTune();
	
	
	
printf("vpid=%02x,pcrpid=%02x,vtype=%02x\n",VPID,PCRPID,vcodec);	
	

    NEXUS_ParserBand_GetSettings(vparserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    parserBandSettings.sourceTypeSettings.inputBand = inputBand;
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
    NEXUS_ParserBand_SetSettings(vparserBand, &parserBandSettings);

    /* Open the audio and video pid channels */
	if(videoPidChannel==NULL)
		videoPidChannel = NEXUS_PidChannel_Open(vparserBand, VPID, NULL);

    /* Open the StcChannel to do lipsync with the PCR */
/* if the stcChannel already open for play the audio , close it and reopen it for play video*/
	if(stcChannel)
		NEXUS_StcChannel_Close(stcChannel);

    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
    stcSettings.modeSettings.pcr.pidChannel = videoPidChannel; /* PCR happens to be on video pid */
    stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    /* Set up decoder Start structures now. We need to know the audio codec to properly set up
    the audio outputs. */
    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = vcodec;
    videoProgram.pidChannel = videoPidChannel;
    videoProgram.stcChannel = stcChannel;


    /* Bring up display and outputs */
if(display==NULL)
{
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.format = NEXUS_VideoFormat_eNtsc;
    display = NEXUS_Display_Open(0, &displaySettings);
#if NEXUS_NUM_COMPONENT_OUTPUTS
   if(platformConfig.outputs.component[0]){
    	NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
   }
#endif 
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
#endif
#if NEXUS_NUM_SVIDEO_OUTPUTS    
    if (platformConfig.outputs.svideo[0]) {
        NEXUS_Display_AddOutput(display, NEXUS_SvideoOutput_GetConnector(platformConfig.outputs.svideo[0]));
    }
#endif
   NEXUS_Display_AddOutput(display,NEXUS_HdmiOutput_GetVideoConnector(platformConfig.outputs.hdmi[0]));    
}
if(window==NULL)
    window = NEXUS_VideoWindow_Open(display, 0);

    
	NEXUS_HdmiOutput_GetSettings(platformConfig.outputs.hdmi[0], &hdmiSettings);
    hdmiSettings.hotplugCallback.callback = hotplug_callback;
    hdmiSettings.hotplugCallback.context = platformConfig.outputs.hdmi[0];
    hdmiSettings.hotplugCallback.param = (int)display;
    NEXUS_HdmiOutput_SetSettings(platformConfig.outputs.hdmi[0], &hdmiSettings);
	
	
	if(vdecode==NULL)
    vdecode = NEXUS_VideoDecoder_Open(0, NULL); /* take default capabilities */
    NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(vdecode));
	
	hotplug_callback(platformConfig.outputs.hdmi[0], (int)display);



    /* Start Decoders */
    NEXUS_VideoDecoder_Start(vdecode, &videoProgram);

    return ;





return;
}






CELTH_VOID	CelthApi_Av_GetVideoWindowParams( unsigned char iDecoder, CELTHAV_VideoWindowParams_t *WindowParams )
{

   NEXUS_DisplaySettings displaySettings;

NEXUS_VideoWindowSettings windowSettings;

if(WindowParams==NULL)
{
return;
}

/* ??????????????????????the better way is return null, if the display is NULL */ 
if(display==NULL||window==NULL)
{
	
return ;
}


if(display==NULL)
{
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.format = NEXUS_VideoFormat_eNtsc;
    display = NEXUS_Display_Open(0, &displaySettings);
#if NEXUS_NUM_COMPONENT_OUTPUTS
   if(platformConfig.outputs.component[0]){
    	NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
   }
#endif 
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
#endif
#if NEXUS_NUM_SVIDEO_OUTPUTS    
    if (platformConfig.outputs.svideo[0]) {
        NEXUS_Display_AddOutput(display, NEXUS_SvideoOutput_GetConnector(platformConfig.outputs.svideo[0]));
    }
#endif
   NEXUS_Display_AddOutput(display,NEXUS_HdmiOutput_GetVideoConnector(platformConfig.outputs.hdmi[0]));    
}
if(window==NULL)
    window = NEXUS_VideoWindow_Open(display, 0);

    


NEXUS_VideoWindow_GetSettings(window, &windowSettings);
WindowParams->Left=windowSettings.position.x;
WindowParams->Top=windowSettings.position.y;
WindowParams->Width=windowSettings.position.width;
WindowParams->Height=windowSettings.position.height;


return;
}
CELTH_VOID	CelthApi_Av_SetVideoWindowParams( unsigned char iDecoder, CELTHAV_VideoWindowParams_t* WindowParams )
{


NEXUS_VideoWindowSettings windowSettings;

   NEXUS_DisplaySettings displaySettings;



if(display==NULL)
{
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.format = NEXUS_VideoFormat_eNtsc;
    display = NEXUS_Display_Open(0, &displaySettings);
#if NEXUS_NUM_COMPONENT_OUTPUTS
   if(platformConfig.outputs.component[0]){
    	NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
   }
#endif 
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
#endif
#if NEXUS_NUM_SVIDEO_OUTPUTS    
    if (platformConfig.outputs.svideo[0]) {
        NEXUS_Display_AddOutput(display, NEXUS_SvideoOutput_GetConnector(platformConfig.outputs.svideo[0]));
    }
#endif
   NEXUS_Display_AddOutput(display,NEXUS_HdmiOutput_GetVideoConnector(platformConfig.outputs.hdmi[0]));    
}

if(window==NULL)
    window = NEXUS_VideoWindow_Open(display, 0);

    


NEXUS_VideoWindow_GetSettings(window, &windowSettings);
windowSettings.position.x = WindowParams->Left;
windowSettings.position.y = WindowParams->Top;
windowSettings.position.width = WindowParams->Width;
windowSettings.position.height = WindowParams->Height;
windowSettings.alpha = 0xFF;
NEXUS_VideoWindow_SetSettings(window, &windowSettings);






return;
}

CELTH_VOID	CelthApi_Av_SetAudioOutputParams( CELTHAV_AudioOutputParams_t *AudioParams )
{
return;
}

CELTH_VOID	CelthApi_Av_GetAudioOutputParams( CELTHAV_AudioOutputParams_t *AudioParams )
{
return;
}



CELTH_VOID	CelthApi_Av_SetAudioMute(  )
{

NEXUS_AudioOutput AudioOutput;


NEXUS_AudioOutputSettings AudioSettings;

/*set all of the audio output as mute*/


AudioOutput = platformConfig.outputs.audioDacs[0],

NEXUS_AudioOutput_GetSettings(AudioOutput, &AudioSettings);

AudioSettings.muted=true;

NEXUS_AudioOutput_SetSettings(AudioOutput, &AudioSettings);






AudioOutput = platformConfig.outputs.spdif[0],

NEXUS_AudioOutput_GetSettings(AudioOutput, &AudioSettings);

AudioSettings.muted=true;

NEXUS_AudioOutput_SetSettings(AudioOutput, &AudioSettings);






AudioOutput = platformConfig.outputs.hdmi[0],

NEXUS_AudioOutput_GetSettings(AudioOutput, &AudioSettings);

AudioSettings.muted=true;

NEXUS_AudioOutput_SetSettings(AudioOutput, &AudioSettings);





return;
}
CELTH_VOID	CelthApi_Av_SetAudioUnmute(  )
{


NEXUS_AudioOutput AudioOutput;


NEXUS_AudioOutputSettings AudioSettings;




AudioOutput = platformConfig.outputs.audioDacs[0],

NEXUS_AudioOutput_GetSettings(AudioOutput, &AudioSettings);

AudioSettings.muted=false;

NEXUS_AudioOutput_SetSettings(AudioOutput, &AudioSettings);






AudioOutput = platformConfig.outputs.spdif[0],

NEXUS_AudioOutput_GetSettings(AudioOutput, &AudioSettings);

AudioSettings.muted=false;

NEXUS_AudioOutput_SetSettings(AudioOutput, &AudioSettings);






AudioOutput = platformConfig.outputs.hdmi[0],

NEXUS_AudioOutput_GetSettings(AudioOutput, &AudioSettings);

AudioSettings.muted=false;

NEXUS_AudioOutput_SetSettings(AudioOutput, &AudioSettings);


return;
}
CELTH_VOID	CelthApi_Av_SetAudioVolume( unsigned char  Volume )
{

NEXUS_AudioOutput AudioOutput;


NEXUS_AudioOutputSettings AudioSettings;




AudioOutput = platformConfig.outputs.audioDacs[0],

NEXUS_AudioOutput_GetSettings(AudioOutput, &AudioSettings);

AudioSettings.volumeType=NEXUS_AudioVolumeType_eDecibel;
AudioSettings.leftVolume = Volume;
AudioSettings.rightVolume = Volume;




NEXUS_AudioOutput_SetSettings(AudioOutput, &AudioSettings);






AudioOutput = platformConfig.outputs.spdif[0],

NEXUS_AudioOutput_GetSettings(AudioOutput, &AudioSettings);


AudioSettings.volumeType=NEXUS_AudioVolumeType_eDecibel;
AudioSettings.leftVolume = Volume;
AudioSettings.rightVolume = Volume;


NEXUS_AudioOutput_SetSettings(AudioOutput, &AudioSettings);






AudioOutput = platformConfig.outputs.hdmi[0],

NEXUS_AudioOutput_GetSettings(AudioOutput, &AudioSettings);

AudioSettings.volumeType=NEXUS_AudioVolumeType_eDecibel;
AudioSettings.leftVolume = Volume;
AudioSettings.rightVolume = Volume;

NEXUS_AudioOutput_SetSettings(AudioOutput, &AudioSettings);




return;
}
CELTH_VOID	CelthApi_Av_SetAudioTrack	( CELTHAV_Track_t eTrack )
{

NEXUS_AudioOutput AudioOutput;


NEXUS_AudioOutputSettings AudioSettings;




AudioOutput = platformConfig.outputs.audioDacs[0],

NEXUS_AudioOutput_GetSettings(AudioOutput, &AudioSettings);

AudioSettings.channelMode=eTrack;




NEXUS_AudioOutput_SetSettings(AudioOutput, &AudioSettings);






AudioOutput = platformConfig.outputs.spdif[0],

NEXUS_AudioOutput_GetSettings(AudioOutput, &AudioSettings);


AudioSettings.channelMode=eTrack;


NEXUS_AudioOutput_SetSettings(AudioOutput, &AudioSettings);






AudioOutput = platformConfig.outputs.hdmi[0],

NEXUS_AudioOutput_GetSettings(AudioOutput, &AudioSettings);

AudioSettings.channelMode=eTrack;

NEXUS_AudioOutput_SetSettings(AudioOutput, &AudioSettings);








return;
}




unsigned char	CelthApi_Av_GetAudioVolume( )
{
  return 100;
}
CELTHAV_Track_t	CelthApi_Av_GetAudioTrack	(  )
{
return CELTHAV_TRACK_STEREO;
}




CELTH_VOID CelthApi_Av_PlayLocalAudioFile(char* cFile,baudio_format_type DataType)
{
}

CELTH_VOID CelthApi_Av_PlayLocalVideoFile(char* cFile,bvideo_codec_type DataType)
{
}


CELTH_VOID CelthApi_Av_PlayLocalAudioData(unsigned char* ucDataBuf,baudio_format_type DataType,unsigned int iBufLen)
{
}

CELTH_VOID CelthApi_Av_PlayLocalVideoData(unsigned char* ucDataBuf,bvideo_codec_type DataType,unsigned int iBufLen)
{
}






