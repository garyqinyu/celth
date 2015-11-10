


#include "nexus_display.h"

#include "celth_stddef.h"

#include "celth_hdmi.h"

#include "celth_display.h"

#define CELTH_NUM_VIDEO_WINDOW 2


static CELTHAPI_VideoFormat_t eGlobalSDVideoFormat = CELTHAPI_VideoFormat_eNTSC;

static CELTHAPI_VideoFormat_t eGlobalHDVideoFormat = CELTHAPI_VideoFormat_e720p;


static CELTH_INT retag=0;





typedef struct
{
	NEXUS_DisplayHandle displayHandle;
	NEXUS_VideoFormat   videoFormat;
	struct
	{
		CELTH_BOOL bwinenable;  /*if true video input has connect to video window,otherwise false*/
		NEXUS_VideoWindowHandle windowHandle; /*a video window of display*/
		NEXUS_VideoInput videoInput;          /*video window input from video decoder*/
	} window[CELTH_NUM_VIDEO_WINDOW];

	struct
	{
		CELTH_BOOL boutenable; /*if true display connect, false display not connect*/
		NEXUS_VideoOutput videoOutConnecter;
	} HDMI,component,composite; /*diaplay output*/
}celth_display_info;



 typedef struct
{
	int m_vout_inited  ;  		/*0->not init,1->has inited*/

	int m_vout_opened ;

	CELTH_U8 m_vout_device ;

	NEXUS_PlatformConfiguration m_platformConfig;

	NEXUS_VideoFormat hd_video_format ;
	NEXUS_VideoFormat sd_video_format ;

	CELTH_HANDLE      hDisplay;
}vdisplay_mgr_t;





/******************************************************************************
 *                                 Global Variables                           *
 ******************************************************************************/
static celth_display_info   *g_hdDisplayMgr = NULL; /*HD display manager*/
static celth_display_info   *g_sdDisplayMgr = NULL; /*SD display manager*/

/******************************************************************************
 *                                 Local Variables                            *
 ******************************************************************************/

static vdisplay_mgr_t m_vdisplay_mgr[1] = {0};




/******************************************************************************
 *                                 Local Functions Declaring                  *
 ******************************************************************************/
static CELTH_Error_Code celthapi_display_device_open(celth_display_info *displayMgr, CELTH_Display_Mode mode);
static CELTH_Error_Code celthapi_display_device_close(celth_display_info *displayMgr, CELTH_Display_Mode mode);
static CELTH_Error_Code celthapi_display_add_graphic_input(celth_display_info *displayMgr, CELTH_U32 mode);

static CELTH_Error_Code celthapi_set_aspect_ratio(NEXUS_VideoWindowHandle window, CELTH_U32 aRatio);
static CELTH_Error_Code celthapi_set_win_transparent(NEXUS_VideoWindowHandle window, CELTH_U32 uiTransparent);
static CELTH_Error_Code celthapi_set_win_location(NEXUS_VideoWindowHandle window, CELTH_RECT *stWinLocation, CELTH_U32 mode);
static CELTH_Error_Code celthapi_set_visable(NEXUS_VideoWindowHandle window, CELTH_U32 uiVisable);
static CELTH_Error_Code celthapi_set_mode(CELTH_U32 uiTvMode);
static CELTH_Error_Code celthapi_set_brightness(NEXUS_VideoWindowHandle window, CELTH_U32 uiBright);
static CELTH_Error_Code celthapi_set_contrast(NEXUS_VideoWindowHandle window, CELTH_U32 uiContrast);
static CELTH_Error_Code celthapi_set_hue(NEXUS_VideoWindowHandle window, CELTH_U32 uiHue);
static CELTH_Error_Code celthapi_set_saturation(NEXUS_VideoWindowHandle window, CELTH_U32 uiSaturation);
static CELTH_Error_Code celthapi_set_sharpness(NEXUS_VideoWindowHandle window, CELTH_U32 uiSharpness);
static CELTH_Error_Code celthapi_set_HD_res(CELTH_U32 uiRes);
static CELTH_Error_Code celthapi_set_window_zorder(NEXUS_VideoWindowHandle window, CELTH_U32 zorder);
static CELTH_Error_Code celthapi_enable_devices(U8 uDevice);








