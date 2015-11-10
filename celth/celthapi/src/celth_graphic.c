#include <stdio.h>
#include "nexus_types.h"
#include "nexus_platform.h"
#include "nexus_display.h"
#include "nexus_surface.h"
#include "nexus_composite_output.h"
#include "nexus_component_output.h"
#include "nexus_core_utils.h"


#include "celth_debug.h"


#include "celth_stddef.h"


#include "celth_display.h"

#include "celth_graphic.h"

#include "celth_defpalette.h"


typedef struct Celth_Compositor {
    Celth_Rect region;
    Celth_Pixel background;
    NEXUS_DisplayHandle display;
    struct {
        NEXUS_SurfaceHandle frame;
        NEXUS_SurfaceMemory mem;
    } buffers[2];
     unsigned curbuffer;
}Celth_Compositor;




static Celth_Compositor  G_Celth_OSD;



Celth_Graphic_Callback  G_Celth_OSD_Draw=NULL;

static void
simple_fill(const NEXUS_SurfaceMemory *compositorBufferMem, const Celth_Rect *rect, Celth_Pixel pixel)
{
    unsigned x,y;
    unsigned width = rect->width;
    for(y=0;y<rect->height;y++) {
        uint32_t *buf = (uint32_t *)((uint8_t *)compositorBufferMem->buffer + compositorBufferMem->pitch*(y+rect->y))+rect->x;
        for(x=0;x<width;x++) {
            buf[x] = pixel;
        }
    }
}


static void simple_fill_withpalatee(const NEXUS_SurfaceMemory *compositorBufferMem, Celth_Rect *rect, unsigned int palattenum) 
{
	
    unsigned x,y;
    unsigned width = rect->width;
    for(y=0;y<rect->height;y++) {
	   
		
        BKNI_Memset(((uint8_t*)compositorBufferMem->buffer + compositorBufferMem->pitch*(y+rect->y))+rect->x, palattenum, width);
	

    	}

}
	


static void compositorbuffer_draw(void *fb, int unused)
{
    Celth_Compositor* compositor= (Celth_Compositor*)fb;
    NEXUS_Error errCode;
    unsigned curbuffer;
    unused = 0;


	
    if(compositor->curbuffer>=sizeof(compositor->buffers)/sizeof(*compositor->buffers)) {
        compositor->curbuffer = 0;
    }
    curbuffer = compositor->curbuffer;
    compositor->curbuffer++;

    errCode = NEXUS_Display_SetGraphicsFramebuffer(compositor->display, compositor->buffers[curbuffer].frame);
    if(errCode) {
        CelthApi_Debug_Err("Can't set compositor buffer\n");
    }

/*    NEXUS_Display_SetGraphicsFramebuffer(compositor->display,compositor->buffers[0].frame);*/
    return;
}






