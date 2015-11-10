/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pspin.hpp - PegSpinButton class definition.
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

#ifndef _PSPIN_
#define _PSPIN_

class PegSpinButton : public PegThing
{
    public:
        PegSpinButton(SIGNED iLeft, SIGNED iTop, WORD wId = 0,
            WORD wStyle = AF_ENABLED|SB_VERTICAL);
        PegSpinButton(SIGNED iLeft, SIGNED iTop, PegTextThing *buddy,
            LONG lMin, LONG lMax, SIGNED iStep, WORD wId = 0,
            WORD wStyle = AF_ENABLED|SB_VERTICAL);
        PegSpinButton(const PegRect &Rect, WORD wId = 0,
            WORD wStyle = AF_ENABLED|SB_VERTICAL);
        PegSpinButton(const PegRect &Rect, PegTextThing *buddy,
            LONG lMin, LONG lMax, SIGNED iStep, WORD wId =0,
            WORD wStyle = AF_ENABLED|SB_VERTICAL);
        virtual SIGNED Message(const PegMessage &Mesg);
        void SetLimits(LONG lMin, LONG lMax, SIGNED iStep = -1);
        void SetBuddy(PegTextThing *buddy)
        {
            mpSlave = buddy;
            if (mpSlave)
            {
                mpSlave->SetCopyMode();
            }
        }
        void SetOutputWidth(SIGNED iWidth) {miOutputWidth = iWidth;}
                
    protected:

        enum MyButtonIds {
            IDB_UPBUTTON = 1010,
            IDB_DOWNBUTTON,
            IDB_LEFTBUTTON,
            IDB_RIGHTBUTTON
        };

        void AddSpinButtons(void);
        void UpdateSlave(WORD wDir);

        PegTextThing *mpSlave;
        LONG mlMin;
        LONG mlMax;
        SIGNED miStep;
        SIGNED miOutputWidth;
};

#endif

