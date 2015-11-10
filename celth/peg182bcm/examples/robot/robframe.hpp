/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// robframe.hpp
//
// Author: Kenneth G. Maxwell
//
// Copyright (c) 1997-2002 Swell Software 
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



class RobotFrame : public PegAnimationWindow
{
	public:
		RobotFrame(PegRect &Rect);
        SIGNED Message(const PegMessage &Mesg);
		void Draw(void);

    private:
        enum MyButtons {
            IDB_RUN = 1,
            IDB_STOP,
            IDS_SPEED,
        };

        void DrawTitles(void);
        void ShadowText(char *pText, PegPoint Put, PegFont *pFont,
	        SIGNED iWidth);
        void UpdateRobotPosition(UCHAR uFrame);

        PegPrompt *mpXPos;
        PegPrompt *mpYPos;
        PegPrompt *mpZPos;
        PegPrompt *mpWeldPrompt;
        StatusBar *mpStatBar;
        BOOL mbForward;
        BOOL mbWeldOn;
        BOOL mbRunning;
        UCHAR mTestBitmapCount;
        PegBitmap *mpTestMaps[5];
};