static CELTHAPI_VideoFormat_t convertvideoformat2celth(NEXUS_VideoFormat vf)
{

	CELTHAPI_VideoFormat_t vformat;

	
	switch(vf)
	{
	case NEXUS_VideoFormat_eNtsc:
	 vformat = CELTHAPI_VideoFormat_eNTSC;
	 break;
	case NEXUS_VideoFormat_ePalD:
	 vformat = CELTHAPI_VideoFormat_ePAL;
	break;
	case NEXUS_VideoFormat_e480p:
	vformat = CELTHAPI_VideoFormat_e480p;
	break;

	
	case NEXUS_VideoFormat_e576p:
	vformat = CELTHAPI_VideoFormat_e576p;
	break;
	case NEXUS_VideoFormat_e720p:
	vformat = CELTHAPI_VideoFormat_e720p;
	break;
	case NEXUS_VideoFormat_e1080p:
		vformat = CELTHAPI_VideoFormat_e1080p;
	break;

	
	case NEXUS_VideoFormat_e1080i:
		vformat = CELTHAPI_VideoFormat_e1080i;
	break;

	default:
		 vformat = CELTHAPI_VideoFormat_eNTSC;
	 break;
	}

return vformat;
	
}





CELTH_Error_Code celthapi_display_device_open(celth_display_info *displayMgr, CELTH_Display_Mode mode)
{
	NEXUS_DisplaySettings displaySettings;

	CelthApi_Debug_EnterFunction("celthapi_display_device_open");

	switch ( mode )
	{
		case CELTH_DISPLAY_HD: /* HD display*/
			CelthApi_Derbug_Msg("open HD display device\n");
			{
				NEXUS_Display_GetDefaultSettings(&displaySettings);
				displaySettings.format  = displayMgr->videoFormat;
				displayMgr->displayHandle = NEXUS_Display_Open(0, &displaySettings);
				if ( NULL == displayMgr->displayHandle )
				{
					CelthApi_Debug_Err("can't open HD display device!\n");
					goto CELTH_ERR;
				}
			}
			break;

		case CELTH_DISPLAY_SD: // SD display
			CelthApi_Debug_Msg("open SD display device\n");
			{
				NEXUS_Display_GetDefaultSettings( &displaySettings );
				displaySettings.format  = displayMgr->videoFormat;
				displaySettings.aspectRatio = NEXUS_DisplayAspectRatio_e4x3;
				displayMgr->displayHandle = NEXUS_Display_Open(1, &displaySettings );
				if ( NULL == displayMgr->displayHandle )
				{
					CelthApi_Debug_Err("can't open SD display device!\n");
					goto CELTH_ERR;
				}
			}
			break;

		default:
			CelthApi_Debug_Err("invalid param mode");
			goto LAB_ERR;
	}

	CelthApi_Debug_LeaveFunction("celthapi_display_device_open");
	return CELTHERR_SUCCESS;
	CELTH_ERR:
	return CELTHERR_ERROR_FAIL;
}


CELTH_Error_Code celthapi_display_device_close(celth_display_info *displayMgr, CELTH_Display_Mode mode)
{
	CelthApi_Debug_EnterFunction("celthapi_display_device_close");

	switch ( mode )
	{
		case CELTH_DISPLAY_HD:
			CelthApi_Debug_Msg("close HD display device\n");
			{

				if ( true == displayMgr->component.enable )
				{

					NEXUS_VideoOutput_Shutdown(displayMgr->component.videoOutConnecter);
					displayMgr->component.enable = false;
					displayMgr->component.videoOutConnecter = NULL;
				}

				if ( NULL == displayMgr->displayHandle )
				{
					CelthApi_Debug_Err("[Error]:%d displayHandle has NULL.\n", __LINE__);
					goto CELTH_ERR;
				}
				NEXUS_Display_RemoveAllOutputs(displayMgr->displayHandle);

				NEXUS_Display_Close(displayMgr->displayHandle);
				displayMgr->displayHandle = NULL;
			}
			break;

		case CELTH_DISPLAY_SD:
			CelthApi_Debug_Msg("close sd display device\n");
			{

				if ( true == displayMgr->composite.enable )
				{
					NEXUS_VideoOutput_Shutdown(displayMgr->composite.videoOutConnecter);
					displayMgr->composite.enable = false;
					displayMgr->composite.videoOutConnecter = NULL;
				}

				if ( NULL == displayMgr->displayHandle )
				{
					CelthApi_Debug_Err("[Error]:%d displayHandle has NULL.\n", __LINE__);
					goto CELTH_ERR;
				}

				NEXUS_Display_RemoveAllOutputs(displayMgr->displayHandle);

				NEXUS_Display_Close(displayMgr->displayHandle);
				displayMgr->displayHandle = NULL;
			}
			break;
		default:
			goto CELTH_ERR;
	}

	CelthApi_Debug_LeaveFunction("celthapi_display_device_close");
	return CELTHERR_SUCCESS;

	CELTH_ERR:
	CelthApi_Debug_Err("celthapi_display_device_close End with failed.\n");
	return CELTHERR_ERROR_FAIL;
}