void CelthApi_Graphic_Init()
{

   /* NEXUS_Error errCode;*/
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_GraphicsSettings graphicsCfg;
    NEXUS_SurfaceCreateSettings surfaceCfg;
    /*NEXUS_PlatformSettings platformSettings;*/
    NEXUS_PlatformConfiguration platformConfig;
	
	NEXUS_VideoFormatInfo videoFormatInfo;

    
	int i;

CelthApi_Debug_Msg("CelthApi_Graphic_Init\n");

/*    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    NEXUS_Platform_Init(&platformSettings);
*/	
    NEXUS_Platform_GetConfiguration(&platformConfig);
	
	
	

    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.displayType = NEXUS_DisplayType_eAuto;
    displaySettings.format = NEXUS_VideoFormat_eNtsc;
    display = NEXUS_Display_Open(0, &displaySettings);
#if NEXUS_NUM_COMPONENT_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif
#if NEXUS_NUM_SCART_INPUTS
    NEXUS_Display_AddOutput(display, NEXUS_ScartInput_GetVideoOutputConnector(platformConfig.inputs.scart[0]));
#if NEXUS_NUM_SCART_INPUTS>1
    NEXUS_Display_AddOutput(display, NEXUS_ScartInput_GetVideoOutputConnector(platformConfig.inputs.scart[1]));
#endif
#endif
#if NEXUS_NUM_COMPOSITE_OUTPUTS 
    if ( platformConfig.outputs.composite[0] ) {
        NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
    }
#endif

	
    NEXUS_VideoFormat_GetInfo(displaySettings.format, &videoFormatInfo);

CelthApi_Debug_Msg("begin\n");


	NEXUS_Display_GetGraphicsSettings(display, &graphicsCfg);
	graphicsCfg.enabled = true;

	G_Celth_OSD_Draw = compositorbuffer_draw;

    graphicsCfg.frameBufferCallback.callback = /*G_Celth_OSD_Draw;*//*compositorbuffer_draw;*/NULL;
    graphicsCfg.frameBufferCallback.context = &G_Celth_OSD;
	
	graphicsCfg.clip.width = videoFormatInfo.width;
    graphicsCfg.clip.height = videoFormatInfo.height;

    NEXUS_Display_SetGraphicsSettings(display, &graphicsCfg);
	
    for(i=0;i<sizeof(G_Celth_OSD.buffers)/sizeof(*G_Celth_OSD.buffers);i++) {
        NEXUS_Surface_GetDefaultCreateSettings(&surfaceCfg);
/*		surfaceCfg.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;*/

		
		surfaceCfg.pixelFormat = NEXUS_PixelFormat_ePalette8;

        surfaceCfg.width = /*graphicsCfg.clip.width;*/videoFormatInfo.width;
        surfaceCfg.height =/* graphicsCfg.clip.height;*/videoFormatInfo.height;

        G_Celth_OSD.buffers[i].frame = NEXUS_Surface_Create(&surfaceCfg);
        if(!G_Celth_OSD.buffers[i].frame ) {
            CelthApi_Debug_Err("Can't create frame buffer!\n");
            return -1;
        }
        NEXUS_Surface_GetMemory(G_Celth_OSD.buffers[i].frame , &G_Celth_OSD.buffers[i].mem);

		CelthApi_Graphic_SetPalette(i,Celth_DefPalette256);
		
     /*   simple_fill(&G_Celth_OSD.buffers[i].mem,(Celth_Rect*) &graphicsCfg.clip, 0xFF00FF00);*/
	simple_fill_withpalatee(&G_Celth_OSD.buffers[i].mem,(Celth_Rect*)&graphicsCfg.clip,4);
    }
     /*   NEXUS_Surface_Flush(G_Celth_OSD.buffers[0].frame);*/	
/* fill with green*/

CelthApi_Debug_Msg("get the frame buffer address [0x%08x]\n",G_Celth_OSD.buffers[0].mem.buffer);	
BKNI_Memset(G_Celth_OSD.buffers[0].mem.buffer,0x44,graphicsCfg.clip.height*G_Celth_OSD.buffers[0].mem.pitch);	
/*
BKNI_Memset((uint8_t*)G_Celth_OSD.buffers[0].mem.buffer+graphicsCfg.clip.height*G_Celth_OSD.buffers[0].mem.pitch/4,3,graphicsCfg.clip.height*G_Celth_OSD.buffers[0].mem.pitch/4);

BKNI_Memset((uint8_t*)G_Celth_OSD.buffers[0].mem.buffer+graphicsCfg.clip.height*G_Celth_OSD.buffers[0].mem.pitch/2,7,graphicsCfg.clip.height*G_Celth_OSD.buffers[0].mem.pitch/4);

BKNI_Memset((uint8_t*)G_Celth_OSD.buffers[0].mem.buffer+3*graphicsCfg.clip.height*G_Celth_OSD.buffers[0].mem.pitch/4,5,graphicsCfg.clip.height*G_Celth_OSD.buffers[0].mem.pitch/4);
*/

	
    G_Celth_OSD.region.x = graphicsCfg.clip.x;
    G_Celth_OSD.region.y = graphicsCfg.clip.y;
    G_Celth_OSD.region.width = graphicsCfg.clip.width;
    G_Celth_OSD.region.height = graphicsCfg.clip.height;		
    G_Celth_OSD.background = 0xFF00FF00;
	
    G_Celth_OSD.curbuffer = 0;
    G_Celth_OSD.display = display;
	
NEXUS_Surface_Flush(G_Celth_OSD.buffers[0].frame);
	NEXUS_Display_SetGraphicsFramebuffer(display, G_Celth_OSD.buffers[0].frame);
/*
NEXUS_Surface_Flush(G_Celth_OSD.buffers[1].frame);
        NEXUS_Display_SetGraphicsFramebuffer(display, G_Celth_OSD.buffers[1].frame);
*/

CelthApi_Debug_Msg("end\n");


  /*  NEXUS_Surface_Flush(G_Celth_OSD.buffers[0].frame);
	
fprintf(stderr,"flush\n");
*/	
	

}


Celth_CompositorHandle CelthApi_Graphic_GetCompositorHandle()
{
	return (Celth_CompositorHandle)&G_Celth_OSD;
}

void*  CelthApi_Graphic_GetOsdAddress(int buffernum)
{
	if((buffernum<0)||(buffernum>=2))
	{
		return NULL	;
	}

	return  G_Celth_OSD.buffers[buffernum].mem.buffer;


	

}


unsigned int CelthApi_Graphic_GetOsdPitch(int buffernum)
{
	if((buffernum<0)||(buffernum>=2))
	{
		return 0	;
	}

	return G_Celth_OSD.buffers[buffernum].mem.pitch;

	
}



