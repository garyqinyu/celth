/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pprogbar.cpp - PegProgressBar class.
//
// Author: Kenneth G. Maxwell
//
// Copyright (c) 1997-2000 Swell Software 
//              All Rights Reserved.
//
// Unauthorized redistribution of this source code, in whole or part,
// without the express written permission of Swell Software
// is strictly prohibited.
//
// Notes:
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifndef USE_PEG_LTOA
#include "stdlib.h"
#endif
#include "string.h"
#include "peg.hpp"


/*--------------------------------------------------------------------------*/
// Note: PegProgressBar uses the non-ANSI "C" function "_ltoa" to convert the
// progress value to a string. Most compilers used to date (MetaWare, Borland,
// Microsoft, Green Hills, Watcom, and Keil) support this function in their
// run-time library. If your compiler does not support the "_ltoa" function,
// you should turn on the define USE_PEG_LTOA in peg.hpp.
//
// Why don't we use the ANSI sprintf? Many run-time implementations of sprintf
// are non-reentrant, or otherwise forbidden from use on embedded platforms
// due to stack requirements or code size. Sprintf is often a very large
// function!
/*--------------------------------------------------------------------------*/


/*--------------------- PEG PROGRESS BAR CLASS -----------------------------*/
/*--------------------------------------------------------------------------*/
PegProgressBar::PegProgressBar(const PegRect &Rect, WORD wStyle,
    SIGNED iMin, SIGNED iMax, SIGNED iCurrent) :
    PegThing(Rect, 0, wStyle)
{
    miMin = iMin;
    miMax = iMax;
    miCurrent = iCurrent;
    muColors[PCI_NORMAL] = PCLR_LOWLIGHT;

  #ifdef PEG_NUM_COLORS
   #if PEG_NUM_COLORS > 4
    muColors[PCI_SELECTED] = PCLR_ACTIVE_TITLE;
   #elif PEG_NUM_COLORS > 2
    muColors[PCI_SELECTED] = PCLR_BORDER;
   #else
    muColors[PCI_SELECTED] = PCLR_HIGHLIGHT;
   #endif
  #else
    muColors[PCI_SELECTED] = PCLR_ACTIVE_TITLE;
  #endif
    muColors[PCI_NTEXT] = PCLR_HIGH_TEXT;
    muColors[PCI_STEXT] = PCLR_HIGH_TEXT;

    InitClient();
    GetButtonRect();

    if (wStyle & PS_LED)
    {
        mpButton = NULL;
    }
    else
    {
        if (wStyle & PS_RECESSED)
        {
            mpButton = new PegButton(mButtonRect, 0, BF_SELECTED); 
        }
        else
        {
            mpButton = new PegButton(mButtonRect, 0, 0); 
        }
        Add(mpButton);
    }
    mpFont = &SysFont;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegProgressBar::Draw()
{
    PegColor LedColor(muColors[PCI_SELECTED], muColors[PCI_SELECTED], CF_FILL);

    BeginDraw();

    if (!(Style() & AF_TRANSPARENT))
    {
        StandardBorder(muColors[PCI_NORMAL]);
    }

    if (!(Style() & PS_LED))
    {
        mpButton->SetColor(PCI_NORMAL, muColors[PCI_SELECTED]);
        mpButton->Draw();
    }
    else
    {
        // here to draw the flat, LED style:
        
        SIGNED iLedSize;
        PegRect LedRect = mClient;

        if (Style() & PS_VERTICAL)
        {
            iLedSize = mClient.Height() / 20;

            if (iLedSize < 3)
            {
                iLedSize = 3;
            }
            LedRect.wTop = LedRect.wBottom - iLedSize + 2;

            while(LedRect.wTop > mButtonRect.wTop)
            {
                Rectangle(LedRect, LedColor, 0);
                LedRect.Shift(0, -iLedSize);
            }
        }
        else
        {
            iLedSize = mClient.Width() / 20;
            if (iLedSize < 3)
            {
                iLedSize = 3;
            }
            LedRect.wRight = LedRect.wLeft + iLedSize - 2;

            while(LedRect.wRight < mButtonRect.wRight)
            {
                Rectangle(LedRect, LedColor, 0);
                LedRect.Shift(iLedSize, 0);
            }
        }
    }

    // here to draw the progress bar value:

    if (Style() & PS_SHOW_VAL)
    {
        PEGCHAR cTemp[40];
        SIGNED iPercent;

        if (Style() & PS_PERCENT)
        {
            iPercent = (miCurrent - miMin)  * 100 / (miMax - miMin);
            _ltoa(iPercent, cTemp, 10);
        }
        else
        {
            _ltoa(miCurrent, cTemp, 10);
        }

        if (Style() & PS_PERCENT)
        {
           #ifdef PEG_UNICODE
            PEGCHAR cPercent[] = {'%', '\0'};
            strcat(cTemp, cPercent);
           #else
            strcat(cTemp, "%");
           #endif
        }
        
        // if the PCI_NTEXT is different from PCI_STEXT, and the
        // button rect top/right is somewhere under the text,
        // draw the text in a different color so it can be seen.
        LedColor.Set(muColors[PCI_NTEXT], muColors[PCI_NTEXT], CF_NONE);
        
        PegPoint Put;
        Put.x = (mClient.wLeft + mClient.wRight) / 2;
        Put.y = (mClient.wTop + mClient.wBottom) / 2;
        Put.x -= TextWidth(cTemp, mpFont) / 2;
        Put.y -= TextHeight(cTemp, mpFont) / 2;
        
        if (muColors[PCI_NTEXT] == muColors[PCI_STEXT])
        {
            DrawText(Put, cTemp, LedColor, mpFont);
        }
        else
        {
            BOOL bUseNormal = FALSE;
            BOOL bUseSelected = FALSE;
            //BOOL bUseBoth;
            
            if (Style() & PS_VERTICAL)
            {
                if (mButtonRect.wTop <= (Put.y + TextHeight(cTemp, mpFont)))
                {
                    if (mButtonRect.wTop >= Put.y)
                    {
                        //bUseBoth = TRUE;
                    }
                    else
                    {
                        bUseSelected = TRUE;
                    }
                }
                else
                {
                    bUseNormal = TRUE;
                }
            }
            else
            {
                if (mButtonRect.wRight >= Put.x)
                {
                    if(mButtonRect.wRight <= (Put.x + TextWidth(cTemp, mpFont)))
                    {
                        //bUseBoth = TRUE;
                    }
                    else
                    {
                        bUseSelected = TRUE;
                    }                    
                }
                else
                {
                    bUseNormal = TRUE;
                }                
            }            
            
            if (bUseNormal || bUseSelected)
            {
                if (bUseSelected)
                {
                    LedColor.Set(muColors[PCI_STEXT], muColors[PCI_STEXT], CF_NONE);
                }
    
                DrawText(Put, cTemp, LedColor, mpFont);                
            }
            else
            {
				// the progress rectangle is somewhere underneath
				// the text.
				PegRect tOldClip = mClip;
				if (Style() & PS_VERTICAL)
				{
					// Draw the selected color text first
					mClip.wBottom = Put.y + (TextHeight(cTemp, mpFont));
					mClip.wTop = mButtonRect.wTop;

					LedColor.Set(muColors[PCI_STEXT], muColors[PCI_STEXT], CF_NONE);
					DrawText(Put, cTemp, LedColor, mpFont);

					// Draw the normal colored text
					mClip.wBottom = mClip.wTop;
					mClip.wTop = Put.y;

					LedColor.Set(muColors[PCI_NTEXT], muColors[PCI_NTEXT], CF_NONE);
					DrawText(Put, cTemp, LedColor, mpFont);
				}
				else
				{
					// Draw the selected color text first
					mClip.wLeft = Put.x;
					mClip.wRight = mButtonRect.wRight;

					LedColor.Set(muColors[PCI_STEXT], muColors[PCI_STEXT], CF_NONE);
					DrawText(Put, cTemp, LedColor, mpFont);

					// Draw the normal colored text
					mClip.wLeft = mClip.wRight;
					mClip.wRight = Put.x + (TextWidth(cTemp, mpFont));

					LedColor.Set(muColors[PCI_NTEXT], muColors[PCI_NTEXT], CF_NONE);
					DrawText(Put, cTemp, LedColor, mpFont);
				}

				mClip = tOldClip;
            }               
        }        
    }
    EndDraw();
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegProgressBar::Update(SIGNED iVal, BOOL bDraw)
{
    Invalidate();
    miCurrent = iVal;
    GetButtonRect();

    if (!(Style() & PS_LED))
    {
        mpButton->Resize(mButtonRect);
    }

    if (bDraw)
    {
        Draw();
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegProgressBar::GetButtonRect(void)
{
    mButtonRect = mClient;

    if (miMax <= miMin)
    {
        return;
    }
    if (Style() & PS_VERTICAL)
    {
        mButtonRect.wTop = mButtonRect.wBottom;
        mButtonRect.wTop -= (mClient.Height() * (miCurrent - miMin)) / (miMax - miMin);
    }
    else
    {
        mButtonRect.wRight = mButtonRect.wLeft;
        mButtonRect.wRight += (mClient.Width() * (miCurrent - miMin)) / (miMax - miMin);
    }
}


// End of file
