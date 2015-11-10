/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: SetupMenuClass
//  
//   Copyright (c) STMicroelectronics
//                 ZI de Rousset, BP2
//                 13106 ROUSSET CEDEX
//  
//    All Rights Reserved
//  
//  Notes:
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include "peg.hpp"
#include "wbstring.hpp"
#include "setup.hpp"
#include "mainmenu.hpp"
#include "service.hpp"
#include "tvdecbut.hpp"
#include "pbitmwin.hpp"
#include "tvprobar.hpp"
#include "anim.hpp"
#include "userfct.hpp"
#include "init_val.h"
#include "steditinput.hpp"

#include "stdio.h"
#include "stdlib.h"
char I2C0Value;
char I2C3Value;

extern U32	Frequency ;
extern U32	SymboleRate;
extern U16	ModulationMethod;

extern "C"
{
	ST_ErrorCode_t AddTransponder(U32 pFrequency,U32 pSymboleRate,U32 pModulationMethod);
}

extern PegFont chinesefont;
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Constructor
//  Create top level object and add all children
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SetupMenuClass::SetupMenuClass(SIGNED iLeft, SIGNED iTop) : 
    PegBitmapWindow(&gbPictBitmap, kAutoAdjustBitmapSize, PegBlueTransparent, FF_RAISED) 
{
    PegRect ChildRect;

    mReal.Set(iLeft, iTop, iLeft + 313, iTop + 279);
    InitClient();
    RemoveStatus(PSF_MOVEABLE|PSF_SIZEABLE);
#if 0
    ChildRect.Set(iLeft + 245, iTop + 146, iLeft + 304, iTop + 173);
    pCRTTypeStatus = new PegPrompt(ChildRect, LS(SID_CRT43), CRTTypeStatusID);
    pCRTTypeStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pCRTTypeStatus);

    ChildRect.Set(iLeft + 245, iTop + 106, iLeft + 304, iTop + 133);
    pI2C3Status = new PegPrompt(ChildRect, LS(SID_ON), I2C3StatusID);
    pI2C3Status->SetColor(PCI_NTEXT, BLUE);
    Add(pI2C3Status);

    ChildRect.Set(iLeft + 245, iTop + 66, iLeft + 304, iTop + 93);
    pI2C0Status = new PegPrompt(ChildRect, LS(SID_ON), I2C0StatusID);
    pI2C0Status->SetColor(PCI_NTEXT, BLUE);
    Add(pI2C0Status);

    ChildRect.Set(iLeft + 245, iTop + 186, iLeft + 304, iTop + 213);
		pTranslucencyBar = new TVProgressBar(ChildRect, FF_RAISED|PS_LED|PS_SHOW_VAL, ucTranslucencyMin, ucTranslucencyMax, 32);
    Add(pTranslucencyBar);