unsigned char CelthApi_Graphic_SetDrawMethod(Celth_Graphic_Callback pDrawFun)
{
	unsigned char err=CELTH_NO_ERROR;
	 NEXUS_GraphicsSettings graphicsCfg;	

	if(G_Celth_OSD.display!=NULL)
	{
		NEXUS_StopCallbacks(G_Celth_OSD.display);
		
		G_Celth_OSD_Draw = pDrawFun;
		
	NEXUS_Display_GetGraphicsSettings(G_Celth_OSD.display, &graphicsCfg);
    graphicsCfg.frameBufferCallback.callback = G_Celth_OSD_Draw;
    graphicsCfg.frameBufferCallback.context = &G_Celth_OSD;
    NEXUS_Display_SetGraphicsSettings(G_Celth_OSD.display, &graphicsCfg);		
		
		NEXUS_StartCallbacks(G_Celth_OSD.display);
	}
	else
	{
	    CelthApi_Debug_Err("G_Celth_OSD has not initailized! \n");
		err=CELTH_ERROR_INVALID_HANDLE;

	}
	return err;
}


unsigned char CelthApi_Graphic_SetDrawRectwithPixel(int buffernum,Celth_Rect rect,Celth_Pixel  pixel)
{

if((buffernum<0)||(buffernum>=2))
{
return CELTH_ERROR_BAD_PARAMETER;
}

if(G_Celth_OSD.buffers[buffernum].frame!=NULL)
{

NEXUS_Surface_GetMemory(G_Celth_OSD.buffers[buffernum].frame , &G_Celth_OSD.buffers[buffernum].mem);
simple_fill(&G_Celth_OSD.buffers[buffernum].mem, &rect, pixel);

return CELTH_NO_ERROR;
}

return CELTH_ERROR_INVALID_HANDLE;

}

unsigned char CelthApi_Graphic_FillRectWithPalette(int buffernum,Celth_Rect rect,unsigned char palettenum)
{

NEXUS_GraphicsSettings graphicsSettings;
int i;

if((buffernum<0)||(buffernum>=2))
{
return CELTH_ERROR_BAD_PARAMETER;
}

if(G_Celth_OSD.buffers[buffernum].frame!=NULL)
{

/*for (i=0;i<rect.height;i++) {
	BKNI_Memset((uint8_t*)G_Celth_OSD.buffers[buffernum].mem.buffer + i*G_Celth_OSD.buffers[buffernum].mem.pitch, G_Celth_OSD.buffers[buffernum].mem.palette[palattenum], G_Celth_OSD.buffers[buffernum].mem.pitch);
}*/

simple_fill_withpalatee(&G_Celth_OSD.buffers[buffernum].mem,&rect,palettenum);


NEXUS_Surface_Flush(G_Celth_OSD.buffers[buffernum].frame);

NEXUS_Display_GetGraphicsSettings(G_Celth_OSD.display, &graphicsSettings);
graphicsSettings.enabled = true;
/*graphicsSettings.clip.x = rect.x;
graphicsSettings.clip.y = rect.y;

graphicsSettings.clip.width = rect.width;
graphicsSettings.clip.height = rect.height;*/
NEXUS_Display_SetGraphicsSettings(G_Celth_OSD.display, &graphicsSettings);

NEXUS_Display_SetGraphicsFramebuffer(G_Celth_OSD.display, G_Celth_OSD.buffers[buffernum].frame); 


return CELTH_NO_ERROR;
}

return CELTH_ERROR_INVALID_HANDLE;



}



void CelthApi_Graphic_SetPalette(int buffernum,unsigned char* pPalatte)
{

	int i;
	if((buffernum<0)||(buffernum>=2))
	{
	return CELTH_ERROR_BAD_PARAMETER;
	}
	
	if(G_Celth_OSD.buffers[buffernum].frame!=NULL)
	{

	
		NEXUS_Surface_GetMemory(G_Celth_OSD.buffers[buffernum].frame , &G_Celth_OSD.buffers[buffernum].mem);
	
	
    	for (i=0;i<G_Celth_OSD.buffers[buffernum].mem.numPaletteEntries;i++) {
        G_Celth_OSD.buffers[buffernum].mem.palette[i] = 0xff000000 | pPalatte[i+2]<<16 | pPalatte[i+1]<<8 | pPalatte[i];
    	}

		return CELTH_NO_ERROR;
	}
	
}






unsigned char CelthApi_Graphic_Flush(int buffernum)
{

    NEXUS_GraphicsSettings graphicsSettings;


	if((buffernum<0)||(buffernum>=2))
	{
	return CELTH_ERROR_BAD_PARAMETER;
	}
    if(G_Celth_OSD.buffers[buffernum].frame!=NULL)
	{
		NEXUS_Surface_Flush(G_Celth_OSD.buffers[buffernum].frame);

		NEXUS_Display_GetGraphicsSettings(G_Celth_OSD.display, &graphicsSettings);
		graphicsSettings.enabled = true;
		/* graphicsSettings.position will default to the display size */
		graphicsSettings.clip.width = G_Celth_OSD.region.width;
		graphicsSettings.clip.height = G_Celth_OSD.region.height;
		NEXUS_Display_SetGraphicsSettings(G_Celth_OSD.display, &graphicsSettings);
		
		NEXUS_Display_SetGraphicsFramebuffer(G_Celth_OSD.display, G_Celth_OSD.buffers[buffernum].frame); 


		
		return CELTH_NO_ERROR;

	}
	return CELTH_ERROR_INVALID_HANDLE;
}
