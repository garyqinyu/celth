#ifndef	_CELTH_DISPLAY_H_
#define	_CELTH_DISPLAY_H_

/* Includes -------------------------------------------------------------------*/


#ifdef __cplusplus /* celth display  */
extern "C" {
#endif


typedef enum
{
	CELTHAPI_VideoFormat_eNTSC,
	CELTHAPI_VideoFormat_ePAL,
	CELTHAPI_VideoFormat_e480p,
	CELTHAPI_VideoFormat_e576p,
	CELTHAPI_VideoFormat_e720p,
	CELTHAPI_VideoFormat_e1080p,
	CELTHAPI_VideoFormat_e1080i
} CELTHAPI_VideoFormat_t;





typedef struct{
    CELTH_U32 x;
    CELTH_U32 y;
    CELTH_U32 w;
    CELTH_U32 h;          
    CELTH_U32 zorder;     
    CELTH_U32 format;     
    CELTH_U8*  pframebuf;  
}CELTHAPI_WIN_INFO;


typedef enum{
	
    CELTH_DISPLAY_SD,
	CELTH_DISPLAY_HD,
	CELTH_DISPLAY_MAX	

}CELTH_Display_Mode;







CELTH_VOID CELTHAPI_SetGlobalSDVideoFormat(CELTHAPI_VideoFormat_t eFormat);

CELTHAPI_VideoFormat_t CELTHAPI_GetGlobalSDVideoFormat();



CELTH_Status_t  CelthApi_Display_Init();
CELTH_Status_t  CelthApi_Display_Exit();

CELTH_INT  CelthApi_Display_GetNum();

CELTH_HANDLE CelthApi_Display_Open (CELTH_INT index);
CELTH_Error_Code  CelthApi_Display_Close(CELTH_HANDLE display);


CELTH_HANDLE CelthApi_VideoWindow_Create (CELTH_HANDLE display);


CELTH_HANDLE CelthApi_Display_GetVideoWindow (CELTH_Display_Mode Mode);


CELTH_Error_Code  CelthApi_VideoWindow_Destroy(CELTH_HANDLE display, CELTH_HANDLE window);









#ifdef __cplusplus /* celth display */
}
#endif

#endif


