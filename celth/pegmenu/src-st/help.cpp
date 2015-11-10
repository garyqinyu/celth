/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: HelpMenuClass
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
#include "help.hpp"
#include "anim.hpp"
#include "mainmenu.hpp"
#include "usertype.hpp"
#include "pbitmwin.hpp"
#include "tvdecbut.hpp"
#include "userfct.hpp"


int iMenu = 0;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Constructor
//  Create top level object and add all children
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
HelpMenuClass::HelpMenuClass(SIGNED iLeft, SIGNED iTop) : 
    PegBitmapWindow(NULL, kAutoAdjustBitmapSize, WHITE, FF_RAISED) 
{
    PegRect ChildRect;

    mReal.Set(iLeft, iTop, iLeft + 339, iTop + 334);
    InitClient();
    RemoveStatus(PSF_MOVEABLE|PSF_SIZEABLE);

    ChildRect.Set(iLeft + 2, iTop + 223, iLeft + 123, iTop + 239);
    pCancelStatus = new PegPrompt(ChildRect, LS(SID_Cancel), CancelStatusID, FF_NONE|AF_TRANSPARENT|TJ_RIGHT);
    pCancelStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pCancelStatus);

    ChildRect.Set(iLeft + 2, iTop + 204, iLeft + 123, iTop + 220);
    pMixStatus = new PegPrompt(ChildRect, LS(SID_Mix), MixStatusID, FF_NONE|AF_TRANSPARENT|TJ_RIGHT);
    pMixStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pMixStatus);

    ChildRect.Set(iLeft + 2, iTop + 186, iLeft + 123, iTop + 202);
    pRevealStatus = new PegPrompt(ChildRect, LS(SID_Reveal), RevealStatusID, FF_NONE|AF_TRANSPARENT|TJ_RIGHT);
    pRevealStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pRevealStatus);

    ChildRect.Set(iLeft + 218, iTop + 191, iLeft + 339, iTop + 207);
    pSubtitleStatus = new PegPrompt(ChildRect, LS(SID_Subtitle), SubtitleStatusID);
    pSubtitleStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pSubtitleStatus);

    ChildRect.Set(iLeft + 6, iTop + 43, iLeft + 125, iTop + 59);
    pColorLinksStatus = new PegPrompt(ChildRect, LS(SID_ColorLinks), ColorLinksStatusID, FF_NONE|AF_TRANSPARENT|TJ_RIGHT);
    pColorLinksStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pColorLinksStatus);

    ChildRect.Set(iLeft + 217, iTop + 57, iLeft + 338, iTop + 73);
    pTxtZoomStatus = new PegPrompt(ChildRect, LS(SID_TxtZoom), TxtZoomStatusID);
    pTxtZoomStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pTxtZoomStatus);

    ChildRect.Set(iLeft + 217, iTop + 205, iLeft + 338, iTop + 221);
    pTwbStatus = new PegPrompt(ChildRect, LS(SID_Teleweb), TwbStatusID);
    pTwbStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pTwbStatus);

    ChildRect.Set(iLeft + 217, iTop + 181, iLeft + 338, iTop + 197);
    pAVToogleStatus = new PegPrompt(ChildRect, LS(SID_AVToggle), AVToggleStatusID);
    pAVToogleStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pAVToogleStatus);

    ChildRect.Set(iLeft + 217, iTop + 163, iLeft + 338, iTop + 179);
    pZoomSubcodeStatus = new PegPrompt(ChildRect, LS(SID_ZoomMode), ZoomStatusID);
    pZoomSubcodeStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pZoomSubcodeStatus);

    ChildRect.Set(iLeft + 217, iTop + 72, iLeft + 338, iTop + 88);
    pTeletextStatus = new PegPrompt(ChildRect, LS(SID_TeletextOnOff), TeletextStatusID);
    pTeletextStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pTeletextStatus);

    ChildRect.Set(iLeft + 217, iTop + 114, iLeft + 338, iTop + 130);
    pPrgPPrgMStatus = new PegPrompt(ChildRect, LS(SID_PrgPlusMinus), PrgPPrgMStatusID);
    pPrgPPrgMStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pPrgPPrgMStatus);

    ChildRect.Set(iLeft + 217, iTop + 85, iLeft + 338, iTop + 101);
    pOKStatus = new PegPrompt(ChildRect, LS(SID_OK), OKStatusID);
    pOKStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pOKStatus);

    ChildRect.Set(iLeft + 217, iTop + 42, iLeft + 338, iTop + 58);
    pStandbyIndexStatus = new PegPrompt(ChildRect, LS(SID_Standby), StandbyIndexStatusID);
    pStandbyIndexStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pStandbyIndexStatus);

    ChildRect.Set(iLeft + 2, iTop + 144, iLeft + 123, iTop + 160);
    pDigitStatus = new PegPrompt(ChildRect, LS(SID_Digit), DigitStatusID, FF_NONE|AF_TRANSPARENT|TJ_RIGHT);
    pDigitStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pDigitStatus);

    ChildRect.Set(iLeft + 2, iTop + 116, iLeft + 123, iTop + 132);
    pSoundStatus = new PegPrompt(ChildRect, LS(SID_SoundPlusMinusMute), SoundStatusID, FF_NONE|AF_TRANSPARENT|TJ_RIGHT);
    pSoundStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pSoundStatus);

    ChildRect.Set(iLeft + 6, iTop + 97, iLeft + 125, iTop + 113);
    pLeftRightStatus = new PegPrompt(ChildRect, LS(SID_LeftRight), LeftRightStatusID, FF_NONE|AF_TRANSPARENT|TJ_RIGHT);
    pLeftRightStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pLeftRightStatus);

    ChildRect.Set(iLeft + 6, iTop + 81, iLeft + 125, iTop + 97);
    pMenuStatus = new PegPrompt(ChildRect, LS(SID_Menu), MenuStatusID, FF_NONE|AF_TRANSPARENT|TJ_RIGHT);
    pMenuStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pMenuStatus);

    ChildRect.Set(iLeft + 6, iTop + 65, iLeft + 125, iTop + 81);
    pUpDownStatus = new PegPrompt(ChildRect, LS(SID_UpDown), UpDownStatusID, FF_NONE|AF_TRANSPARENT|TJ_RIGHT);
    pUpDownStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pUpDownStatus);

    ChildRect.Set(iLeft + 155, iTop + 294, iLeft + 280, iTop + 321);
    pHelpStatus = new PegPrompt(ChildRect, LS(SID_General), HelpStatusID, FF_NONE|AF_TRANSPARENT|TJ_CENTER);
    pHelpStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pHelpStatus);

    ChildRect.Set(iLeft + 290, iTop + 297, iLeft + 317, iTop + 319);
    pRightBitmap = new PegIcon(ChildRect, &gbRighiconBitmap);
		pRightBitmap->RemoveStatus(PSF_ACCEPTS_FOCUS);
		Add(pRightBitmap);

    ChildRect.Set(iLeft + 17, iTop + 298, iLeft + 44, iTop + 320);
    pLeftBitmap = new PegIcon(ChildRect, &gbLefticonBitmap);
		pLeftBitmap->RemoveStatus(PSF_ACCEPTS_FOCUS);
		Add(pLeftBitmap);

    ChildRect.Set(iLeft + 50, iTop + 294, iLeft + 149, iTop + 321);
		pHelpSelectionButton = new TVDecoratedButton(ChildRect, LS(SID_HelpMenuWindowTitle), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, HelpSelectionButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pHelpSelectionButton->SetColor(PCI_NORMAL, 223);
    pHelpSelectionButton->SetColor(PCI_NTEXT, BLUE);
    pHelpSelectionButton->AddStatus(PSF_ACCEPTS_FOCUS);
    Add(pHelpSelectionButton);

    ChildRect.Set(iLeft + 131, iTop + 32, iLeft + 213, iTop + 264);
		pRCBitmap = new PegIcon(ChildRect, &gbRC_menBitmap);
		pRCBitmap->RemoveStatus(PSF_ACCEPTS_FOCUS);
		Add(pRCBitmap);
 
		/* WB End Construction */

		Id(HELP_MENU_ID);

		pCancelStatus->RemoveStatus(PSF_ACCEPTS_FOCUS);
    pMixStatus->RemoveStatus(PSF_ACCEPTS_FOCUS);
    pRevealStatus->RemoveStatus(PSF_ACCEPTS_FOCUS);
    pSubtitleStatus->RemoveStatus(PSF_ACCEPTS_FOCUS);
    pColorLinksStatus->RemoveStatus(PSF_ACCEPTS_FOCUS);
    pTxtZoomStatus->RemoveStatus(PSF_ACCEPTS_FOCUS);
    pTwbStatus->RemoveStatus(PSF_ACCEPTS_FOCUS);
    pAVToogleStatus->RemoveStatus(PSF_ACCEPTS_FOCUS);
    pZoomSubcodeStatus->RemoveStatus(PSF_ACCEPTS_FOCUS);
    pTeletextStatus->RemoveStatus(PSF_ACCEPTS_FOCUS);
    pPrgPPrgMStatus->RemoveStatus(PSF_ACCEPTS_FOCUS);
    pOKStatus->RemoveStatus(PSF_ACCEPTS_FOCUS);
    pStandbyIndexStatus->RemoveStatus(PSF_ACCEPTS_FOCUS);
    pDigitStatus->RemoveStatus(PSF_ACCEPTS_FOCUS);
    pSoundStatus->RemoveStatus(PSF_ACCEPTS_FOCUS);
    pLeftRightStatus->RemoveStatus(PSF_ACCEPTS_FOCUS);
    pMenuStatus->RemoveStatus(PSF_ACCEPTS_FOCUS);
    pUpDownStatus->RemoveStatus(PSF_ACCEPTS_FOCUS);
    pHelpStatus->RemoveStatus(PSF_ACCEPTS_FOCUS);

		if (gbCurrentLanguage == 1) /* language = chinese */
		{
				pCancelStatus->SetFont(&A12RFont);
        pMixStatus->SetFont(&A12RFont);
        pRevealStatus->SetFont(&A12RFont);
        pSubtitleStatus->SetFont(&A12RFont);
        pColorLinksStatus->SetFont(&A12RFont);
        pTxtZoomStatus->SetFont(&A12RFont);
        pTwbStatus->SetFont(&A12RFont);
        pAVToogleStatus->SetFont(&A12RFont);
        pZoomSubcodeStatus->SetFont(&A12RFont);
        pTeletextStatus->SetFont(&A12RFont);
        pPrgPPrgMStatus->SetFont(&A12RFont);
        pOKStatus->SetFont(&A12RFont);
        pStandbyIndexStatus->SetFont(&A12RFont);
        pDigitStatus->SetFont(&A12RFont);
        pSoundStatus->SetFont(&A12RFont);
        pLeftRightStatus->SetFont(&A12RFont);
        pMenuStatus->SetFont(&A12RFont);
        pUpDownStatus->SetFont(&A12RFont);
        pHelpStatus->SetFont(&A12RFont);
				pHelpSelectionButton->SetFont(&A12RFont);
		}
		else if (gbCurrentLanguage == 2)  /* language = korean */
		{
				pCancelStatus->SetFont(&korean);
        pMixStatus->SetFont(&korean);
        pRevealStatus->SetFont(&korean);
        pSubtitleStatus->SetFont(&korean);
        pColorLinksStatus->SetFont(&korean);
        pTxtZoomStatus->SetFont(&korean);
        pTwbStatus->SetFont(&korean);
        pAVToogleStatus->SetFont(&korean);
        pZoomSubcodeStatus->SetFont(&korean);
        pTeletextStatus->SetFont(&korean);
        pPrgPPrgMStatus->SetFont(&korean);
        pOKStatus->SetFont(&korean);
        pStandbyIndexStatus->SetFont(&korean);
        pDigitStatus->SetFont(&korean);
        pSoundStatus->SetFont(&korean);
        pLeftRightStatus->SetFont(&korean);
        pMenuStatus->SetFont(&korean);
        pUpDownStatus->SetFont(&korean);
        pHelpStatus->SetFont(&korean);
				pHelpSelectionButton->SetFont(&korean);
		}
		else /* default for English & French */
		{
				pCancelStatus->SetFont(&Pronto_10);
        pMixStatus->SetFont(&Pronto_10);
        pRevealStatus->SetFont(&Pronto_10);
        pSubtitleStatus->SetFont(&Pronto_10);
        pColorLinksStatus->SetFont(&Pronto_10);
        pTxtZoomStatus->SetFont(&Pronto_10);
        pTwbStatus->SetFont(&Pronto_10);
        pAVToogleStatus->SetFont(&Pronto_10);
        pZoomSubcodeStatus->SetFont(&Pronto_10);
        pTeletextStatus->SetFont(&Pronto_10);
        pPrgPPrgMStatus->SetFont(&Pronto_10);
        pOKStatus->SetFont(&Pronto_10);
        pStandbyIndexStatus->SetFont(&Pronto_10);
        pDigitStatus->SetFont(&Pronto_10);
        pSoundStatus->SetFont(&Pronto_10);
        pLeftRightStatus->SetFont(&Pronto_10);
        pMenuStatus->SetFont(&Pronto_10);
        pUpDownStatus->SetFont(&Pronto_10);
        pHelpStatus->SetFont(&Pronto_10);
				pHelpSelectionButton->SetFont(&Pronto_10);
		}
		
		DisplayHelp(); /* setup help window */

}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED HelpMenuClass::Message(const PegMessage &Mesg)
{
		MainMenuClass *pMainWindow;

    switch (Mesg.wType)
    {
		
		case PM_KEYLEFT:
		case PM_KEYRIGHT:
				switch(Mesg.iData)
				{
					case HelpSelectionButtonID:
						if (Mesg.wType == PM_KEYRIGHT)
						{
						}
						else
						{
						}
						DisplayHelp(); /* setup help window */
						break;
				}
				break;

		case PM_CLOSE:
			  /* left key = close = go back to previous menu */
				Destroy(this);
				pMainWindow = (MainMenuClass*) Presentation()->Find(MAIN_MENU_ID);
				pMainWindow->SetBitmap(&gbMainBitmap);
				break;

    default:
        return PegWindow::Message(Mesg);
    }
    return 0;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void HelpMenuClass::DisplayHelp(void)
{
	BeginDraw();
	if (iMenu == 0 )
	{ /* General Menu */
				pCancelStatus->DataSet(NULL);
        pMixStatus->DataSet(NULL);
        pRevealStatus->DataSet(NULL);
        pSubtitleStatus->DataSet(NULL);
        pColorLinksStatus->DataSet(NULL);
        pTxtZoomStatus->DataSet(NULL);
        pTwbStatus->DataSet(LS(SID_TelewebOnOff));
        pAVToogleStatus->DataSet(LS(SID_AVToggle));
        pZoomSubcodeStatus->DataSet(LS(SID_ZoomMode));
        pTeletextStatus->DataSet(LS(SID_TeletextOnOff));
        pPrgPPrgMStatus->DataSet(LS(SID_PrgPlusMinus));
        pOKStatus->DataSet(LS(SID_OK));
        pStandbyIndexStatus->DataSet(LS(SID_Standby));
        pDigitStatus->DataSet(LS(SID_Digit));
        pSoundStatus->DataSet(LS(SID_SoundPlusMinusMute));
        pLeftRightStatus->DataSet(LS(SID_LeftRight));
        pMenuStatus->DataSet(LS(SID_Menu));
        pUpDownStatus->DataSet(LS(SID_UpDown));
				pRCBitmap->SetIcon(&gbRC_menBitmap);
	}
	else 	if (iMenu == 1 )
	{ /* Txt Menu */
				pCancelStatus->DataSet(LS(SID_Cancel));
        pMixStatus->DataSet(LS(SID_Mix));
        pRevealStatus->DataSet(LS(SID_Reveal));
        pSubtitleStatus->DataSet(LS(SID_Subtitle));
        pColorLinksStatus->DataSet(LS(SID_ColorLinks));
        pTxtZoomStatus->DataSet(LS(SID_TxtZoom));
        pTwbStatus->DataSet(NULL);
        pAVToogleStatus->DataSet(NULL);
        pZoomSubcodeStatus->DataSet(LS(SID_Subcode));
        pTeletextStatus->DataSet(LS(SID_TeletextOnOff));
        pPrgPPrgMStatus->DataSet(LS(SID_PagePlusMinus));
        pOKStatus->DataSet(NULL);
        pStandbyIndexStatus->DataSet(LS(SID_Index));
        pDigitStatus->DataSet(LS(SID_DigitTxt));
        pSoundStatus->DataSet(LS(SID_SoundPlusMinusMute));
        pLeftRightStatus->DataSet(LS(SID_PreviousPage));
        pMenuStatus->DataSet(LS(SID_ListBasicModes));
        pUpDownStatus->DataSet(LS(SID_SubpagePlusMinus));
				pRCBitmap->SetIcon(&gbRC_txtBitmap);
	}
	else 	if (iMenu == 2 )
	{ /* Twb Menu */
				pCancelStatus->DataSet(NULL);
        pMixStatus->DataSet(NULL);
        pRevealStatus->DataSet(NULL);
        pSubtitleStatus->DataSet(NULL);
        pColorLinksStatus->DataSet(NULL);
        pTxtZoomStatus->DataSet(NULL);
        pTwbStatus->DataSet(LS(SID_TelewebOnOff));
        pAVToogleStatus->DataSet(NULL);
        pZoomSubcodeStatus->DataSet(NULL);
        pTeletextStatus->DataSet(NULL);
        pPrgPPrgMStatus->DataSet(NULL);
        pOKStatus->DataSet(LS(SID_OK));
        pStandbyIndexStatus->DataSet(NULL);
        pDigitStatus->DataSet(NULL);
        pSoundStatus->DataSet(LS(SID_SoundPlusMinusMute));
        pLeftRightStatus->DataSet(LS(SID_LeftRightTwb));
        pMenuStatus->DataSet(LS(SID_Menu));
        pUpDownStatus->DataSet(LS(SID_UpDown));
				pRCBitmap->SetIcon(&gbRC_twbBitmap);
	}
	EndDraw();
	Invalidate();
	Draw();
}