#endif
    ChildRect.Set(iLeft + 13, iTop + 226, iLeft + 113, iTop + 253);
    pCancelbutton = new TVDecoratedButton(ChildRect, LS(SID_ChannelCancelButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, CancelButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pCancelbutton->SetColor(PCI_NORMAL, 223);
    pCancelbutton->SetColor(PCI_NTEXT, BLUE);
    Add(pCancelbutton);

    ChildRect.Set(iLeft + 13, iTop + 186, iLeft + 113, iTop + 213);
    pOKbutton = new TVDecoratedButton(ChildRect, LS(SID_ChannelOKButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, OKButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pOKbutton->SetColor(PCI_NORMAL, 223);
    pOKbutton->SetColor(PCI_NTEXT, BLUE);
    Add(pOKbutton);


    ChildRect.Set(iLeft + 123, iTop + 146, iLeft + 242, iTop + 173);
    LabelModulation = new PegPrompt(ChildRect, LS(SID_LabelModulation), LabelModulationID);
    LabelModulation->SetColor(PCI_NTEXT, BLUE);
    Add(LabelModulation);

    ChildRect.Set(iLeft + 123, iTop + 106, iLeft + 242, iTop + 133);
    LabelSymbolrate = new PegPrompt(ChildRect, LS(SID_LabelSymbolrate), LabelSymbolrateID);
    LabelSymbolrate->SetColor(PCI_NTEXT, BLUE);
    Add(LabelSymbolrate);

    ChildRect.Set(iLeft + 123, iTop + 66, iLeft + 242, iTop + 93);
    LabelFrequency = new PegPrompt(ChildRect, LS(SID_LabelFrequency), LabelFrequencyID);
    LabelFrequency->SetColor(PCI_NTEXT, BLUE);
    Add(LabelFrequency);


    ChildRect.Set(iLeft + 13, iTop + 146, iLeft + 113, iTop + 173);
    EditModulation = new MutilSelectString(ChildRect);
    Add(EditModulation);


    ChildRect.Set(iLeft + 13, iTop + 106, iLeft + 113, iTop + 133);
    EditSymbolrate = new stEditInput(ChildRect);
    Add(EditSymbolrate);

    ChildRect.Set(iLeft + 13, iTop + 66, iLeft + 113, iTop + 93);
    EditFrequency = new stEditInput(ChildRect);
    Add(EditFrequency);

/*    ChildRect.Set(iLeft + 20, iTop + 15, iLeft + 160, iTop + 44);
    pSetupMenuWindowTitle = new PegPrompt(ChildRect, LS(SID_SetupMenuWindowTitle), SetupMenuWindowTitleID, FF_NONE|AF_TRANSPARENT|TJ_CENTER);
    pSetupMenuWindowTitle->SetColor(PCI_NORMAL, 130);
    pSetupMenuWindowTitle->SetColor(PCI_NTEXT, BLUE);
    Add(pSetupMenuWindowTitle); */

    /* WB End Construction */
		Id(SETUP_MENU_ID);

	if (gbCurrentLanguage == 1) /* language = chinese */
	{
		LabelFrequency->SetFont(&chinesefont);
		LabelSymbolrate->SetFont(&chinesefont);
		LabelModulation->SetFont(&chinesefont);
		pOKbutton->SetFont(&chinesefont);
		pCancelbutton->SetFont(&chinesefont);
	}
	else if (gbCurrentLanguage == 0)
	{
		LabelFrequency->SetFont(&Pronto_12);
		LabelSymbolrate->SetFont(&Pronto_12);
		LabelModulation->SetFont(&Pronto_12);
		pOKbutton->SetFont(&Pronto_12);
		pCancelbutton->SetFont(&Pronto_12);
	}

		EditFrequency->SetFont(&Pronto_12);
		EditSymbolrate->SetFont(&Pronto_12);
		EditModulation->SetFont(&Pronto_12); 
#if 0
		if (gbCurrentLanguage == 1) /* language = chinese */
		{
	    pSetupMenuWindowTitle->SetFont(&A12RFont);
		}
		else if (gbCurrentLanguage == 2)  /* language = korean */
		{
	    pSetupMenuWindowTitle->SetFont(&korean);
		}
		else /* default for English & French */
		{
	    pSetupMenuWindowTitle->SetFont(&Pronto_16);
		}
#endif

		RefreshSetupWindow(); /* initialize window */
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED SetupMenuClass::Message(const PegMessage &Mesg)
{
		BOOL bValueChanged = FALSE;
		char a[20];
		switch (Mesg.wType)
    {
	case PM_KEYRIGHT:
	case PM_KEYLEFT:
				switch(Mesg.iData)
				{
					case OKButtonID:
						printf("the input is %s",(char*)EditFrequency ->DataGet());
						break;
					case CancelButtonID:
						break;
					case CRTButtonID:
						break;

					case I2C0ButtonID:
						if (Mesg.wType == PM_KEYRIGHT)
						{
						}
						else
						{
						}

						break;

					case I2C3ButtonID:
						if (Mesg.wType == PM_KEYRIGHT)
						{
						}
						else
						{
						}
						break;
				}
				break;
	 case SIGNAL(OKButtonID, PSF_CLICKED):
		strcpy(a,EditFrequency ->DataGet());
		Frequency = (U32)atoi(a);
		strcpy(a,EditSymbolrate ->DataGet());
		SymboleRate = (U32)atoi(a);

		ModulationMethod = (U16)EditModulation ->GetModulationStatus();
		AddTransponder(Frequency,SymboleRate,ModulationMethod);
		break;

	case PM_CLOSE:
			BackToPreviousMenu();
			break;

	default:
		return PegWindow::Message(Mesg);
    }
    return 0;
}

void SetupMenuClass::BackToPreviousMenu(void)
{
	ServiceMenuClass *pServiceWindow;

	Destroy(this);
	pServiceWindow = (ServiceMenuClass*) Presentation()->Find(SERVICE_MENU_ID);
//	pServiceWindow->SetBitmap(&gbMainBitmap);
	pServiceWindow->SetBackgroundColor(PegBlueTransparent);
}


void SetupMenuClass::RefreshSetupWindow(void)
{

}
