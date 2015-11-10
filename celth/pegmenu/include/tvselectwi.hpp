/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: TvSelectWindowClass
//  
//   Copyright (c) ST microelectronics
//  
//    All Rights Reserved
//  
//  Notes:
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#ifndef _TVSELECTWI_HPP
#define _TVSELECTWI_HPP

#include "peg.hpp"
#include "tempheader.hpp" /*for virtal test, should be deleted when there is ap_cmd module in the software*/

#define max_string 10

typedef struct {
	int numberofString;
	const PEGCHAR* StringToDisplay[max_string]; /* string to display for a given language */
} HorizontalScrollText_t;
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class TvSelectWindowClass : public PegWindow
{
    public:

		TvSelectWindowClass(SIGNED iLeft, SIGNED iTop, SIGNED iRight, \
			APCTRL_LinkId_t eEventToNotify, APCTRL_PegState_t eSubStateToNotify, \
			HorizontalScrollText_t strings, int InitStringNumber, int menuID);
		SIGNED Message(const PegMessage &Mesg);
        void Draw(void);
        enum ChildIds {
            ID_SelectWindow_TextButton = 1,
        };
		int GetCurrentStringNumber(void);
		void ProcessUpandDownPress( int Number);
		void ProcessLeftandRightPress( int Number);

    private:
        PegTextButton *m_pSelectWindowTextButton;
		HorizontalScrollText_t StringToDisplay;
		int CurrentStringNumber;
		int parentMenu;
		APCTRL_LinkId_t eEvent;
		APCTRL_PegState_t eSubState;
};

#endif/*_TVSELECTWI_HPP*/