CELTH_VOID CELTHAPI_SetGlobalSDVideoFormat(CELTHAPI_VideoFormat_t eFormat)
{
	eGlobalSDVideoFormat = eFormat;
}

CELTHAPI_VideoFormat_t CELTHAPI_GetGlobalSDVideoFormat()
{
	return eGlobalSDVideoFormat;
}


CELTH_Status_t  CelthApi_Display_Init()
{

	
	CelthApi_Debug_EnterFunction("CelthApi_Display_Init");

	m_vdisplay_mgr->hd_video_format =	NEXUS_VideoFormat_e720p50hz;/*NEXUS_VideoFormat_e720p50hz; // NEXUS_VideoFormat_eNtsc, NEXUS_VideoFormat_e576p, NEXUS_VideoFormat_e720p*/
	m_vdisplay_mgr->sd_video_format =	NEXUS_VideoFormat_ePalN;/* NEXUS_VideoFormat_ePalD, NEXUS_VideoFormat_eNtsc */

	if ( 0 != m_vdisplay_mgr->m_vout_inited )
	{
		CelthApi_Debug_Msg(" display module have inited\n");
		goto CELTH_OK;
	}

	g_hdDisplayMgr = (celth_display_info *)CELTHOSL_MemoryAlloc(sizeof(celth_display_info));
	if ( NULL == g_hdDisplayMgr )
	{
		CelthApi_Debug_Err("malloc HD display manager fail\n");
		goto CELTH_ERR;
	}

	/*init HD display manager*/
	CELTHOSL_Memset(g_hdDisplayMgr, 0, sizeof(celth_display_info));
	g_hdDisplayMgr->videoFormat = m_vdisplay_mgr->hd_video_format;

	g_sdDisplayMgr = (celth_display_info *)CELTHOSL_MemoryAlloc(sizeof(celth_display_info));
	if ( NULL == g_sdDisplayMgr )
	{
		CelthApi_Debug_Err("malloc SD display manager fail\n");
		goto CELTH_ERR;
	}

	CELTHOSL_Memset(g_sdDisplayMgr, 0, sizeof(celth_display_info));
	g_sdDisplayMgr->videoFormat = m_vdisplay_mgr->sd_video_format;

	/*init enable devices*/

	m_vdisplay_mgr->m_vout_inited = 1;

	m_vdisplay_mgr->m_vout_opened = 0;

	m_vdisplay_mgr->m_vout_device = CELTH_DISPLAY_DEV_NONE;

	CELTH_OK:
	CelthApi_Debug_Msg(" Display Init End with Succuss.\n");
	return CELTH_NO_ERROR;

	CELTH_ERR:
	if ( g_hdDisplayMgr )
	{
		CELTHOSL_MemoryFree(g_hdDisplayMgr);
		g_hdDisplayMgr = NULL;
	}
	
	CelthApi_Debug_Err(" Display Init End with Fail.\n");
	return CELTH_ERROR_NO_MEMORY;

}
CELTH_Status_t  CelthApi_Display_Exit()
{

	
	CelthApi_Debug_EnterFunction("CelthApi_Display_Exit");

	if ( 0 == m_vdisplay_mgr->m_vout_inited )
	{
		CelthApi_Debug_Msg("Display module have destroyed\n");
		goto CELTH_OK;
	}


	
	if ( 1 == m_vdisplay_mgr->m_vout_open )
	{
		CelthApi_Display_Close(m_vdisplay_mgr->hDisplay);

		/*m_vdisplay_mgr->hDisplay=NULL;*/
		m_vdisplay_mgr->m_vout_open=0;
		m_vdisplay_mgr->m_vout_device=CELTH_DISPLAY_DEV_NONE;
		
	}


	

	if ( g_hdDisplayMgr )
	{
		CELTHOSL_MemoryFree(g_hdDisplayMgr);
		g_hdDisplayMgr = NULL;
	}

	if ( g_sdDisplayMgr )
	{
		CELTHOSL_MemoryFree(g_sdDisplayMgr);
		g_sdDisplayMgr = NULL;
	}

	
	m_vdisplay_mgr->m_vout_inited = 0;
	

	CELTH_OK:
		CelthApi_Debug_LeaveFunction("CelthApi_Display_Exit");
	return CELTH_NO_ERROR;
}

