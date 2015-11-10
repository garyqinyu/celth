#ifndef __CELTH_STDDEF_H
#define __CELTH_STDDEF_H

#ifdef __cplusplus

extern "C" {

#endif



typedef		unsigned char		CELTH_BYTE;	/* 8bits integer */
typedef		unsigned short 		CELTH_WORD;	/* 16bits integer */
typedef		unsigned long		CELTH_DWORD;	/* 32bits integer */
typedef		char				CELTH_CHAR;          /* 8 bit (preferably) unsigned, no UNICODE*/
typedef		short				CELTH_SHORT;	/* 16bits signed integer */
typedef		int					CELTH_INT;	/* 32bits signed integer */
typedef		unsigned int		CELTH_UINT;
typedef		unsigned long		CELTH_ULONG;
typedef		signed long			CELTH_SLONG;
typedef		float				CELTH_FLOAT;	
typedef		unsigned char		CELTH_BOOL;
typedef 	int					CELTH_CLOCK;
typedef		int					CELTH_SIZE;

typedef     unsigned int        CELTH_U32;

typedef     unsigned short      CELTH_U16;

typedef     unsigned char       CELTH_U8;

#define    bool           unsigned char

typedef signed long long		CELTH_I64;
typedef unsigned long long		CELTH_UI64;

typedef		CELTH_BYTE			CELTH_COLOR;

typedef		unsigned			CELTH_UNSIGNED;

typedef		CELTH_INT				*CELTH_LPINT;

#define		CELTH_VOID			void
#define		CELTH_STATIC			static
#define		CELTH_CONST			const

#define		CELTH_VOLATILE		volatile

typedef const CELTH_CHAR*			CELTH_Revision_t;
typedef CELTH_DWORD 				CELTH_ErrorCode_t;
typedef	CELTH_VOID*				CELTH_HANDLE;

#define CELTH_INFINITE         0xffffffff

#undef	TRUE
#define TRUE	1

#undef	FALSE
#define FALSE	0

#undef	NULL
#define	NULL	0

#define	DUMNY	CELTH_DWORD
typedef struct _tagPOINT_ 
{
   CELTH_SHORT	x;
   CELTH_SHORT	y;
} CELTH_POINT, *CELTH_PPOINT;

typedef struct _tagRECT 
{
   CELTH_SHORT	sLeft;
   CELTH_SHORT	sTop;
   CELTH_SHORT	sRight;
   CELTH_SHORT	sBottom;
} CELTH_RECT;

typedef struct 
{
    CELTH_WORD	Mjd;
    CELTH_BYTE	hh;
    CELTH_BYTE	mm;
    CELTH_BYTE	ss;
} CELTH_UTC;

/*------------------------------------------------------------------------------------------------------*/
/* picture */
typedef	enum	_PICTURE_TYPE
{
	PICTURE_INVALID,
	PICTURE_BMP,
	PICTURE_STILL,
	PICTURE_IFRAME
} PICTURE_TYPE;

typedef	struct	_tagPICTURE_
{
	PICTURE_TYPE			type;
	CELTH_WORD				wWidth;
	CELTH_WORD				wHeight;
	CELTH_CONST CELTH_BYTE*		data;
}CELTH_PICTURE;

/*------------------------------------------------------------------------------------------------------*/
/* language */
typedef enum _CELTHDC_LANGUAGE
{
	CELTH_ENGLISH,
	CELTH_RUSSIAN,
	CELTH_CHINESE,
	CELTH_LANG_COUNTER
}CELTH_LANGUAGE;

typedef struct _tagSTRINGS
{
	CELTH_INT			m_iStr;
	CELTH_CHAR***		m_szStr;
}
CELTH_STRINGS;

/* a simple and fast variable swapping algorithm: */
#define CELTHSWAP(a, b) {a ^= b; b ^= a; a ^= b;}
#define	CELTHMIN(a, b)	(a > b ? b : a)
#define	CELTHMAX(a, b)	(a < b ? b : a)
#define	CELTHBCD2DEC(x)	(((x>>4)&0x0F)*10+(x&0x0F))

/* Common driver error constants */
#define CELTH_MODULE_ID	0
#define CELTH_DRIVER_BASE	(CELTH_MODULE_ID << 16)
typedef enum _CELTH_status_t
{
    CELTH_NO_ERROR = CELTH_DRIVER_BASE,
	CELTH_ERROR_OS_FAIL,	
	CELTH_ERROR_FAIL,
    CELTH_ERROR_BAD_PARAMETER,             /* Bad parameter passed       */
    CELTH_ERROR_NO_MEMORY,                 /* Memory allocation failed   */
    CELTH_ERROR_UNKNOWN_DEVICE,            /* Unknown device name        */
    CELTH_ERROR_ALREADY_INITIALIZED,       /* Device already initialized */
    CELTH_ERROR_NO_FREE_HANDLES,           /* Cannot open device again   */
    CELTH_ERROR_OPEN_HANDLE,               /* At least one open handle   */
    CELTH_ERROR_INVALID_HANDLE,            /* Handle is not valid        */
    CELTH_ERROR_FEATURE_NOT_SUPPORTED,     /* Feature unavailable        */
    CELTH_ERROR_INTERRUPT_INSTALL,         /* Interrupt install failed   */
    CELTH_ERROR_INTERRUPT_UNINSTALL,       /* Interrupt uninstall failed */
    CELTH_ERROR_TIMEOUT,                   /* Timeout occured            */
    CELTH_ERROR_DEVICE_BUSY                /* Device is currently busy   */
}CELTH_Status_t;




typedef enum _CELTH_Error_Code
{
    CELTHERR_SUCCESS = 0x0,
	CELTHERR_ERROR_OS,	
	CELTHERR_ERROR_FAIL,
    CELTHERR_ERROR_BAD_PARAMETER,             /* Bad parameter passed       */
    CELTHERR_ERROR_NO_MEMORY,                 /* Memory allocation failed   */
    CELTHERR_ERROR_UNKNOWN_DEVICE,            /* Unknown device name        */
    CELTHERR_ERROR_ALREADY_INITIALIZED,       /* Device already initialized */
    CELTHERR_ERROR_NO_FREE_HANDLES,           /* Cannot open device again   */
    CELTHERR_ERROR_OPEN_HANDLE,               /* At least one open handle   */
    CELTHERR_ERROR_INVALID_HANDLE,            /* Handle is not valid        */
    CELTHERR_ERROR_FEATURE_NOT_SUPPORTED,     /* Feature unavailable        */
    CELTHERR_ERROR_INTERRUPT_INSTALL,         /* Interrupt install failed   */
    CELTHERR_ERROR_INTERRUPT_UNINSTALL,       /* Interrupt uninstall failed */
    CELTHERR_ERROR_TIMEOUT,                   /* Timeout occured            */
    CELTHERR_ERROR_DEVICE_BUSY                /* Device is currently busy   */
}CELTH_Error_Code;

  

typedef enum Celth_FrontendBand{

Celth_Band_0=0,
Celth_Band_1,
Celth_Band_2,
Celth_Band_3,
Celth_Band_Invalid = 0xFF

}Celth_FrontendBand;


typedef enum Celth_ParserBand{
	
	Celth_ParserBand_0=0,
	Celth_ParserBand_1,
	Celth_ParserBand_2,
	Celth_ParserBand_3,
	Celth_ParserBand_4,
	Celth_ParserBand_5,
	Celth_ParserBand_6,
	Celth_ParserBand_Max
	
}Celth_ParserBand;




typedef enum STREAM_SOURCE_FROM_e
{
	CELTH_STREAM_SOURCE_UNKONWN = 0,
	CELTH_STREAM_SOURCE_FROM_DEVICE,    /* stream source from demux device*/
	CELTH_STREAM_SOURCE_FROM_DEMUX,     /* stream source from demux*/
	CELTH_STREAM_SOURCE_FROM_MEMORY,     /* stream source from memory*/
	CELTH_STREAM_SOURCE_FROM_MAX
}CELTH_STREAM_SOURCE;


typedef enum CELTH_DISPLAY_DEVICE_e
{
	CELTH_DISPLAY_DEV_NONE    = 0x00,

	CELTH_DISPLAY_DEV_HDMI    = 0x01,

	CELTH_DISPLAY_DEV_CVBS    = 0x02,

	CELTH_DISPLAY_DEV_YPBPR   = 0x04,
	
	CELTH_DISPLAY_DEV_YCBCR   = 0x08,

	CELTH_DISPLAY_DEV_SVIDEO  = 0x10,

	CELTH_DISPLAY_DEV_ALL     = 0xFF

}CELTH_DISPLAY_OUTDEVICE;





typedef enum Celth_MemData_Type_e{
	CELTH_MEM_NONE   = 0,
	CELTH_MEM_PCM    = 1,
	CELTH_MEM_MP3    = 2,
	CELTH_MEM_TS     = 3,
	CELTH_MEM_ES     = 4,
	CELTH_MEM_MAX    = 5
} Celth_MemData_Type;


typedef enum Celth_ADEC_Type_e
{
	CELTH_ADEC_UNKONWN = 0,
	CELTH_ADEC_PCM,          // PCM output
	CELTH_ADEC_COMPRESS,     // compress output, such as AC3
	CELTH_ADEC_MAX
}Celth_ADEC_type;




#define CELTHAssert assert






#ifdef __cplusplus

}

#endif






#endif
