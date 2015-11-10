/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// panimwin.hpp - Peg Animation Window class definition.
//
// Author: Kenneth G. Maxwell
//
// Copyright (c) 1997-2000 Swell Software 
//              All Rights Reserved.
//
// Unauthorized redistrition of this source code, in whole or part,
// without the express written permission of Swell Software
// is strictly prohibited.
//
// Notes:
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifndef _ANIMWIN_
#define _ANIMWIN_

/*--------------------------------------------------------------------------*/
class PegAnimationWindow : public PegWindow
{
    public:
        PegAnimationWindow(const PegRect &Rect, PegBitmap *pBackground,
            PegBitmap **FrameList, UCHAR uNumFrames, SIGNED xPos, SIGNED yPos,
            UCHAR *Palette = NULL,  WORD wStyle = FF_NONE);

        SIGNED Message(const PegMessage &Mesg);
        virtual void Draw(void);
        virtual void Run(WORD wPeriod, UCHAR uFrame = 0);
        virtual void Stop(void);
        virtual void SetFrame(UCHAR uFrame = 0);

    protected:
        virtual void DrawAnimationFrame(void);
        UCHAR muAnimationFrame;
        UCHAR muNumFrames;
        UCHAR *mpPalette;
        PegPoint mFramePoint;
        PegBitmap *mpBackground;
        PegBitmap **mpAnimations;

    private:

};


#endif