CELTH_INT  CelthApi_Display_GetNum()
{
	/* put sd and hd display together */

	return 1;
}

CELTH_HANDLE CelthApi_Display_Open (CELTH_INT index)
{

		CELTH_Error_Code  errret;

		

		NEXUS_Error nerr;

		CelthApi_Debug_EnterFunction("CelthApi_Display_Open");
		
		if(index!=1)
		{
			CelthApi_Debug_Err("display open with exceed index %x!\n",index);
			goto CELTH_ERR;
		}

		if(0==m_vdisplay_mgr->m_vout_inited)
		{
			CelthApi_Debug_Err("display open with no init !\n");
			goto CELTH_ERR;
		}

		if(m_vdisplay_mgr->m_vout_opened)
		{
			goto CELTH_OK;
		}
		else
		{

		/*open HD display device*/

		m_vdisplay_mgr->m_vout_device = CELTH_DISPLAY_DEV_NONE;
		errret = celthapi_display_device_open(g_hdDisplayMgr, CELTH_DISPLAY_HD);
		if ( CELTHERR_SUCCESS != errret )
		{
			CelthApi_Debug_Err("open HD display device fail\n");
			goto CELTH_ERR;
		}
		
		/*open SD display device*/
		errret = celthapi_display_device_open(g_sdDisplayMgr, CELTH_DISPLAY_SD);
		if ( CELTHERR_SUCCESS == errret )
		{
			CelthApi_Debug_Err("open SD display device fail\n");
			goto CELTH_ERR;
		}


	#if 0	
		
		/* connect graphic surface to HD display*/
		if ( 0 != porting_graphics_get_surface(PORT_DISPLAY_HD) )
		{
			ret = vout_display_add_graphic_input(g_hdDisplayMgr, PORT_DISPLAY_HD);
			if ( IPANEL_ERR == ret )
			{
				porting_fatal(PORT_MODULE_VOUT,"add graphic surface to HD display fail");
				goto LAB_ERR;
			}
		
			porting_info(PORT_MODULE_VOUT, "connect HD graphics to HD display success");
		}
		else
		{
			porting_warn(PORT_MODULE_VOUT, "Can't connect HD graphics to HD display");
		}
		
		/* connect graphic surface to SD display*/
		if ( 0 != porting_graphics_get_surface(PORT_DISPLAY_SD) )
		{
			ret = vout_display_add_graphic_input(g_sdDisplayMgr, PORT_DISPLAY_SD);
			if ( IPANEL_ERR == ret )
			{
				porting_fatal(PORT_MODULE_VOUT,"add graphic surface to SD display fail");
				goto LAB_ERR;
			}
		
			porting_info(PORT_MODULE_VOUT,	"connect SD graphics to SD display success" );
		}
		else
		{
			porting_warn(PORT_MODULE_VOUT, "Can't connect SD graphics to SD display");
		}

	#endif	
		
		/*get platform config ,and connect display to all output*/
		NEXUS_Platform_GetConfiguration(&m_vdisplay_mgr->m_platformConfig);
		
		/* HD output */
	#if 0	
		ret = porting_hdmi_add_video(g_hdDisplayMgr->displayHandle);
		if ( IPANEL_ERR == ret )
		{
			g_hdDisplayMgr->HDMI.enable = false;
			g_hdDisplayMgr->HDMI.videoOutConnecter = IPANEL_NULL;
			porting_fail(PORT_MODULE_VOUT, "Can't connect HD display to HDMI");
		}
		else
		{
			g_hdDisplayMgr->HDMI.enable = true;
			g_hdDisplayMgr->HDMI.videoOutConnecter = IPANEL_NULL; /*can not get HDMI connect,so display module do not use it*/
			m_vout_mgr->m_vout_device |= IPANEL_VIDEO_OUTPUT_HDMI;
			porting_info(PORT_MODULE_VOUT,"connect HD display to HDMI success");
		}
	#endif


		errret = CelthApi_Hdmi_Add_Video(g_hdDisplayMgr->displayHandle);
		if ( CELTHERR_SUCCESS != nerr )
		{
			g_hdDisplayMgr->HDMI.enable = false;
			g_hdDisplayMgr->HDMI.videoOutConnecter = NULL;
			CelthApi_Debug_Err("Can't connect HD display to hdmi\n");
		}
		else
		{
			g_hdDisplayMgr->HDMI.enable = true;
			g_hdDisplayMgr->HDMI.videoOutConnecter = NEXUS_ComponentOutput_GetConnector(m_vdisplay_mgr->m_platformConfig.outputs.hdmi[0]);
			m_vdisplay_mgr->m_vout_device |= CELTH_DISPLAY_DEV_HDMI;
			CelthApi_Debug_Msg("connect HD display to component success!\n");
		}

	
	
		
		nerr = NEXUS_Display_AddOutput(g_hdDisplayMgr->displayHandle, NEXUS_ComponentOutput_GetConnector(m_vdisplay_mgr->m_platformConfig.outputs.component[0]));
		if ( NEXUS_SUCCESS != nerr )
		{
			g_hdDisplayMgr->component.enable = false;
			g_hdDisplayMgr->component.videoOutConnecter = NULL;
			CelthApi_Debug_Err("Can't connect HD display to component\n");
		}
		else
		{
			g_hdDisplayMgr->component.enable = true;
			g_hdDisplayMgr->component.videoOutConnecter = NEXUS_ComponentOutput_GetConnector(m_vdisplay_mgr->m_platformConfig.outputs.component[0]);
			m_vdisplay_mgr->m_vout_device |= CELTH_DISPLAY_DEV_YPBPR;
			m_vdisplay_mgr->m_vout_device |= CELTH_DISPLAY_DEV_YCBCR;
			CelthApi_Debug_Msg("connect HD display to component success!\n");
		}
		
		/* SD output */
		nerr = NEXUS_Display_AddOutput(g_sdDisplayMgr->displayHandle, NEXUS_CompositeOutput_GetConnector(m_vdisplay_mgr->m_platformConfig.outputs.composite[0]));
		if ( NEXUS_SUCCESS != nerr )
		{
			g_sdDisplayMgr->composite.enable = false;
			g_sdDisplayMgr->composite.videoOutConnecter = NULL;
			CelthApi_Debug_Err("Can't connect SD display to composite\n");
		}
		else
		{
			g_sdDisplayMgr->composite.enable = true;
			g_sdDisplayMgr->composite.videoOutConnecter = NEXUS_CompositeOutput_GetConnector(m_vdisplay_mgr->m_platformConfig.outputs.composite[0]);
			m_vdisplay_mgr->m_vout_device |= CELTH_DISPLAY_DEV_CVBS;
			CelthApi_Debug_Msg("connect SD display to composite success\n");
		}


		m_vdisplay_mgr->hDisplay = (CELTH_HANDLE)(g_hdDisplayMgr);/* let  the g_hdDisplayMgr as hDisplay*/

		
			
		}

		
		CELTH_OK:		
			CelthApi_Debug_LeaveFunction("CelthApi_Display_Open");
		return m_vdisplay_mgr->hDisplay;

		CELTH_ERR:
			
			CelthApi_Debug_Err("CelthApi_Display_Open return failed!\n");
			return NULL;

		
	
}
CELTH_Error_Code  CelthApi_Display_Close(CELTH_HANDLE display)
{

	CELTH_Error_Code errcode;


	
	CelthApi_Debug_EnterFunction("CelthApi_Display_Close");


	/* didnot use the display handle*/

	CelthApi_Hdmi_Remove_Video(g_hdDisplayMgr->displayHandle);

	g_hdDisplayMgr->HDMI.enable = false;
	g_hdDisplayMgr->HDMI.videoOutConnecter = NULL;
	

	/*close HD display device*/
	errcode=celthapi_display_device_close(g_hdDisplayMgr, CELTH_DISPLAY_HD);

	/*close SD display device*/
	errcode|=celthapi_display_device_close(g_sdDisplayMgr, CELTH_DISPLAY_SD);


	
	m_vdisplay_mgr->m_vout_opened = 0;

	m_vdisplay_mgr->m_vout_device = CELTH_DISPLAY_DEV_NONE;

	
	CelthApi_Debug_LeaveFunction("CelthApi_Display_Close");

	return errcode;
}


