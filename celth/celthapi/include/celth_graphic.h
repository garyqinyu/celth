#ifndef __CELTH_GRAPHIC_H
#define __CELTH_GRAPHIC_H

#ifdef __cplusplus

extern "C" {

#endif





typedef struct Celth_Rect
{
    signed short x;
    signed short y;
    unsigned short width;
    unsigned short height;
} Celth_Rect;

typedef unsigned int Celth_Pixel; 


typedef void (*Celth_Graphic_Callback)(void *context, int param);     

/* 
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
*/
typedef void* Celth_CompositorHandle;



/*extern Celth_Compositor  G_Celth_OSD;

extern Celth_Graphic_Callback  G_Celth_OSD_Draw;
*/ 




void CelthApi_Graphic_Init();

unsigned char CelthApi_Graphic_SetDrawMethod(Celth_Graphic_Callback pDrawFun);


Celth_CompositorHandle CelthApi_Graphic_GetCompositorHandle();

void*  CelthApi_Graphic_GetOsdAddress(int buffernum);

unsigned int CelthApi_Graphic_GetOsdPitch(int buffernum);

void CelthApi_Graphic_SetPalette(int buffernum,unsigned char* pPalatte);


unsigned char CelthApi_Graphic_SetDrawRectwithPixel(int buffernum,Celth_Rect rect,Celth_Pixel  pixel);

unsigned char CelthApi_Graphic_FillRectWithPalette(int buffernum,Celth_Rect rect,unsigned char palattenum);

unsigned char CelthApi_Graphic_Flush(int buffernum);/*broadcom double buffer */





#ifdef __cplusplus

}

#endif






#endif