CELTH_HANDLE CelthApi_VideoWindow_Create (CELTH_HANDLE display)
{

CelthApi_Debug_EnterFunction("CelthApi_VideoWindow_Create");

CELTH_INT i = 0;
CELTH_INT windowIndexHD = 0,windowIndexSD = 0;




if ( NULL == g_hdDisplayMgr->displayHandle )
{
	CelthApi_Debug_Err("displayHandle is NULL.");
	goto CELTH_ERR;
}

g_hdDisplayMgr->window[windowIndexHD].windowHandle = NEXUS_VideoWindow_Open(g_hdDisplayMgr->displayHandle, 0);
if ( NULL == g_hdDisplayMgr->window[windowIndexHD].windowHandle )
{
	CelthApi_Debug_Err("can't open HD window.");
	goto CELTH_ERR;
}



if ( NULL == g_sdDisplayMgr->displayHandle )
{
	CelthApi_Debug_Err("displayHandle is NULL.");
	goto CELTH_ERR;
}

g_sdDisplayMgr->window[windowIndexSD].windowHandle = NEXUS_VideoWindow_Open(g_sdDisplayMgr->displayHandle, 0);
if ( NULL == g_sdDisplayMgr->window[windowIndexSD].windowHandle )
{
	CelthApi_Debug_Err("can't open SD window.");
	goto CELTH_ERR;
}

CELTH_OK:

	
	CelthApi_Debug_LeaveFunction("CelthApi_VideoWindow_Create");
	retag=1;

	return (CELTH_HANDLE) &retag;

CELTH_ERR:
	retag=0;

	return (CELTH_HANDLE ) &retag;
	

	

}
CELTH_Error_Code  CelthApi_VideoWindow_Destroy(CELTH_HANDLE display, CELTH_HANDLE window)
{

int i;


CelthApi_Debug_EnterFunction("CelthApi_VideoWindow_Destroy");

/*close all HD video window*/
for ( i = 0; i < CELTH_NUM_VIDEO_WINDOW; i++ )
{
	if ( g_hdDisplayMgr->window[i].windowHandle )
	{
		NEXUS_VideoWindow_Close(g_hdDisplayMgr->window[i].windowHandle);
		g_hdDisplayMgr->window[i].windowHandle = NULL;
	}
}

/*close all SD video window*/
for ( i = 0; i < CELTH_NUM_VIDEO_WINDOW; i++ )
{
	if ( g_sdDisplayMgr->window[i].windowHandle )
	{
		NEXUS_VideoWindow_Close(g_sdDisplayMgr->window[i].windowHandle);
		g_sdDisplayMgr->window[i].windowHandle = NULL;
	}
}


	
	CelthApi_Debug_LeaveFunction("CelthApi_VideoWindow_Destroy");
return CELTH_NO_ERROR;


}



CELTH_HANDLE CelthApi_Display_GetVideoWindow (CELTH_Display_Mode Mode)
{

	
	CelthApi_Debug_EnterFunction("CelthApi_Display_GetVideoWindow");


	NEXUS_VideoWindowHandle winhandle;


	switch(Mode)
		{
		
		case CELTH_DISPLAY_HD:

			if(NULL!=g_hdDisplayMgr->window[0].windowHandle)
				winhandle=g_hdDisplayMgr->window[0].windowHandle;
			break;


		case CELTH_DISPLAY_SD:

			
			if(NULL!=g_sdDisplayMgr->window[0].windowHandle)
				winhandle=g_hdDisplayMgr->window[0].windowHandle;
			break;


	    default:
			winhandle=NULL;
			break;
		}




	CELTH_OK:


		
		CelthApi_Debug_LeaveFunction("CelthApi_Display_GetVideoWindow");

		return (CELTH_HANDLE)winhandle;

	

}








