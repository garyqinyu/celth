
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "peg.hpp"
#include "bolas.h"
#include "wbstring.hpp"

const int TimerId = 99;
const int AnimateId = 120;
const int FinishId = 240;
const int HUMAN_MODE = 0;
const int AI_MODE_1 = 1;
const int AI_MODE_2 = 2;
const int AI_MODE_3 = 3;
const int ALPHA_SENTINEL = -1000;
const int BETA_SENTINEL = 1000;
const int SEARCH_DEPTH = 2;
int MOVE_LENGTH = 1;

int	Taper1[2] = { 99, 99};
int Taper2[2] = { 99, 99};
int	Taper3[2] = { 5, 5 };
int FullDepth[2] = { SEARCH_DEPTH, SEARCH_DEPTH };
char PartialDepth = 0;
int MaxDepth = SEARCH_DEPTH;
int	AlphaBeta[2] = { 0, 0 };
int	IterativeDeepening[2] = { 0, 0 };
int	BallMorph = 0;
int	Trace = 0;
int Jumping = 0;
int Jumped;
int	Alpha;
int	Beta;
long	MovesConsidered;
long	MovesRejected;
long	NumberOfNodes;
long	RedundantMoves;
long	SecondarySearch;

NODE	*TreeParent;
//char *(ModeDesc[4]) = { "Human", "Best Move", "BFS-2", "MiniMax" };
char *(ModeDesc[4]) = { "Human", "Easy", "Medium", "Difficult" };

int FOCUSX = 0;
int FOCUSY = 0;
int LASTROW = 0;
int LASTCOL = 0;
	   				 				   			   			
int ActivePlayer;
int NumberOfGames=1;
int CurrentGame=0;
int StartPlayer = PLAYER1;
long starttime = 0;
bool Animating = false;

extern UCHAR PegCustomPalette[];

extern PegBitmap gbPlayer1Bitmap;
extern PegBitmap gbPlayer2Bitmap;
extern PegBitmap gbPlayer1SmallBitmap;
extern PegBitmap gbPlayer2SmallBitmap;
extern PegBitmap gbPlayer1sBitmap;
extern PegBitmap gbPlayer2sBitmap;
extern PegBitmap gbPlayer1tBitmap;
extern PegBitmap gbPlayer2tBitmap;
extern PegBitmap gbPlayer1TurnBitmap;
extern PegBitmap gbPlayer2TurnBitmap;
extern PegBitmap gbPlayer1VoltsBitmap;
extern PegBitmap gbPlayer2VoltsBitmap;
extern PegBitmap gbP1blankBitmap;
extern PegBitmap gbPlayer1MicroBitmap;
extern PegBitmap gbPlayer1LitBitmap;
extern PegBitmap gbPlayer1MediumBitmap;
extern PegBitmap gbPlayer2BlankBitmap;
extern PegBitmap gbPlayer2MicroBitmap;
extern PegBitmap gbPlayer2LitBitmap;
extern PegBitmap gbPlayer2MediumBitmap;
extern PegBitmap gbBlankTurnBitmap;
extern PegBitmap gbEmptyBitmap;
extern PegBitmap gbGameOverBitmap;
extern PegBitmap gbBoardBitmap;
extern PegBitmap gbLogoBitmap;


struct {
	int Mode;
	int Wins;
	int Losses;
	int Count;
	int SelRow;
	int SelCol;
	int Selected;
   int NewPieces;
	long ticks;
	} Player[ 2 ];


unsigned VisibleBoard[ NUM_SQUARES_Y ];
const int BOARD_MASK =  0x3;
const int PLAYER1_COUNT_OFFSET_X = 44;
const int PLAYER1_COUNT_OFFSET_Y = 92;
const int PLAYER2_COUNT_OFFSET_X = 144;
const int PLAYER2_COUNT_OFFSET_Y = PLAYER1_COUNT_OFFSET_Y;
const int PLAYER1_MODE_OFFSET_X = 22;
const int PLAYER2_MODE_OFFSET_X = 112;
const int PLAYER_MODE_OFFSET_Y = 52;
const int PLAYER1_WIN_OFFSET_X = 24;
const int PLAYER2_WIN_OFFSET_X = 114;
const int PLAYER_WIN_OFFSET_Y = 132;
const int PLAYER_LOSS_OFFSET_Y = 132;
const int PLAYER1_LOSS_OFFSET_X = 64;
const int PLAYER2_LOSS_OFFSET_X = 154;
const int CUR_GAME_OFFSET_X = 64;
const int CUR_GAME_OFFSET_Y = 172;
const int TOT_GAME_OFFSET_X = 114;
const int TOT_GAME_OFFSET_Y = CUR_GAME_OFFSET_Y;
const int PLAYER1_SMALL_OFFSET_X = 30;
const int PLAYER1_SMALL_OFFSET_Y = 3;
const int PLAYER2_SMALL_OFFSET_X = 130;
const int PLAYER1_TIME_OFFSET_X = 9;
const int PLAYER2_TIME_OFFSET_X = 154;
const int PLAYER_TIME_OFFSET_Y = CUR_GAME_OFFSET_Y;
const int SMALL_SIZE = 30;
const int TURN_SIZE = 30;
const int TURN1_OFFSET_X = 60;
const int TURN2_OFFSET_X = 100;
const int TURN_OFFSET_Y = 5;
const int MAX_DEPTH = 12;


// ----------- Board Manipulation --------------
/*--------------------------------------------------------------------------*/

int	GetBoardValue( unsigned Brd[NUM_SQUARES_Y], int row, int col );
int	CountBoardValues( unsigned Brd[NUM_SQUARES_Y], unsigned int val );
int	AnyMovesAvailable( unsigned Brd[NUM_SQUARES_Y], int player );
void 	UpdateAlpha( NODE *pTmp );



int ANI_WAIT(int a) {return a==EMPTY?-1:(a==PLAYER2?ANI_WAIT2:ANI_WAIT1);}
int ANI_UP(int a)	{return a==EMPTY?-1:(a==PLAYER2?ANI_UP2	:ANI_UP1);}
int MICRO(int a)	{return a==EMPTY?-1:(a==PLAYER2?MICRO2	:MICRO1);}
int MEDIUM(int a)	{return a==EMPTY?-1:(a==PLAYER2?MEDIUM2	:MEDIUM1);}
int PLAYER(int a)	{return a==EMPTY?-1:(a==PLAYER2?PLAYER2	:PLAYER1);}



/*--------------------------------------------------------------------------*/
//--------------- OptionDialog ---------------
/*--------------------------------------------------------------------------*/

OptionDialog::OptionDialog(PegRect Rect, const PEGCHAR *name)
		  : PegDialog(Rect, name)
{
    PegRect ChildRect;
    PegThing *pChild1;

	int iLeft = Rect.wLeft;
	int iTop  = Rect.wTop + 20;

    ChildRect.Set(iLeft + 55, iTop + 9, iLeft + 144, iTop + 28);
    Add(new PegPrompt(ChildRect, LS(SID_PL1), 0, FF_NONE|AF_TRANSPARENT|TJ_CENTER));
    ChildRect.Set(iLeft + 210, iTop + 9, iLeft + 299, iTop + 29);
    Add(new PegPrompt(ChildRect, LS(SID_PL2), 0, FF_NONE|AF_TRANSPARENT|TJ_CENTER));

//    ChildRect.Set(iLeft + 251, iTop + 387, iLeft + 330, iTop + 413);
	ChildRect.Set(iLeft + 251, iTop + 180, iLeft + 330, iTop + 206);
    Add(new PegTextButton(ChildRect, LS(SID_HELP), IDB_HELP, AF_ENABLED|TJ_CENTER));
//    ChildRect.Set(iLeft + 142, iTop + 387, iLeft + 221, iTop + 413);
	ChildRect.Set(iLeft + 142, iTop + 180, iLeft + 221, iTop + 206);
    Add(new PegTextButton(ChildRect, LS(SID_CANCEL), IDB_CANCEL, AF_ENABLED|TJ_CENTER));
//    ChildRect.Set(iLeft + 32, iTop + 387, iLeft + 111, iTop + 413);
	ChildRect.Set(iLeft + 32, iTop + 180, iLeft + 111, iTop + 206);
    Add(new PegTextButton(ChildRect, LS(SID_OK), IDB_OK, AF_ENABLED|TJ_CENTER));

/*
    ChildRect.Set(iLeft + 21, iTop + 303, iLeft + 339, iTop + 365);
    pChild1 = new PegGroup(ChildRect, NULL);
*/

//    ChildRect.Set(iLeft + 114, iTop + 318, iLeft + 139, iTop + 337);
	ChildRect.Set(iLeft + 134, iTop + 150, iLeft + 159, iTop + 169);
    pNumGames = new PegString(ChildRect, LS(SID_ONE), 0, FF_RECESSED|AF_ENABLED|TT_COPY|EF_EDIT);
	/*pChild1->*/Add(pNumGames);
//    ChildRect.Set(iLeft + 27, iTop + 317, iLeft + 116, iTop + 336);
	ChildRect.Set(iLeft + 47, iTop + 149, iLeft + 136, iTop + 168);
    /*pChild1->*/Add(new PegPrompt(ChildRect, LS(SID_NUMGAMES)));
//  ChildRect.Set(iLeft + 255, iTop + 342, iLeft + 344, iTop + 361);
	ChildRect.Set(iLeft + 205, iTop + 150, iLeft + 294, iTop + 169);
    pJump = new PegCheckBox(ChildRect, LS(SID_JUMPING));
	/*pChild1->*/Add(pJump);


/*
    ChildRect.Set(iLeft + 153, iTop + 341, iLeft + 242, iTop + 360);
    pTrace = new PegCheckBox(ChildRect, LS(SID_TRACE));
	pChild1->Add(pTrace);
    ChildRect.Set(iLeft + 153, iTop + 318, iLeft + 242, iTop + 337);
    pChild1->Add(new PegCheckBox(ChildRect, LS(SID_LAYOUT), 0, FF_RAISED));
    ChildRect.Set(iLeft + 31, iTop + 339, iLeft + 120, iTop + 358);
    pBallMorph = new PegCheckBox(ChildRect, LS(SID_BALLMORPH), 0, FF_RAISED);
	pChild1->Add(pBallMorph);
	Add(pChild1);

    ChildRect.Set(iLeft + 205, iTop + 141, iLeft + 318, iTop + 160);
    Add(new PegPrompt(ChildRect, LS(SID_MINIOPT), 0, FF_NONE|AF_TRANSPARENT|TJ_CENTER));
    ChildRect.Set(iLeft + 39, iTop + 141, iLeft + 152, iTop + 160);
    Add(new PegPrompt(ChildRect, LS(SID_MINIOPT), 0, FF_NONE|AF_TRANSPARENT|TJ_CENTER));
    ChildRect.Set(iLeft + 179, iTop + 24, iLeft + 182, iTop + 294);
    Add(new PegTextButton(ChildRect, NULL, 0, TJ_CENTER));

    ChildRect.Set(iLeft + 31, iTop + 272, iLeft + 120, iTop + 291);
    p1IterativeDeepening = new PegCheckBox(ChildRect, LS(SID_ITER), 0, 0);
	Add(p1IterativeDeepening);
    ChildRect.Set(iLeft + 31, iTop + 251, iLeft + 144, iTop + 270);
    p1AlphaBeta = new PegCheckBox(ChildRect, LS(SID_ABC), 0, 0);
	Add(p1AlphaBeta);
    ChildRect.Set(iLeft + 305, iTop + 270, iLeft + 328, iTop + 289);
    p1Iter = new PegString(ChildRect, LS(SID_ONE), 0, FF_RECESSED|AF_ENABLED|TT_COPY|EF_EDIT);
	Add(p1Iter);
    ChildRect.Set(iLeft + 198, iTop + 272, iLeft + 287, iTop + 291);
    p2IterativeDeepening = new PegCheckBox(ChildRect, LS(SID_ITER), 0, 0);
	Add(p2IterativeDeepening);
    ChildRect.Set(iLeft + 198, iTop + 251, iLeft + 311, iTop + 270);
    p2AlphaBeta = new PegCheckBox(ChildRect, LS(SID_ABC), 0, 0);
	Add(p2AlphaBeta);
    ChildRect.Set(iLeft + 138, iTop + 270, iLeft + 161, iTop + 289);
    p2Iter = new PegString(ChildRect, LS(SID_ONE), 0, FF_RECESSED|AF_ENABLED|TT_COPY|EF_EDIT);
	Add(p2Iter);

    ChildRect.Set(iLeft + 304, iTop + 162, iLeft + 327, iTop + 181);
    p2SearchDepth = new PegString(ChildRect, LS(SID_THREE), 0, FF_RECESSED|AF_ENABLED|TT_COPY|EF_EDIT);
	Add(p2SearchDepth);
    ChildRect.Set(iLeft + 304, iTop + 183, iLeft + 327, iTop + 202);
	p2Taper1 = new PegString(ChildRect, LS(SID_NINE), 0, FF_RECESSED|AF_ENABLED|TT_COPY|EF_EDIT);
    Add(p2Taper1);
    ChildRect.Set(iLeft + 304, iTop + 204, iLeft + 327, iTop + 223);
    p2Taper2 = new PegString(ChildRect, LS(SID_NINE), 0, FF_RECESSED|AF_ENABLED|TT_COPY|EF_EDIT);
	Add(p2Taper2);
    ChildRect.Set(iLeft + 304, iTop + 225, iLeft + 327, iTop + 244);
    p2Tapern = new PegString(ChildRect, LS(SID_FIVE), 0, FF_RECESSED|AF_ENABLED|TT_COPY|EF_EDIT);
	Add(p2Tapern);
    ChildRect.Set(iLeft + 137, iTop + 162, iLeft + 160, iTop + 181);
    p1SearchDepth = new PegString(ChildRect, LS(SID_THREE), 0, FF_RECESSED|AF_ENABLED|TT_COPY|EF_EDIT);
	Add(p1SearchDepth);
    ChildRect.Set(iLeft + 137, iTop + 183, iLeft + 160, iTop + 202);
    p1Taper1 = new PegString(ChildRect, LS(SID_NINE), 0, FF_RECESSED|AF_ENABLED|TT_COPY|EF_EDIT);
	Add(p1Taper1);
    ChildRect.Set(iLeft + 137, iTop + 204, iLeft + 160, iTop + 223);
    p1Taper2 = new PegString(ChildRect, LS(SID_NINE), 0, FF_RECESSED|AF_ENABLED|TT_COPY|EF_EDIT);
	Add(p1Taper2);
    ChildRect.Set(iLeft + 137, iTop + 225, iLeft + 160, iTop + 244);
    p1Tapern = new PegString(ChildRect, LS(SID_FIVE), 0, FF_RECESSED|AF_ENABLED|TT_COPY|EF_EDIT);
	Add(p1Tapern);

    ChildRect.Set(iLeft + 198, iTop + 228, iLeft + 297, iTop + 247);
    Add(new PegPrompt(ChildRect, LS(SID_TDN)));
    ChildRect.Set(iLeft + 198, iTop + 206, iLeft + 295, iTop + 225);
    Add(new PegPrompt(ChildRect, LS(SID_TD2)));
    ChildRect.Set(iLeft + 198, iTop + 184, iLeft + 297, iTop + 203);
    Add(new PegPrompt(ChildRect, LS(SID_TD1)));
    ChildRect.Set(iLeft + 198, iTop + 162, iLeft + 287, iTop + 181);
    Add(new PegPrompt(ChildRect, LS(SID_SEARCH)));
    ChildRect.Set(iLeft + 31, iTop + 228, iLeft + 130, iTop + 247);
    Add(new PegPrompt(ChildRect, LS(SID_TDN)));
    ChildRect.Set(iLeft + 31, iTop + 206, iLeft + 128, iTop + 225);
    Add(new PegPrompt(ChildRect, LS(SID_TD2)));
    ChildRect.Set(iLeft + 31, iTop + 185, iLeft + 130, iTop + 204);
    Add(new PegPrompt(ChildRect, LS(SID_TD1)));
    ChildRect.Set(iLeft + 31, iTop + 163, iLeft + 120, iTop + 182);
    Add(new PegPrompt(ChildRect, LS(SID_SEARCH)));
*/
    ChildRect.Set(iLeft + 41, iTop + 26, iLeft + 162, iTop + 135);
    pChild1 = new PegGroup(ChildRect, NULL);
    ChildRect.Set(iLeft + 56, iTop + 41, iLeft + 145, iTop + 60);
	p1rbh = new PegRadioButton(ChildRect, LS(SID_HUMAN));
	pChild1->Add(p1rbh);
    ChildRect.Set(iLeft + 56, iTop + 64, iLeft + 145, iTop + 83);
	p1rba1 = new PegRadioButton(ChildRect, LS(SID_BM1));
	pChild1->Add(p1rba1);
    ChildRect.Set(iLeft + 56, iTop + 86, iLeft + 145, iTop + 105);
	p1rba2 = new PegRadioButton(ChildRect, LS(SID_BF2));
	pChild1->Add(p1rba2);
    ChildRect.Set(iLeft + 56, iTop + 110, iLeft + 145, iTop + 129);
	p1rba3 = new PegRadioButton(ChildRect, LS(SID_MINIMAX));
	pChild1->Add(p1rba3);
    Add(pChild1);

    ChildRect.Set(iLeft + 197, iTop + 26, iLeft + 318, iTop + 135);
    pChild1 = new PegGroup(ChildRect, NULL);
    ChildRect.Set(iLeft + 212, iTop + 41, iLeft + 301, iTop + 60);
	p2rbh = new PegRadioButton(ChildRect, LS(SID_HUMAN));
	pChild1->Add(p2rbh);
    ChildRect.Set(iLeft + 212, iTop + 64, iLeft + 301, iTop + 83);
	p2rba1 = new PegRadioButton(ChildRect, LS(SID_BM1));
	pChild1->Add(p2rba1);
    ChildRect.Set(iLeft + 212, iTop + 87, iLeft + 301, iTop + 106);
	p2rba2 = new PegRadioButton(ChildRect, LS(SID_BF2));
	pChild1->Add(p2rba2);
    ChildRect.Set(iLeft + 212, iTop + 110, iLeft + 301, iTop + 129);
	p2rba3 = new PegRadioButton(ChildRect, LS(SID_MINIMAX));
	pChild1->Add(p2rba3);
    Add(pChild1);



	SetupWindow();
}	 	  			   				  	   				  

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
OptionDialog::~OptionDialog()
{}

void OptionDialog::SetupWindow()
{
  char str[10];
  PEGCHAR pstr[10];

  ltoa(NumberOfGames, str, 10);
  AsciiToUnicode(pstr,str);
  pNumGames->DataSet(pstr);

/*  
  ltoa(FullDepth[0]+1, str, 10);
  AsciiToUnicode(pstr,str);
  p1SearchDepth->DataSet(pstr);
  ltoa(FullDepth[1]+1, str, 10);
  AsciiToUnicode(pstr,str);
  p2SearchDepth->DataSet(pstr);

  ltoa(Taper1[0], str, 10);
  AsciiToUnicode(pstr,str);
  p1Taper1->DataSet(pstr);
  ltoa(Taper1[1], str, 10);
  AsciiToUnicode(pstr,str);
  p2Taper1->DataSet(pstr);

  ltoa(Taper2[0], str, 10);
  AsciiToUnicode(pstr,str);
  p1Taper2->DataSet(pstr);
  ltoa(Taper2[1], str, 10);
  AsciiToUnicode(pstr,str);
  p2Taper2->DataSet(pstr);

  ltoa(Taper3[0], str, 10);
  AsciiToUnicode(pstr,str);
  p1Tapern->DataSet(pstr);
  ltoa(Taper3[1], str, 10);
  AsciiToUnicode(pstr,str);
  p2Tapern->DataSet(pstr);

  pBallMorph->SetSelected(BallMorph);

    pTrace->SetSelected(Trace);

    p1AlphaBeta->SetSelected(AlphaBeta[0]);
    p2AlphaBeta->SetSelected(AlphaBeta[1]);
    p1IterativeDeepening->SetSelected(IterativeDeepening[0]);
    p2IterativeDeepening->SetSelected(IterativeDeepening[1]);
*/
    pJump->SetSelected(Jumping);
/*
    p1AlphaBeta->Disable();
  p2AlphaBeta->Disable();
  p1IterativeDeepening->Disable();
  p2IterativeDeepening->Disable();
*/
  p1rbh->SetSelected(FALSE);
  p1rba1->SetSelected(FALSE);
  p1rba2->SetSelected(FALSE);
  p1rba3->SetSelected(FALSE);
  p2rbh->SetSelected(FALSE);
  p2rba1->SetSelected(FALSE);
  p2rba2->SetSelected(FALSE);
  p2rba3->SetSelected(FALSE);

  int mode = Player[0].Mode;

  // player #1
  if ( mode == HUMAN_MODE )
		p1rbh->SetSelected(TRUE);

  if ( mode == AI_MODE_1 )
		p1rba1->SetSelected(TRUE);

  if ( mode == AI_MODE_2 )
		p1rba2->SetSelected(TRUE);

  if ( mode == AI_MODE_3 )
		p1rba3->SetSelected(1);

  // player #2
  mode = Player[1].Mode;
  if ( mode == HUMAN_MODE )
		p2rbh->SetSelected(TRUE);

  if ( mode == AI_MODE_1 )
		p2rba1->SetSelected(TRUE);

  if ( mode == AI_MODE_2 )
		p2rba2->SetSelected(TRUE);

  if ( mode == AI_MODE_3 )
		p2rba3->SetSelected(TRUE);

}

SIGNED OptionDialog::Message(const PegMessage &Mesg)
{
    switch (Mesg.wType)
    {
	case SIGNAL(IDB_OK, PSF_CLICKED):
		if(CanClose())
		{
			return PegDialog::Message(Mesg);
		}
		break;
	case SIGNAL(IDB_HELP, PSF_CLICKED):
		Help();
		break;
    default:
        return(PegDialog::Message(Mesg));
    }
    return 0;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
BOOL OptionDialog::CanClose()
{
  int mode;

  if ( p1rbh->IsSelected())
		mode = HUMAN_MODE;
  else if ( p1rba1->IsSelected())
		mode = AI_MODE_1;
  else if ( p1rba2->IsSelected())
		mode = AI_MODE_2;
  else if ( p1rba3->IsSelected())
		mode = AI_MODE_3;
  else
		mode = HUMAN_MODE;

  if ( pJump->IsChecked())
		Jumping = 1;
  else
		Jumping = 0;
  if(Jumping)
  {
		MOVE_LENGTH = 2;
  }
  else
  {
		MOVE_LENGTH = 1;
  }
/*
  if ( pTrace->IsChecked())
		Trace = 1;
  else
		Trace = 0;

  if ( p1AlphaBeta->IsChecked())
		AlphaBeta[0] = 1;
  else
		AlphaBeta[0] = 0;

  if ( p2AlphaBeta->IsChecked())
		AlphaBeta[1] = 1;
  else
		AlphaBeta[1] = 0;

  if ( p1IterativeDeepening->IsChecked())
		IterativeDeepening[0] = 1;
  else
		IterativeDeepening[0] = 0;

  if ( p2IterativeDeepening->IsChecked())
		IterativeDeepening[1] = 1;
  else
		IterativeDeepening[1] = 0;

  if ( pBallMorph->IsChecked())
		BallMorph = 1;
  else
		BallMorph = 0;
*/
  pGame->SetPlayerMode( PLAYER1, mode );

  Player[0].ticks = 0;

  if ( p2rbh->IsSelected())
		mode = HUMAN_MODE;
  else if ( p2rba1->IsSelected())
		mode = AI_MODE_1;
  else if ( p2rba2->IsSelected())
		mode = AI_MODE_2;
  else if ( p2rba3->IsSelected())
		mode = AI_MODE_3;
  else
		mode = HUMAN_MODE;

  pGame->SetPlayerMode( PLAYER2, mode );

  Player[1].ticks = 0;
/*  
  int TmpTaper = atoi(p1Taper1->DataGet());

  if (TmpTaper < 0 )
  {
//	 MessageBox("Error", "P1: Taper 1 Size must be greater than zero");
	 PegMessageWindow *pWin = new PegMessageWindow(LS(SID_ERROR), 
			LS(SID_ERROR5), FF_RAISED|MW_OK);
	 Presentation()->Center(pWin);
	 pWin->Execute();
	 return FALSE;
  }

  Taper1[0] = TmpTaper;

  TmpTaper = atoi(p2Taper1->DataGet());
  if (TmpTaper < 0 )
  {
//	 MessageBox("P2: Taper 1 Size must be greater than zero");
	 PegMessageWindow *pWin = new PegMessageWindow(LS(SID_ERROR), 
			LS(SID_ERROR6), FF_RAISED|MW_OK);
	 Presentation()->Center(pWin);
	 pWin->Execute();
	 return FALSE;
  }
  Taper1[1] = TmpTaper;

  TmpTaper = atoi(p1Taper2->DataGet());
  if (TmpTaper < 0 )
  {
//	 MessageBox("P1: Taper 2 Size must be greater than zero");
	 PegMessageWindow *pWin = new PegMessageWindow(LS(SID_ERROR), 
			LS(SID_ERROR7), FF_RAISED|MW_OK);
	 Presentation()->Center(pWin);
	 pWin->Execute();
	 return FALSE;
  }
  Taper2[0] = TmpTaper;

  TmpTaper = atoi(p2Taper2->DataGet());
  if (TmpTaper < 0 )
  {
//	 MessageBox("P2: Taper 2 Size must be greater than zero");
	 PegMessageWindow *pWin = new PegMessageWindow(LS(SID_ERROR), 
			LS(SID_ERROR8), FF_RAISED|MW_OK);
	 Presentation()->Center(pWin);
	 pWin->Execute();
	 return FALSE;
  }
  Taper2[1] = TmpTaper;
  TmpTaper = atoi(p1Tapern->DataGet());

  if (TmpTaper < 0 )
  {
//	 MessageBox("P1: Taper N Size must be greater than zero");
	 PegMessageWindow *pWin = new PegMessageWindow(LS(SID_ERROR), 
			LS(SID_ERROR9), FF_RAISED|MW_OK);
	 Presentation()->Center(pWin);
	 pWin->Execute();
	 return FALSE;
  }

  Taper3[0] = TmpTaper;

  TmpTaper = atoi(p2Tapern->DataGet());
  if (TmpTaper < 0 )
  {
//	 MessageBox("P2: Taper N Size must be greater than zero");
	 PegMessageWindow *pWin = new PegMessageWindow(LS(SID_ERROR), 
			LS(SID_ERROR10), FF_RAISED|MW_OK);
	 Presentation()->Center(pWin);
	 pWin->Execute();
	 return FALSE;
  }
  Taper3[1] = TmpTaper;

  int TmpDepth = atoi(p1SearchDepth->DataGet());

  if (TmpDepth < 2 || TmpDepth > MAX_DEPTH || !(TmpDepth % 2) )
  {
//	 MessageBox("P1: Search Depth must be odd, or is too large");
	 PegMessageWindow *pWin = new PegMessageWindow(LS(SID_ERROR), 
			LS(SID_ERROR11), FF_RAISED|MW_OK);
	 Presentation()->Center(pWin);
	 pWin->Execute();
	 return FALSE;
  }
  FullDepth[0] = TmpDepth - 1;
  TmpDepth = atoi(p2SearchDepth->DataGet());

  if (TmpDepth < 2 || TmpDepth > MAX_DEPTH || !(TmpDepth % 2) )
  {
//	 MessageBox("P2: Search Depth must be odd, or is too large");
	 PegMessageWindow *pWin = new PegMessageWindow(LS(SID_ERROR), 
			LS(SID_ERROR12), FF_RAISED|MW_OK);
	 Presentation()->Center(pWin);
	 pWin->Execute();
	 return FALSE;
  }

  FullDepth[1] = TmpDepth - 1;
*/
  int times = atoi(pNumGames->DataGet());

  if (times > 0 && times <= 999 ) {
	 NumberOfGames = times;
	 CurrentGame = 0;
	 return TRUE;
  } else {
//	 MessageBox("Number of Games must be between 1 and 999");
	 PegMessageWindow *pWin = new PegMessageWindow(LS(SID_ERROR), 
			LS(SID_ERROR13), FF_RAISED|MW_OK);
	 Presentation()->Center(pWin);
	 pWin->Execute();
    return FALSE;
  }


}

void OptionDialog::Help()
{
	PegRect Rect;
	Rect.Set(95,56,539,363);
	BolasHelpScreen *bhs = new BolasHelpScreen(Rect);

	bhs->Execute();
}



/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
GameWindow::GameWindow(PegRect Rect) : PegDecoratedWindow(Rect)
{
  srand(time(NULL));
  
  Screen()->SetPalette(0, 256, PegCustomPalette);

  IsGameOver = TRUE;
  IsPause = FALSE;
  IsFirstTime = TRUE;
  CurrentGame = 0;
  StartPlayer = PLAYER1;
  Taper1[0] = 99;
  Taper1[1] = 99;
  Taper2[0] = 99;
  Taper2[1] = 99;
  Taper3[0] = 5;
  Taper3[1] = 5;
  FullDepth[0] = SEARCH_DEPTH;
  FullDepth[1] = SEARCH_DEPTH;
  PartialDepth = 0;
  MaxDepth = SEARCH_DEPTH;
  AlphaBeta[0] = 0;
  AlphaBeta[1] = 0;
  IterativeDeepening[0] = 0;
  IterativeDeepening[1] = 0;
  BallMorph = 0;
  Trace = 0;
  Jumping = 0;
  MOVE_LENGTH = 1;
  Player[0].ticks = 0;
  Player[1].ticks = 0;
  GameOver = NULL;

  IsBusy = FALSE;
  SetupWindow(Rect);
  InitBoard( VisibleBoard );
  SetPlayerMode( PLAYER1,HUMAN_MODE );
  SetPlayerMode( PLAYER2,AI_MODE_3 );
//  SetCursor(GetModule(), "Wand");
//  SetPointerType(Wand);
  Id(BOLAS_ID);

}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void GameWindow::SetupWindow(PegRect Rect)
{
  PegRect ChildRect;
  //CursorDown = new TCursor(inst, "WandDown");
  //CursorUp = new TCursor(inst, "Wand");

  Player1Animate[0] = &gbP1blankBitmap;
  Player1Animate[1] = &gbPlayer1MicroBitmap;
  Player1Animate[2] = &gbPlayer1LitBitmap;
  Player1Animate[3] = &gbPlayer1MediumBitmap;
  Player1Animate[4] = &gbPlayer1Bitmap;
  Player2Animate[0] = &gbPlayer2BlankBitmap;
  Player2Animate[1] = &gbPlayer2MicroBitmap;
  Player2Animate[2] = &gbPlayer2LitBitmap;
  Player2Animate[3] = &gbPlayer2MediumBitmap;
  Player2Animate[4] = &gbPlayer2Bitmap;

  ChildRect.Set(SCOREBOARD_X, SCOREBOARD_Y, SCOREBOARD_X + SCOREBOARD_WIDTH, SCOREBOARD_Y + SCOREBOARD_HEIGHT);
  ScoreBoard = new PegIcon(ChildRect, &gbBoardBitmap);

  int iLeft = SCOREBOARD_X + TURN1_OFFSET_X;
  int iTop = SCOREBOARD_Y + TURN_OFFSET_Y;
  ChildRect.Set(iLeft, iTop, iLeft + TURN_SIZE, iTop + TURN_SIZE);
  Player1Arrow = new PegBitmapLight(ChildRect, 2);
  Player1Arrow->SetStateBitmap(PLAYER1, &gbPlayer1tBitmap);
  Player1Arrow->SetStateBitmap(PLAYER2, &gbBlankTurnBitmap);
  Add(Player1Arrow);
  iLeft = SCOREBOARD_X + TURN2_OFFSET_X;
  ChildRect.Set(iLeft, iTop, iLeft + TURN_SIZE, iTop + TURN_SIZE);
  Player2Arrow = new PegBitmapLight(ChildRect, 2);
  Player2Arrow->SetStateBitmap(PLAYER2, &gbPlayer2tBitmap);
  Player2Arrow->SetStateBitmap(PLAYER1, &gbBlankTurnBitmap);
  Add(Player2Arrow);

  iLeft = SCOREBOARD_X + PLAYER1_SMALL_OFFSET_X;
  iTop = SCOREBOARD_Y + PLAYER1_SMALL_OFFSET_Y;
  ChildRect.Set(iLeft, iTop, iLeft + SMALL_SIZE, iTop + SMALL_SIZE);
  Player1Ball = new PegBitmapLight(ChildRect, 2);
  Player1Ball->SetStateBitmap(PLAYER1, &gbPlayer1VoltsBitmap);
  Player1Ball->SetStateBitmap(PLAYER2, &gbPlayer1sBitmap);
  Add(Player1Ball);
  iLeft = SCOREBOARD_X + PLAYER2_SMALL_OFFSET_X;
  ChildRect.Set(iLeft, iTop, iLeft + SMALL_SIZE, iTop + SMALL_SIZE);
  Player2Ball = new PegBitmapLight(ChildRect, 2);
  Player2Ball->SetStateBitmap(PLAYER2, &gbPlayer2VoltsBitmap);
  Player2Ball->SetStateBitmap(PLAYER1, &gbPlayer2sBitmap);
  Add(Player2Ball);

	iLeft = Rect.wLeft;
	iTop = Rect.wTop;

    PegMenuDescription GameaMenu [] = {
    {LS(SID_ABOUT),  IDM_ABOUT,  AF_ENABLED, NULL},
    {NULL,  0,  AF_ENABLED|BF_SEPERATOR, NULL},
    {LS(SID_OPTIONS),  IDM_OPTION,  AF_ENABLED, NULL},
    {LS(SID_PAUSE),  IDM_PAUSE,  AF_ENABLED, NULL},
    {LS(SID_STOP),  IDM_STOP, 0 , NULL},
    {LS(SID_NEW),  IDM_NEW,  AF_ENABLED, NULL},
    {NULL, 0, 0, NULL}
    };
    PegMenuDescription TopMenu [] = {
    {LS(SID_GAME),  0,  AF_ENABLED, GameaMenu},
    {NULL, 0, 0, NULL}
    };

	PegThing *pChild1;

    mReal.Set(iLeft, iTop, iLeft + 595, iTop + 442);
    InitClient();
    SetColor(PCI_NORMAL, BLACK);
    Add (new PegTitle(LS(SID_BOLAS), TF_SYSBUTTON|TF_MINMAXBUTTON|TF_CLOSEBUTTON));
    Add(new PegMenuBar(TopMenu));

    ChildRect.Set(iLeft + 343, iTop + 406, iLeft + 357, iTop + 425);
    pChild1 = new PegPrompt(ChildRect, LS(SID_G), 0, FF_NONE|AF_ENABLED|AF_TRANSPARENT|TJ_LEFT);
    ((PegPrompt *) pChild1)->SetColor(PCI_NTEXT, WHITE);
    Add(pChild1);

    ChildRect.Set(iLeft + 289, iTop + 407, iLeft + 303, iTop + 426);
    pChild1 = new PegPrompt(ChildRect, LS(SID_F), 0, FF_NONE|AF_ENABLED|AF_TRANSPARENT|TJ_LEFT);
    ((PegPrompt *) pChild1)->SetColor(PCI_NTEXT, WHITE);
    Add(pChild1);

    ChildRect.Set(iLeft + 242, iTop + 407, iLeft + 256, iTop + 426);
    pChild1 = new PegPrompt(ChildRect, LS(SID_E), 0, FF_NONE|AF_ENABLED|AF_TRANSPARENT|TJ_LEFT);
    ((PegPrompt *) pChild1)->SetColor(PCI_NTEXT, WHITE);
    Add(pChild1);

    ChildRect.Set(iLeft + 192, iTop + 408, iLeft + 206, iTop + 427);
    pChild1 = new PegPrompt(ChildRect, LS(SID_D), 0, FF_NONE|AF_ENABLED|AF_TRANSPARENT|TJ_LEFT);
    ((PegPrompt *) pChild1)->SetColor(PCI_NTEXT, WHITE);
    Add(pChild1);

    ChildRect.Set(iLeft + 141, iTop + 407, iLeft + 155, iTop + 426);
    pChild1 = new PegPrompt(ChildRect, LS(SID_C), 0, FF_NONE|AF_ENABLED|AF_TRANSPARENT|TJ_LEFT);
    ((PegPrompt *) pChild1)->SetColor(PCI_NTEXT, WHITE);
    Add(pChild1);

    ChildRect.Set(iLeft + 90, iTop + 406, iLeft + 104, iTop + 425);
    pChild1 = new PegPrompt(ChildRect, LS(SID_B), 0, FF_NONE|AF_ENABLED|AF_TRANSPARENT|TJ_LEFT);
    ((PegPrompt *) pChild1)->SetColor(PCI_NTEXT, WHITE);
    Add(pChild1);

    ChildRect.Set(iLeft + 46, iTop + 405, iLeft + 60, iTop + 424);
    pChild1 = new PegPrompt(ChildRect, LS(SID_A), 0, FF_NONE|AF_ENABLED|AF_TRANSPARENT|TJ_LEFT);
    ((PegPrompt *) pChild1)->SetColor(PCI_NTEXT, WHITE);
    Add(pChild1);

    ChildRect.Set(iLeft + 13, iTop + 366, iLeft + 27, iTop + 385);
    pChild1 = new PegPrompt(ChildRect, LS(SID_SEVEN), 0, FF_NONE|AF_ENABLED|AF_TRANSPARENT|TJ_LEFT);
    ((PegPrompt *) pChild1)->SetColor(PCI_NTEXT, WHITE);
    Add(pChild1);

    ChildRect.Set(iLeft + 13, iTop + 318, iLeft + 27, iTop + 337);
    pChild1 = new PegPrompt(ChildRect, LS(SID_SIX), 0, FF_NONE|AF_ENABLED|AF_TRANSPARENT|TJ_LEFT);
    ((PegPrompt *) pChild1)->SetColor(PCI_NTEXT, WHITE);
    Add(pChild1);

    ChildRect.Set(iLeft + 14, iTop + 269, iLeft + 28, iTop + 288);
    pChild1 = new PegPrompt(ChildRect, LS(SID_FIVE), 0, FF_NONE|AF_ENABLED|AF_TRANSPARENT|TJ_LEFT);
    ((PegPrompt *) pChild1)->SetColor(PCI_NTEXT, WHITE);
    Add(pChild1);

    ChildRect.Set(iLeft + 14, iTop + 217, iLeft + 28, iTop + 236);
    pChild1 = new PegPrompt(ChildRect, LS(SID_FOUR), 0, FF_NONE|AF_ENABLED|AF_TRANSPARENT|TJ_LEFT);
    ((PegPrompt *) pChild1)->SetColor(PCI_NTEXT, WHITE);
    Add(pChild1);

    ChildRect.Set(iLeft + 14, iTop + 167, iLeft + 28, iTop + 186);
    pChild1 = new PegPrompt(ChildRect, LS(SID_THREE), 0, FF_NONE|AF_ENABLED|AF_TRANSPARENT|TJ_LEFT);
    ((PegPrompt *) pChild1)->SetColor(PCI_NTEXT, WHITE);
    Add(pChild1);

    ChildRect.Set(iLeft + 14, iTop + 119, iLeft + 28, iTop + 138);
    pChild1 = new PegPrompt(ChildRect, LS(SID_TWO), 0, FF_NONE|AF_ENABLED|AF_TRANSPARENT|TJ_LEFT);
    ((PegPrompt *) pChild1)->SetColor(PCI_NTEXT, WHITE);
    Add(pChild1);

    ChildRect.Set(iLeft + 15, iTop + 71, iLeft + 29, iTop + 90);
    pChild1 = new PegPrompt(ChildRect, LS(SID_ONE), 0, FF_NONE|AF_ENABLED|AF_TRANSPARENT|TJ_LEFT);
    ((PegPrompt *) pChild1)->SetColor(PCI_NTEXT, WHITE);
    Add(pChild1);

    ChildRect.Set(SCOREBOARD_X, SCOREBOARD_Y, SCOREBOARD_X + SCOREBOARD_WIDTH, SCOREBOARD_Y + SCOREBOARD_HEIGHT);
    Add(new PegIcon(ChildRect, &gbBoardBitmap));

	int x,y,w=GRID_SIZE,h=GRID_SIZE;
	for(int j = NUM_SQUARES_X-1; j >=0; j--)
	{
		x = BOARD_MIN_X + (GRID_SIZE * j);
		for(int i = NUM_SQUARES_Y-1; i >= 0; i--)
		{
			y = BOARD_MIN_Y + (GRID_SIZE * i);
			ChildRect.Set(x,y,w,h);
			GameBoard[i][j] = new BolasSquare(this, ChildRect, 10,i,j);
			GameBoard[i][j]->SetStateBitmap(PLAYER1, &gbPlayer1Bitmap);
			GameBoard[i][j]->SetStateBitmap(PLAYER2, &gbPlayer2Bitmap);
			GameBoard[i][j]->SetStateBitmap(EMPTY, &gbP1blankBitmap);
			GameBoard[i][j]->SetStateBitmap(MICRO1, &gbPlayer1MicroBitmap);
			GameBoard[i][j]->SetStateBitmap(LITTLE1, &gbPlayer1LitBitmap);
			GameBoard[i][j]->SetStateBitmap(MEDIUM1, &gbPlayer1MediumBitmap);
			GameBoard[i][j]->SetStateBitmap(MICRO2, &gbPlayer2MicroBitmap);
			GameBoard[i][j]->SetStateBitmap(LITTLE2, &gbPlayer2LitBitmap);
			GameBoard[i][j]->SetStateBitmap(MEDIUM2, &gbPlayer2MediumBitmap);
			Add(GameBoard[i][j]);
		}
	}

	CreateAnimationTimer();

	NewGame();


}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void GameWindow::SetPlayerMode( int p, int mode )
{
	Player[p].Mode = mode;
	Player[p].Wins = 0;
	Player[p].Losses = 0;

	ResetPlayerCount( p );
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void GameWindow::ResetPlayerCount( int p )
{
	Player[p].Count = 2;
	Player[p].SelRow = (p == PLAYER1) ? 0 : NUM_SQUARES_Y - 1;
	Player[p].SelCol = (p == PLAYER1) ? 0 : NUM_SQUARES_X - 1;
	Player[p].Selected = 0;
	Player[p].ticks = 0;

	Animating = TRUE;
	AniGrid[Player[p].SelRow][Player[p].SelCol] = ANI_UP(p);
	SetBoardValue( VisibleBoard,Player[p].SelRow,Player[p].SelCol,p );

	if ( p == PLAYER1 )
	{
		AniGrid[NUM_SQUARES_Y - 1][0] = ANI_UP(p);
		SetBoardValue( VisibleBoard, NUM_SQUARES_Y-1, 0, p );
	}
	else
	{
		AniGrid[0][NUM_SQUARES_X - 1] = ANI_UP(p);
		SetBoardValue( VisibleBoard, 0, NUM_SQUARES_X-1, p );
	}
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void GameWindow::CreateTimer()
{
  SetTimer(TimerId, ONE_SECOND / 16, ONE_SECOND / 16);
  bTimerActive = 1;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void GameWindow::CreateAnimationTimer()
{
    SetTimer(AnimateId, ONE_SECOND / 16, ONE_SECOND / 16);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void GameWindow::CreateFinishTimer()
{
    SetTimer(FinishId, ONE_SECOND / 16, ONE_SECOND / 16);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void GameWindow::StopTimer()
{
    if (bTimerActive)
    {
        KillTimer(TimerId);
		bTimerActive = 0;
    }
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void GameWindow::StopAnimationTimer()
{
	KillTimer(AnimateId);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void GameWindow::StopFinishTimer()
{
	KillTimer(FinishId);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void GameWindow::ClaimSquare( unsigned Brd[NUM_SQUARES_Y],int p, int row, int col )
{
	int val = GetBoardValue( Brd, row, col );

	if ( val == EMPTY || val == -1 )
		return;

	if ( val != p )
	{
		if(&Brd[0] == &VisibleBoard[0])
		{
			AniGrid[row][col] = ANI_WAIT(p);
			Animating = true;
		}
		SetBoardValue( Brd, row, col, p );
		return;
	}

    // must be one of mine
	return;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void GameWindow::CaptureBalls( unsigned Brd[NUM_SQUARES_Y], int p, int row, int col )
{
	int	trow = row - 1;
	if ( trow >= 0 )
	{
		ClaimSquare( Brd, p,trow,col-1 );
		ClaimSquare( Brd, p,trow,col );
		ClaimSquare( Brd, p,trow,col+1 );
	}
	ClaimSquare( Brd, p,row,col-1 );
//	ClaimSquare( Brd, p,row,col );
	ClaimSquare( Brd, p,row,col+1 );
	trow = row + 1;
	if ( trow < NUM_SQUARES_Y )
	{
		ClaimSquare( Brd, p,trow,col-1 );
		ClaimSquare( Brd, p,trow,col );
		ClaimSquare( Brd, p,trow,col+1 );
	}
	return;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void GameWindow::DrawGameOver()
{
	int iLeft = mClient.wLeft - 5;
	int iTop = mClient.wTop - 44;
	PegRect ChildRect;
	ChildRect.Set(iLeft + SCOREBOARD_X, iTop + BOARD_MIN_Y, 
				iLeft + SCOREBOARD_X + 200, iTop + BOARD_MIN_Y + 200);
	GameOver = new PegIcon(ChildRect, &gbGameOverBitmap);
    Add(GameOver);
	Invalidate();

}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void GameWindow::DrawOwner(PegPoint &Put, int Owner)
{

}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void GameWindow::DrawScoreBoard()
{
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void GameWindow::DrawBoard()
{
}



/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
extern PegFont Arial10;
void GameWindow::WriteScore()
{
	int iLeft = mClient.wLeft - 5;
	int iTop = mClient.wTop - 44;
	PegRect Rect;
	Rect.Set(iLeft + SCOREBOARD_X, iTop + SCOREBOARD_Y,
				iLeft + SCOREBOARD_X + SCOREBOARD_WIDTH,
				iTop + SCOREBOARD_Y + SCOREBOARD_HEIGHT);
	Invalidate(Rect);


	// Player 1
	char s[ 32 ];
	PEGCHAR ps[32];
	PegPoint pp;
	PegColor Color(0x24, 0x21, CF_FILL);

	if ( IsGameOver && !IsFirstTime )
	{
		if ( CurrentGame < NumberOfGames )
		{
			CurrentGame++;
			if ( Player[0].Count > Player[1].Count )
			{
				Player[0].Wins++;
				Player[1].Losses++;
			}
			if ( Player[0].Count < Player[1].Count )
			{
				Player[1].Wins++;
				Player[0].Losses++;
			}
		}
      
		if ( CurrentGame < NumberOfGames )
		{
			IsGameOver = 0;

			NewGame();
		}
		else
		{
//			StopTimer();
		}

	}
	if ( IsFirstTime )
      	DrawGameOver( );


	sprintf(s, "%s", ModeDesc[Player[0].Mode] );
	AsciiToUnicode(ps,s);
	pp.x = iLeft + SCOREBOARD_X + PLAYER1_MODE_OFFSET_X;
	pp.y = iTop + SCOREBOARD_Y + PLAYER_MODE_OFFSET_Y;
	BeginDraw();
	Screen()->DrawText(this, pp, LS(SID_SPACE17), Color, &Arial10);
	Screen()->DrawText(this, pp, ps, Color, &Arial10);

	sprintf(s, "%s", ModeDesc[Player[1].Mode] );
	AsciiToUnicode(ps,s);
	pp.x = iLeft + SCOREBOARD_X + PLAYER2_MODE_OFFSET_X;
	pp.y = iTop + SCOREBOARD_Y + PLAYER_MODE_OFFSET_Y;
	Screen()->DrawText(this, pp, LS(SID_SPACE17), Color, &Arial10);
	Screen()->DrawText(this, pp, ps, Color, &Arial10);

	sprintf(s, "%02d", Player[0].Count);
	AsciiToUnicode(ps,s);
	pp.x = iLeft + SCOREBOARD_X + PLAYER1_COUNT_OFFSET_X;
	pp.y = iTop + SCOREBOARD_Y + PLAYER1_COUNT_OFFSET_Y;
	Screen()->DrawText(this, pp, ps, Color, &Arial10);

	sprintf(s, "%02d", Player[1].Count);
	AsciiToUnicode(ps,s);
	pp.x = iLeft + SCOREBOARD_X + PLAYER2_COUNT_OFFSET_X;
	pp.y = iTop + SCOREBOARD_Y + PLAYER2_COUNT_OFFSET_Y;
	Screen()->DrawText(this, pp, ps, Color, &Arial10);

	sprintf(s, "%03d", Player[0].Wins);
	AsciiToUnicode(ps,s);
	pp.x = iLeft + SCOREBOARD_X + PLAYER1_WIN_OFFSET_X;
	pp.y = iTop + SCOREBOARD_Y + PLAYER_WIN_OFFSET_Y;
	Screen()->DrawText(this, pp, LS(SID_SPACE6), Color, &Arial10);
	Screen()->DrawText(this, pp, ps, Color, &Arial10);

	sprintf(s, "%03d", Player[1].Wins);
	AsciiToUnicode(ps,s);
	pp.x = iLeft + SCOREBOARD_X + PLAYER2_WIN_OFFSET_X;
	pp.y = iTop + SCOREBOARD_Y + PLAYER_WIN_OFFSET_Y;
	Screen()->DrawText(this, pp, LS(SID_SPACE6), Color, &Arial10);
	Screen()->DrawText(this, pp, ps, Color, &Arial10);

	sprintf(s, "%03d", Player[0].Losses);
	AsciiToUnicode(ps,s);
	pp.x = iLeft + SCOREBOARD_X + PLAYER1_LOSS_OFFSET_X;
	pp.y = iTop + SCOREBOARD_Y + PLAYER_LOSS_OFFSET_Y;
	Screen()->DrawText(this, pp, LS(SID_SPACE6), Color, &Arial10);
	Screen()->DrawText(this, pp, ps, Color, &Arial10);

	sprintf(s, "%03d", Player[1].Losses);
	AsciiToUnicode(ps,s);
	pp.x = iLeft + SCOREBOARD_X + PLAYER2_LOSS_OFFSET_X;
	pp.y = iTop + SCOREBOARD_Y + PLAYER_LOSS_OFFSET_Y;
	Screen()->DrawText(this, pp, LS(SID_SPACE6), Color, &Arial10);
	Screen()->DrawText(this, pp, ps, Color, &Arial10);

	int TmpGame = (CurrentGame < NumberOfGames) ? CurrentGame+1 : CurrentGame;		
	sprintf(s, "%03d", TmpGame);
	AsciiToUnicode(ps,s);
	pp.x = iLeft + SCOREBOARD_X + CUR_GAME_OFFSET_X;
	pp.y = iTop + SCOREBOARD_Y + CUR_GAME_OFFSET_Y;
	Screen()->DrawText(this, pp, LS(SID_SPACE6), Color, &Arial10);
	Screen()->DrawText(this, pp, ps, Color, &Arial10);

	sprintf(s, "%03d", NumberOfGames);
	AsciiToUnicode(ps,s);
	pp.x = iLeft + SCOREBOARD_X + TOT_GAME_OFFSET_X;
	pp.y = iTop + SCOREBOARD_Y + TOT_GAME_OFFSET_Y;
	Screen()->DrawText(this, pp, LS(SID_SPACE6), Color, &Arial10);
	Screen()->DrawText(this, pp, ps, Color, &Arial10);

	int val;		
	val = Player[0].ticks;
	if ( val > 999 )
	{
		val /= 60;
		sprintf( s, "%02d m",val );
	}
	else
	{
		sprintf(s, "%03d", val );
	}
	AsciiToUnicode(ps,s);
	pp.x = iLeft + SCOREBOARD_X + PLAYER1_TIME_OFFSET_X;
	pp.y = iTop + SCOREBOARD_Y + PLAYER_TIME_OFFSET_Y;
	Screen()->DrawText(this, pp, LS(SID_SPACE6), Color, &Arial10);
	Screen()->DrawText(this, pp, ps, Color, &Arial10);

	val = Player[1].ticks;
	if ( val > 999 )
	{
		val /= 60;
		sprintf( s, "%02d m",val );
	}
	else
	{
		sprintf(s, "%03d", val );
	}
	AsciiToUnicode(ps,s);
	pp.x = iLeft + SCOREBOARD_X + PLAYER2_TIME_OFFSET_X;
	pp.y = iTop + SCOREBOARD_Y + PLAYER_TIME_OFFSET_Y;
	Screen()->DrawText(this, pp, LS(SID_SPACE6), Color, &Arial10);
	Screen()->DrawText(this, pp, ps, Color, &Arial10);

	EndDraw();
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void GameWindow::CmAbout()
{
/*
	PegPrompt *pp1, *pp2;
	PegRect Rect;
	PEGCHAR ps1[32],ps2[32];
	char *s1 = "Bolas v1.0";
	char *s2 = "PEG v1.80\nCopyright 2002 Swell Software, Inc";
	AsciiToUnicode(ps1,s1);
	AsciiToUnicode(ps2,s2);
//	PegMessageWindow *pWin = new PegMessageWindow(LS(SID_ABOUT), LS(SID_BOLAS), FF_RAISED|MW_OK);
	Rect.Set(20,10,40,40);
	pp1 = new PegPrompt(Rect, ps1);
	Rect.Set(20,20,40,60);
	pp2 = new PegPrompt(Rect, ps2);
	pWin->Add(pp1);
	pWin->Add(pp2);
//	pWin->Execute();
*/
	PegRect Rect;
	Rect.Set(100,100, 450,270);
	BolasAboutScreen *bas = new BolasAboutScreen(Rect);

	bas->Execute();
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void GameWindow::CmOptions()
{
  PegRect Rect;
//  Rect.Set(10,10,369,454);
  Rect.Set(100,100,459, 340);

  OptionDialog *od = new OptionDialog(Rect, LS(SID_GAMEOPT));
  od->pGame = this;

  int i = od->Execute();
  if (i == IDB_OK)
  {
	CurrentGame = 0;
	CmResetGame();
  }
} 


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void GameWindow::CmPause()
{
  if (IsGameOver)
	 return;

  PegMenuButton *pmb = (PegMenuButton*)Find(IDM_PAUSE);
  if (IsPause) {
	 IsPause = FALSE;
 	 pmb->DataSet(LS(SID_PAUSE));

  } else {
	 IsPause = TRUE;
	 StopTimer();
	 pmb->DataSet(LS(SID_CONTINUE));
  }
  Presentation()->MoveFocusTree(GameBoard[FOCUSY][FOCUSX]);
}



/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void GameWindow::ClearBoard( unsigned Brd[NUM_SQUARES_Y] )
{
	for ( int i = 0; i < NUM_SQUARES_X; i++ )
		for ( int j = 0; j < NUM_SQUARES_Y; j++ )
		{
			AniGrid[j][i] = ANI_NONE;
			SetBoardValue( Brd,j,i, EMPTY );
			GameBoard[j][i]->SetState(EMPTY);
			GameBoard[j][i]->Selected = 0;
		}
	FOCUSX = FOCUSY = 0;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void GameWindow::InitBoard( unsigned Brd[NUM_SQUARES_Y] )
{
	for ( int j = 0; j <= NUM_SQUARES_Y; j++ )
		Brd[ j ] = 0xFFFF;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void GameWindow::CmResetGame()
{
  StopTimer();
  StopFinishTimer();
  IsFirstTime = FALSE;
  ClearBoard( VisibleBoard );
  if(GameOver)
  {
	Destroy(GameOver);
	GameOver = NULL;
  }
  ResetPlayerCount( PLAYER1 );
  ResetPlayerCount( PLAYER2 );
  Player[0].ticks = 0;
  Player[1].ticks = 0;
  PegMenuButton *pmb = (PegMenuButton*)Find(IDM_PAUSE);
  pmb->DataSet(LS(SID_PAUSE));
  ActivePlayer = StartPlayer;
  StartPlayer = 1 - StartPlayer;
  IsBusy = 0;
  if ( CurrentGame == NumberOfGames )
		CurrentGame = 0;

  if ( Player[ActivePlayer].Mode != HUMAN_MODE )
		CreateTimer();


  Invalidate(Presentation()->mReal);
  Presentation()->MoveFocusTree(GameBoard[0][0]);

  IsGameOver = FALSE;
  if (IsPause) {
	 IsPause = FALSE;
	 pmb->DataSet(LS(SID_PAUSE));
  }
	starttime = time(NULL);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void GameWindow::NewGame()
{
  StopTimer();
  StopFinishTimer();
  IsFirstTime = FALSE;
  ClearBoard( VisibleBoard );
  if(GameOver)
  {
	Destroy(GameOver);
	GameOver = NULL;
  }
  ResetPlayerCount( PLAYER1 );
  ResetPlayerCount( PLAYER2 );
  PegMenuButton *pmb = (PegMenuButton*)Find(IDM_PAUSE);
  pmb->DataSet(LS(SID_PAUSE));
  ActivePlayer = StartPlayer;
  StartPlayer = 1 - StartPlayer;
  IsBusy = 0;
  if ( CurrentGame == NumberOfGames )
		CurrentGame = 0;

  if ( Player[ActivePlayer].Mode != HUMAN_MODE )
		CreateTimer();


  Invalidate(Presentation()->mReal);
  WriteScore();
  Presentation()->MoveFocusTree(GameBoard[0][0]);

  IsGameOver = FALSE;
  if (IsPause) {
	 IsPause = FALSE;
	 pmb->DataSet(LS(SID_PAUSE));
  }
  starttime = time(NULL);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void GameWindow::CmStopGame()
{
  CurrentGame = NumberOfGames;
  StopTimer();
  StopAnimationTimer();
  StopFinishTimer();
  PegMenuButton *pmb = (PegMenuButton*)Find(IDM_PAUSE);
  pmb->DataSet(LS(SID_PAUSE));
  IsPause = FALSE;
  IsGameOver = TRUE;
  DrawGameOver();
  Invalidate(Presentation()->mReal);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void GameWindow::EvDestroy()
{
  delete Player1;
  delete Player2;
  delete Player1Small;
  delete Player2Small;
  delete Player1Turn;
  delete Player2Turn;
  delete Empty;
  delete GameOver;
  delete ScoreBoard;
  delete BlankTurn;
//  delete CursorDown;
//  delete CursorUp;
  delete Player1Volts;
  delete Player2Volts;

  for ( int i = 0; i < 5; i++ )
  {
		delete Player1Animate[i];
		delete Player2Animate[i];
  }
  StopTimer();
  StopFinishTimer();
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void GameWindow::EvLButtonDown(UINT, PegPoint clickPoint)
{
  int row, column, iLeft, iTop;
  iLeft = mClient.wLeft - 5;
  iTop = mClient.wTop - 44;
  column = (clickPoint.x - (BOARD_MIN_X + iLeft)) / GRID_SIZE;
  row = (clickPoint.y - (BOARD_MIN_Y + iTop)) / GRID_SIZE;
  FOCUSX = column;
  FOCUSY = row;
  if ( Player[ActivePlayer].Mode != HUMAN_MODE )
  {
	  return;
  }

  if (IsGameOver || IsPause || IsBusy || Animating)
  {
//    MessageBeep( -1 );
	 return;
  }

  if ( (clickPoint.x > BOARD_MAX_X + iLeft) || (clickPoint.x < BOARD_MIN_X + iLeft) ||
		 (clickPoint.y > BOARD_MAX_Y + iTop) || (clickPoint.y < BOARD_MIN_Y + iTop) )
  {
//		MessageBeep( -1 );
		return;
  }

  PegPoint point;
  point = Screen()->GetPointerPos();
  Screen()->SetPointer(point);
//  SetCursor(GetModule(), "WandDown");


  int val = GetBoardValue( VisibleBoard, row, column );
  // this is owned by opponent
  if ( val != EMPTY && val != ActivePlayer )
  {
//		MessageBeep( -1 );
		return;
  }

  // Select a new anchor point
  if ( val == ActivePlayer )
  {
		if(Player[ActivePlayer].Selected)
		{
			GameBoard[Player[ActivePlayer].SelRow][Player[ActivePlayer].SelCol]->Selected = 0;
			GameBoard[Player[ActivePlayer].SelRow][Player[ActivePlayer].SelCol]->EraseFocusIndicator();
		}
      Player[ActivePlayer].SelRow = row;
		Player[ActivePlayer].SelCol = column;
		Player[ActivePlayer].Selected = 1;
		GameBoard[row][column]->Selected = 1;
		GameBoard[row][column]->DrawSelection();
		return;
  }

  if ( !Player[ActivePlayer].Selected )
  {
//		MessageBeep( -1 );
		return;
  }


  // My Achor point has been captured
  if ( GetBoardValue( VisibleBoard,
		Player[ActivePlayer].SelRow,
		Player[ActivePlayer].SelCol ) != ActivePlayer )
  {
//		MessageBeep( -1 );
		return;
  }

  int RowDis = abs(row-Player[ActivePlayer].SelRow);
  int ColDis =	abs(column-Player[ActivePlayer].SelCol);
  int JumpDistance = RowDis + ColDis;

  if (JumpDistance > MOVE_LENGTH*2 || ColDis > MOVE_LENGTH 
			|| RowDis > MOVE_LENGTH)
  {
//		MessageBeep( -1 );
		return;
  }
  int before = CountBoardValues( VisibleBoard, ActivePlayer );
  LASTROW = Player[ActivePlayer].SelRow;
  LASTCOL = Player[ActivePlayer].SelCol;
  Player[ActivePlayer].Selected = 0;
  GameBoard[LASTROW][LASTCOL]->Selected = 0;
  if(Trace)
  {
//	OpenTraceFile();
	WriteTraceFile("LBTTN");
	TraceBoard(LASTCOL, LASTROW, column, row);
//	CloseTraceFile();
  }

  if ( ColDis == 2 || RowDis == 2 )
  {
	Jumped = true;
	AniGrid[LASTROW][LASTCOL] = ANI_OUT;
  }
  else
  {
	Jumped = false;
	AniGrid[LASTROW][LASTCOL] = ANI_DOWN;
  }

  AniGrid[row][column] = ANI_WAIT(ActivePlayer);
  Animating = true;
  SetBoardValue( VisibleBoard, row, column, ActivePlayer );
  CaptureBalls( VisibleBoard, ActivePlayer, row, column );
  long delta = labs(time(NULL) - starttime);
  if ( !delta )
		delta++;
  Player[ ActivePlayer ].ticks += delta;

  starttime = time(NULL);
  Player[ ActivePlayer ].Count = CountBoardValues( VisibleBoard, ActivePlayer );
  if(Jumped)	// We haven't emptied the original square yet.
  {
	Player[ActivePlayer].Count--;
  }
  Player[ActivePlayer].NewPieces = Player[ActivePlayer].Count - before;
  ActivePlayer = 1 - ActivePlayer;
  Player[ ActivePlayer ].Count = CountBoardValues( VisibleBoard, ActivePlayer );

  
  unsigned Brd[NUM_SQUARES_Y];
  memcpy( Brd,VisibleBoard, NUM_SQUARES_Y * sizeof(unsigned) );
  if(Jumped)
  {
 	SetBoardValue(Brd,LASTROW,LASTCOL,EMPTY);
  }
  // see if opponent has any moves left
  if ( !AnyMovesAvailable( Brd, ActivePlayer ) )
  {
		ActivePlayer = 1 - ActivePlayer;
		CreateFinishTimer();
  }
  else
  {
	CreateTimer();
  }

  WriteScore();
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void GameWindow::ComputersTurn()
{
	if ( Player[ActivePlayer].Mode == HUMAN_MODE )
		return;

	StopTimer();

	IsBusy = 1;

//	SetCursor( 0,IDC_WAIT);
	DrawScoreBoard();
	WriteScore();

	PegMessageWindow *pWin;
	if(Jumping)
	{
		PEGCHAR s[16];
		AsciiToUnicode(s, "Thinking...");
		pWin = new PegMessageWindow(LS(SID_BOLAS), s, FF_RAISED);
		Presentation()->Center(pWin);
		Presentation()->Add(pWin);
	}

	starttime = time(NULL);
	int depth = 1;
//	if ( Trace )
//		OpenTraceFile();

	int before = CountBoardValues( VisibleBoard, ActivePlayer );

	if ( Player[ ActivePlayer ].Mode <= AI_MODE_2 )
	{
		MovesConsidered = 0;
		MovesRejected = 0;
		NumberOfNodes = 0;
		RedundantMoves = 0;
      	SecondarySearch = 0;
		ComputeNextMove( ActivePlayer, Player[ ActivePlayer ].Mode, VisibleBoard, depth );
	}
	else
	{
		PartialDepth = 0;
		MaxDepth = FullDepth[ ActivePlayer ];
		while( 1 )
		{	   
			NODE   	Tree;
			TreeParent = &Tree;
			Tree.Parent = 0;
			Tree.Player = ActivePlayer;
			Tree.VirtualPlayer = ActivePlayer;
			Tree.Depth = 0;
			Tree.AnchorX = 99;
			Tree.AnchorY = 99;
			Tree.NextX = 99;
			Tree.NextY = 99;
			Alpha = ALPHA_SENTINEL;
			Beta = BETA_SENTINEL;
			NumberOfNodes = 0;
			MovesConsidered = 0;
         RedundantMoves = 0;
			MovesRejected = 0;
         SecondarySearch = 0;
	      InitQueue( &Tree.Children );
			memcpy( Tree.Board, VisibleBoard, sizeof(unsigned) * NUM_SQUARES_Y );
			if ( Trace )
			{
				WriteTraceFile( "**** Trace Start: ****\n" );
			}

			ComputeNextMoveTree( Tree );
			if ( !PartialDepth )
				MakeBestMoveTree( Tree );
			RemoveNode( &Tree );
         if ( Trace )
			{
				WriteTraceFile( "**** Trace END: ****\n" );
			}
			if ( !PartialDepth )
			{
				MaxDepth = FullDepth[ ActivePlayer ];
				break;
			}
			else
			{
				MovesConsidered = 0;
				MovesRejected = 0;
				NumberOfNodes = 0;
				RedundantMoves = 0;
		      	SecondarySearch = 0;
				ComputeNextMove( ActivePlayer, AI_MODE_2, VisibleBoard, 1 );
				break;
			}
		}
	}

//	if ( Trace )
//	   	CloseTraceFile();
	

	long delta = labs( time(NULL) - starttime );
	if ( !delta )
		delta++;
	Player[ ActivePlayer ].ticks += delta;
	Player[ ActivePlayer ].Count = CountBoardValues( VisibleBoard, ActivePlayer );
	if(Jumped)	// We haven't emptied the original square yet
	{
		Player[ActivePlayer].Count--;
	}
	Player[ ActivePlayer ].NewPieces = Player[ ActivePlayer ].Count - before;
	ActivePlayer = 1 - ActivePlayer;
	Player[ ActivePlayer ].Count = CountBoardValues( VisibleBoard, ActivePlayer );
   
	//SetCursor(GetModule(), "Wand");
	
	IsBusy = 0;
	
	unsigned Brd[NUM_SQUARES_Y];
	memcpy( Brd,VisibleBoard, NUM_SQUARES_Y * sizeof(unsigned) );
	if(Jumped)
	{
		SetBoardValue(Brd,LASTROW,LASTCOL,EMPTY);
	}

	if ( !AnyMovesAvailable( Brd, ActivePlayer ) )

	{
			ActivePlayer = 1 - ActivePlayer;
			if ( !AnyMovesAvailable( Brd, ActivePlayer ) )
			{
				IsGameOver = 1;
				DrawGameOver();
				WriteScore();
			}
			else
			{
				CreateFinishTimer();
			}
	}
	else if ( Player[ActivePlayer].Mode != HUMAN_MODE )
		CreateTimer();
	starttime = time(NULL);
	WriteScore();
	if(Jumping)
	{
		Presentation()->Destroy(pWin);
	}

}	


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void GameWindow::EvTimer(UINT id)
{
  if ( id == TimerId )
  {
		if ( Player[ActivePlayer].Mode == HUMAN_MODE )
			return;

		if ( !IsGameOver && !Animating)
			ComputersTurn();
  }
  if ( id == FinishId )
  {
		if(Animating)
		{
			return;
		}
		if ( AnyMovesAvailable( VisibleBoard, ActivePlayer ) )
		{
			ComputeNextMove( ActivePlayer, AI_MODE_1, VisibleBoard, 1 );

			Player[ 0 ].Count = CountBoardValues( VisibleBoard, 0 );
			Player[ 1 ].Count = CountBoardValues( VisibleBoard, 1 );
			WriteScore();
		}
		else
		{
			IsGameOver = 1;
			DrawGameOver();
			WriteScore();
			StopFinishTimer();
		}
  }

  if ( id == AnimateId )
  {
		if ( IsPause )
			return;
	
		EvAnimate();
  }
}



/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// ----------- Board Manipulation --------------
void GameWindow::SetBoardValue( unsigned Brd[NUM_SQUARES_Y], int row, int col, int val )
{
	int tmpval = GetBoardValue( Brd, row, col );
	
	unsigned tmp = Brd[ row ];				   
	unsigned mask = BOARD_MASK << (col*2);
	tmp &= ~mask;
	tmp |= mask & (val << (col*2));
	Brd[ row ] = tmp;

	return;
}

void GameWindow::EvAnimate()
{
	int col,row;
    bool done = true;
	for(row = 0; row < NUM_SQUARES_Y; row++)
	{
		for(col = 0; col < NUM_SQUARES_X; col++)
		{

			int state = GameBoard[row][col]->GetCurrentState();
			int ani = AniGrid[row][col];
			int owner = GetBoardValue(VisibleBoard,row,col);

			if(ani <= ANI_NONE)
			{
				continue;
			}
			done = false;
			if(ani == ANI_DOWN || ani == ANI_OUT)
			{
				if(state == MICRO1 || state == MICRO2)
				{
					if(ani == ANI_DOWN)
					{
						AniGrid[row][col] = ANI_UP(owner);
					}	
					else
					{
						AniGrid[row][col] = ANI_NONE;
						GameBoard[row][col]->SetState(EMPTY);
						SetBoardValue(VisibleBoard,row,col,EMPTY);
					}
					for(int i = 0; i < NUM_SQUARES_Y; i++)
					{
						for(int j = 0; j < NUM_SQUARES_X; j++)
						{
							if(AniGrid[j][i] == ANI_WAIT(owner))
							{
								AniGrid[j][i] = ANI_UP(owner);
							}
						}
					}
					col = NUM_SQUARES_Y;
					row = NUM_SQUARES_X;
				}
				else if((state == PLAYER1 || state == PLAYER2))
				{
					GameBoard[row][col]->SetState(MEDIUM(owner));
				}
				else
				{
					GameBoard[row][col]->SetState(state - 1);
				}
			}
			else if(ani == ANI_UP1 || ani == ANI_UP2)
			{
				if(state == MEDIUM(owner))
				{
					GameBoard[row][col]->SetState(PLAYER(owner));
					AniGrid[row][col] = ANI_NONE;
				}
				else if(state == EMPTY || state == PLAYER(1 - owner))
				{
					GameBoard[row][col]->SetState(MICRO(owner));
				}
				else if (state == PLAYER(owner))
				{
					AniGrid[row][col] = ANI_NONE;  // THIS SHOULDN'T HAPPEN
				}
				else
				{
					GameBoard[row][col]->SetState(state + 1);
				}
			}
		}
	}
	if(done)
	{
		Animating = false;

		Player1Arrow->SetState(ActivePlayer);
		Player1Ball->SetState(ActivePlayer);
		Player2Arrow->SetState(ActivePlayer);
		Player2Ball->SetState(ActivePlayer);

	}
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
int	GetBoardValue( unsigned Brd[NUM_SQUARES_Y], int row, int col )
{
	if ( row < 0 || row >= NUM_SQUARES_Y )
		return -1;

	if ( col < 0 || col >= NUM_SQUARES_X )
		return -1;

	unsigned tmp = Brd[ row ];

	tmp = tmp >> (col << 1);
	return( tmp & BOARD_MASK );
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
int	AnyMovesAvailable( unsigned Brd[NUM_SQUARES_Y], int player )
{
	
	for ( int x = 0; x < NUM_SQUARES_X; x++ )
	{
		for ( int y = 0; y < NUM_SQUARES_Y; y++ )
		{
      	if ( GetBoardValue( Brd,y,x ) != player )
						continue;
			for ( int i = -MOVE_LENGTH; i <= MOVE_LENGTH; i++ )
			{
				for ( int j = -MOVE_LENGTH; j <= MOVE_LENGTH; j++ )
				{
					if ( !i && !j )
						continue;
					if ( GetBoardValue( Brd,y+j,x+i ) == EMPTY )
               	return 1;
				}
			}
		 }
	}
	return 0;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
int	CountBoardValues( unsigned Brd[NUM_SQUARES_Y], unsigned int val )
{
	int cnt = 0;
	for ( int row = 0; row < NUM_SQUARES_Y; row++ )
	{
		unsigned tmp = Brd[ row ];
      
		// 1
		if ( (tmp & BOARD_MASK) == val )
				cnt++;
		// 2
		tmp = tmp >> 2;
		if ( (tmp & BOARD_MASK) == val )
				cnt++;
		// 3
		tmp = tmp >> 2;
		if ( (tmp & BOARD_MASK) == val )
				cnt++;
		// 4
		tmp = tmp >> 2;
		if ( (tmp & BOARD_MASK) == val )
				cnt++;
		// 5
		tmp = tmp >> 2;
		if ( (tmp & BOARD_MASK) == val )
				cnt++;
		// 6
		tmp = tmp >> 2;
		if ( (tmp & BOARD_MASK) == val )
				cnt++;
		// 7
		tmp = tmp >> 2;
		if ( (tmp & BOARD_MASK) == val )
				cnt++;
	}
	return( cnt );
}

const int PlausibleOneMove = 1;
const int PlausibleInitial = 0;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
int	GameWindow::ComputeNextMove( int player, int mode, unsigned Brd[NUM_SQUARES_Y], int depth )
{

	int	LargestCount = -100;
	int	BestX = -1;
	int 	BestY = -1;
	int	ix;
	int	iy;
	int	AnchorX;
	int	AnchorY;
	char	PlausibleMap[ NUM_SQUARES_Y ][ NUM_SQUARES_X ];
//	memset( &PlausibleMap[0][0],PlausibleInitial,NUM_SQUARES_Y*NUM_SQUARES_X);
	for(int i = 0; i < NUM_SQUARES_Y; i++)
	{
		for(int j = 0; j < NUM_SQUARES_X; j++)
		{
			PlausibleMap[i][j] = PlausibleInitial;
		}
	}
	int InitialCount = CountBoardValues( Brd, player );

	unsigned TmpBrd[NUM_SQUARES_Y];
	for ( int y = 0; y <= NUM_SQUARES_Y; y++ )
	{
		for ( int x = 0; x <= NUM_SQUARES_X; x++ )
		{
			if ( GetBoardValue( Brd, y, x ) == player )
			{
				if ( depth )
					NumberOfNodes++;
				for ( iy = -MOVE_LENGTH; iy <= MOVE_LENGTH; iy++ )
				{
					for ( ix = -MOVE_LENGTH; ix <= MOVE_LENGTH; ix++ )
					{
						if ( ix == 0 && iy == 0 )
							continue;

						int yt = y + iy;
						int xt = x + ix;
						if ( yt < 0 || xt < 0 || yt >= NUM_SQUARES_Y ||
							xt >= NUM_SQUARES_X )
							continue;

						if ( GetBoardValue( Brd, yt, xt ) == EMPTY )
						{
							int RowDis = abs(y - yt);
							int ColDis = abs(x - xt);
							int JumpDistance = RowDis + ColDis;

							if (JumpDistance > MOVE_LENGTH*2 || 
									ColDis > MOVE_LENGTH || RowDis > MOVE_LENGTH)
								continue;

							if ( ((ColDis == 1 && RowDis == 1)) ||
								((ColDis == 1) && (RowDis == 0)) ||
								((ColDis == 0) && (RowDis == 1))  )
							{
								if ( PlausibleMap[yt][xt] != PlausibleInitial )
								{				 
									if ( depth )
										RedundantMoves++;
									continue;
								}

								else
								{
									PlausibleMap[yt][xt] = PlausibleOneMove;
								}				
							}

							memcpy( TmpBrd,Brd, NUM_SQUARES_Y * sizeof(unsigned) );
							if ( ColDis == 2 || RowDis == 2 )
							{
								SetBoardValue( TmpBrd, y,x, EMPTY );
							}
							SetBoardValue( TmpBrd, yt,xt,player );
							CaptureBalls( TmpBrd, player, yt, xt );
							int TmpCount;

							if ( depth )
								MovesConsidered++;
							else
								SecondarySearch++;

							TmpCount = CountBoardValues( TmpBrd, player );


							if ( depth && mode == AI_MODE_2 )
							{
								TmpCount -= ComputeNextMove( 1 - player, mode, TmpBrd, depth - 1 );
							}

							if ( TmpCount < LargestCount )
							{
								if ( depth )
									MovesRejected++;
							}

							if ( TmpCount == LargestCount )
							{
								if ( (rand() % 10) > 5 )
								{
									LargestCount = TmpCount;
									BestX = xt;
									BestY = yt;
									AnchorX = x;
									AnchorY = y;
								}
							}

							if ( TmpCount > LargestCount )
							{
									LargestCount = TmpCount;
									BestX = xt;
									BestY = yt;
									AnchorX = x;
									AnchorY = y;
							}
						}
					}
				}
			}
		}
	}

	if ( BestX == -1 )
	{
      return 0;
	}

	int RowDis = abs(AnchorY - BestY);
	int ColDis = abs(AnchorX - BestX);

	if(Trace && (&Brd[0] == &VisibleBoard[0]))
	{
		WriteTraceFile("CNM");
		TraceBoard(AnchorX,AnchorY,BestX,BestY);
	}
	if(&Brd[0] == &VisibleBoard[0])
	{
		AniGrid[BestY][BestX] = ANI_WAIT(player);
		if(ColDis == 2 || RowDis == 2)
		{
			Jumped = true;
			AniGrid[AnchorY][AnchorX] = ANI_OUT;
		}
		else
		{
			Jumped = false;
			AniGrid[AnchorY][AnchorX] = ANI_DOWN;
		}
		Animating = true;
		LASTROW = AnchorY;
		LASTCOL = AnchorX;
	}
	SetBoardValue( Brd, BestY,BestX,player );
	CaptureBalls( Brd, player, BestY, BestX );
	if(Jumped)
	{
		return (CountBoardValues( Brd, player ) - InitialCount - 1);
	}
	else
	{
		return (CountBoardValues( Brd, player ) - InitialCount);
	}
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
NODE	*LargestPlausible( QUEUE *pPlauseQueue )
{
	NODE	*pLargest = (NODE *)RemoveQueueElement( pPlauseQueue );

	int	wSize = pPlauseQueue->wSize;
	while( wSize )
	{
		NODE	*pTmp = (NODE *)RemoveQueueElement( pPlauseQueue );
		if ( pTmp->Value > pLargest->Value )
		{
			InsertQueueElement( pPlauseQueue,(ELEMENT *)pLargest );
			pLargest = pTmp;
		}
		else
		{
			InsertQueueElement( pPlauseQueue,(ELEMENT *)pTmp );
		}
		wSize--;
	}
	return pLargest;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
int	GameWindow::ComputeNextMoveTree( NODE& Tree )
{
	int	ix;
	int	iy;
	QUEUE	PlauseMoves;
	char	PlausibleMap[ NUM_SQUARES_Y ][ NUM_SQUARES_X ];

	// Alpha Set
	if ( Tree.Depth == 1 )
	{
		Beta = BETA_SENTINEL;
		NODE *pTmp;
		int	wSize;
		pTmp = Tree.Parent;
		wSize = pTmp->Children.wSize;
		NODE *pChild;
		for ( int i = 0; i < wSize; i++ )
		{
			pChild = (NODE *)RemoveQueueElement( &pTmp->Children );
			if ( pChild->Value != NODE_SENTINEL )
			{
				if ( pChild->Value > Alpha )
					Alpha = pChild->Value;
			}
			InsertQueueElement( &pTmp->Children,(ELEMENT *)pChild );

		}
	}

	// Alpha Test
	if ( (Tree.Depth == 2) && (Alpha != ALPHA_SENTINEL) )
	{
		NODE *pTmp = Tree.Parent;
		if ( pTmp->Alpha )
			return -1;

		if ( pTmp->Value != NODE_SENTINEL && pTmp->Value < Alpha )
		{
			pTmp->Alpha = 1;
         return -1;
		}
	}

	// Beta Set
	if ( Tree.Depth == 2 )
	{
		NODE *pTmp;
		int	wSize;
		pTmp = Tree.Parent;
		wSize = pTmp->Children.wSize;
		NODE *pChild;
		for ( int i = 0; i < wSize; i++ )
		{
			pChild = (NODE *)RemoveQueueElement( &pTmp->Children );
			if ( pChild->Value != NODE_SENTINEL )
			{
				if ( pChild->Value < Beta )
					Beta = pChild->Value;
			}
			InsertQueueElement( &pTmp->Children,(ELEMENT *)pChild );

		}
	}

   // Beta Test
	if ( (Tree.Depth == 3) && (Beta != BETA_SENTINEL) )
	{
		NODE *pTmp = Tree.Parent;
		if ( pTmp->Beta )
			return -1;

		if ( pTmp->Value != NODE_SENTINEL && pTmp->Value > Beta )
		{
			pTmp->Beta = 1;
         return -1;
		}
	}

	int i,j;
//	memset( &PlausibleMap[0][0],PlausibleInitial,NUM_SQUARES_Y*NUM_SQUARES_X);
	for(i = 0; i < NUM_SQUARES_X; i++)
	{
		for(j = 0; j < NUM_SQUARES_Y; j++)
		{
			//memset(&PlausibleMap[i][j], PlausibleInitial, 1);
			PlausibleMap[i][j] = PlausibleInitial;
		}
	}

	InitQueue( &PlauseMoves );
	unsigned Brd[NUM_SQUARES_Y];
	memcpy( Brd,Tree.Board,NUM_SQUARES_Y * sizeof(unsigned) );

	for ( int y = 0; y <= NUM_SQUARES_Y; y++ )
	{
		for ( int x = 0; x <= NUM_SQUARES_X; x++ )
		{
			// if we own this square...
			if ( (unsigned)GetBoardValue( Brd, y, x ) == Tree.VirtualPlayer )
			{
				NumberOfNodes++;
				// search surrounding squares for plausible move
				for ( iy = -MOVE_LENGTH; iy <= MOVE_LENGTH; iy++ )
				{
					for ( ix = -MOVE_LENGTH; ix <= MOVE_LENGTH; ix++ )
					{
						if ( ix == 0 && iy == 0 )
							continue;

						int yt = y + iy;
						int xt = x + ix;
						if ( yt < 0 || xt < 0 || yt >= NUM_SQUARES_Y ||
							xt >= NUM_SQUARES_X )
							continue;

						if ( GetBoardValue( Brd, yt, xt ) == EMPTY )
						{
							int RowDis = abs(y - yt);
							int ColDis = abs(x - xt);
							int JumpDistance = RowDis + ColDis;

							if (JumpDistance > MOVE_LENGTH*2 || 
									ColDis > MOVE_LENGTH || RowDis > MOVE_LENGTH)
								continue;
									
							if ( ((ColDis == 1 && RowDis == 1)) ||
								((ColDis == 1) && (RowDis == 0)) ||
								((ColDis == 0) && (RowDis == 1))  )
							{
								if ( PlausibleMap[yt][xt] != PlausibleInitial )
								{				 
									RedundantMoves++;
									continue;
								}
								else
								{
									PlausibleMap[yt][xt] = PlausibleOneMove;
								}
							}
							
							unsigned TmpBrd[NUM_SQUARES_Y];
							memcpy( TmpBrd,Brd, NUM_SQUARES_Y * sizeof(unsigned) );

							if ( ColDis == 2 || RowDis == 2 )
							{
								SetBoardValue( TmpBrd, y,x, EMPTY );
							}				 
							SetBoardValue( TmpBrd, yt,xt,Tree.VirtualPlayer );
							CaptureBalls( TmpBrd, Tree.VirtualPlayer, yt, xt );

							int TmpCount = CountBoardValues( TmpBrd, Tree.VirtualPlayer );
							TmpCount -= CountBoardValues( TmpBrd, 1 - Tree.VirtualPlayer );

							MovesConsidered++;

							NODE *pPlauseMove = new NODE;
							pPlauseMove->AnchorX = x;
							pPlauseMove->AnchorY = y;
							pPlauseMove->NextX = xt;
							pPlauseMove->NextY = yt;
							pPlauseMove->Value = TmpCount;

							memcpy( pPlauseMove->Board,TmpBrd, NUM_SQUARES_Y * sizeof(unsigned) );
							InsertQueueElement( &PlauseMoves, (ELEMENT *)pPlauseMove );

						}
					}								
				}	
			}	
		}	
	}	


	if ( PlauseMoves.wSize == 0 )
	{
		PartialDepth = 1;
		int Max = CountBoardValues( Tree.Board, Tree.VirtualPlayer );
		int Min = CountBoardValues( Tree.Board, 1 - Tree.VirtualPlayer );

		Tree.Value = Max - Min;
		PropagateValuesUp( &Tree );
		return 0;
	}


	int	OriginalSize = PlauseMoves.wSize;

	NODE *pMoveLow = (NODE *)RemoveQueueElement( &PlauseMoves );
	int	wSize = PlauseMoves.wSize;
	while( wSize > 0 )
	{
		NODE *pTmpMove = (NODE *)RemoveQueueElement( &PlauseMoves );

		if ( pTmpMove->Value < pMoveLow->Value )
		{
			InsertQueueElement( &PlauseMoves, (ELEMENT *)pMoveLow );
			pMoveLow = pTmpMove;
		}
		else
		{
			InsertQueueElement( &PlauseMoves, (ELEMENT *)pTmpMove );
		}
      wSize--;
	}

	if ( pMoveLow )
	{
		if ( PlauseMoves.wSize )
			delete pMoveLow;
		else
			InsertQueueElement( &PlauseMoves, (ELEMENT *)pMoveLow );
	}
 
	QUEUE	OrderedMoves;
	InitQueue( &OrderedMoves );
	NODE	*pLarge;
	int	TaperSize;
	if ( Tree.Depth == 0 )
		TaperSize = Taper1[ Tree.Player ];

	else if ( Tree.Depth == 1 )
		TaperSize = Taper2[ Tree.Player ];

	else
		TaperSize = Taper3[ Tree.Player ];

	for ( i = 0; i < TaperSize; i++  )
	{
		if ( PlauseMoves.wSize )
		{
			pLarge = LargestPlausible( &PlauseMoves );
			InsertQueueElement( &OrderedMoves,(ELEMENT *)pLarge );
		}
		else
		{
			break;
		}
	}

   // purge left-over moves
	while( PlauseMoves.wSize )
	{
		pLarge = LargestPlausible( &PlauseMoves );
		delete pLarge;
	}

	MovesRejected += (OriginalSize - OrderedMoves.wSize);


	while( OrderedMoves.wSize )
	{
		NODE *pMove = (NODE *)RemoveQueueElement( &OrderedMoves );

		if ( !pMove )
			break;

		NODE	*pNode;

		pNode = pMove;

		AddNode( Tree, pNode );

		if ( Tree.Depth < MaxDepth )
		{
			pNode->Value = NODE_SENTINEL;
			ComputeNextMoveTree( *pNode );

			if ( OrderedMoves.wSize == 0 && pNode->Parent->Parent == 0 )
					DeleteThirdPly( pNode );
		}
		else
		{
			PropagateValuesUp( pNode );
		}
	}
	return 0;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void	GameWindow::PropagateValuesUp( NODE *pNode )
{
	NODE *pTmp = pNode->Parent;
	int Value = pNode->Value;

	if ( !pTmp )
	{
//		MessageBox( "Error: Minimax, Null Parent 1" );
		PegMessageWindow *pWin = new PegMessageWindow(LS(SID_ERROR), 
				LS(SID_ERROR1), FF_RAISED|MW_OK);
		Presentation()->Center(pWin);
		pWin->Execute();
		return;
	}
	while( pTmp->Parent )
	{
		if ( pTmp->Value == NODE_SENTINEL )
				pTmp->Value = Value;

		if ( (pTmp->Depth & 1) )
		{
				if ( pTmp->Value < Value )
				{
					return;
				}
				else
				{
					pTmp->Value = Value;
				}
		}
		else
		{
				if ( pTmp->Value > Value )
				{
					return;
				}
				else
				{
					pTmp->Value = Value;
				}
		}

		Value = pTmp->Value;

		pTmp = pTmp->Parent;
      if ( !pTmp )
		{
//			MessageBox( "Error: Minimax, Null Parent 2" );
			PegMessageWindow *pWin = new PegMessageWindow(LS(SID_ERROR), 
					LS(SID_ERROR2), FF_RAISED|MW_OK);
			Presentation()->Center(pWin);
			pWin->Execute();
			return;
		}
	}
	return;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void	UpdateAlpha( NODE *pTmp )
{
	if ( pTmp->Depth == 1 )
	{
		if ( Alpha == ALPHA_SENTINEL )
		{
			Alpha = pTmp->Value;
		}
		else
		{
			if ( Alpha < pTmp->Value )
				Alpha = pTmp->Value;
		}
	}
	return;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void	GameWindow::MakeBestMoveTree( NODE& Tree )
{
	NODE *pTmp = 0;
	NODE *pBestMax = 0;
	int	Value = -99;
	int	wSize = Tree.Children.wSize;
	for ( int i = 0; i < wSize; i++ )
	{
		pTmp = (NODE *)RemoveQueueElement( &Tree.Children );
		if ( pTmp->Value == NODE_SENTINEL )
		{
//			MessageBox( "Error: Minimax, Invalid Branch" );
			PegMessageWindow *pWin = new PegMessageWindow(LS(SID_ERROR), 
					LS(SID_ERROR3), FF_RAISED|MW_OK);
			Presentation()->Center(pWin);
			pWin->Execute();
		}
		if ( (pTmp->Value == Value) && ((rand() % 10) >= 5) )
		{
			pBestMax = pTmp;
		}
		if ( pTmp->Value > Value )
		{
			Value = pTmp->Value;
			pBestMax = pTmp;
		}

		InsertQueueElement( &Tree.Children, (ELEMENT *)pTmp );
	}

	if ( pBestMax )
	{
		int	AnchorX = pBestMax->AnchorX;
		int	AnchorY = pBestMax->AnchorY;
		int	BestX = pBestMax->NextX;
		int	BestY = pBestMax->NextY;

		int RowDis = abs(AnchorY - BestY);
		int ColDis = abs(AnchorX - BestX);

		if(Trace)
		{
			WriteTraceFile("MBMT");
			TraceBoard(AnchorX,AnchorY,BestX,BestY);
		}
		if ( ColDis == 2 || RowDis == 2 )
		{
			AniGrid[AnchorY][AnchorX] = ANI_OUT;
			Jumped = true;
		}
		else
		{
			AniGrid[AnchorY][AnchorX] = ANI_DOWN;
			Jumped = false;
		}							  			 	 		 

		AniGrid[BestY][BestX] = ANI_WAIT(ActivePlayer);
		Animating = true;
		LASTROW = BestY;
		LASTCOL = BestX;
		SetBoardValue( VisibleBoard, BestY,BestX, pBestMax->Player );
		CaptureBalls( VisibleBoard, pBestMax->Player, BestY, BestX );
	}
	else
	{
//		MessageBox( "Error: Minimax, No Move Found!" );
		PegMessageWindow *pWin = new PegMessageWindow(LS(SID_ERROR), 
				LS(SID_ERROR4), FF_RAISED|MW_OK);
		Presentation()->Center(pWin);
		pWin->Execute();
	}
	return;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void	GameWindow::DeleteThirdPly( NODE *pNode )
{
	NODE *pTmp = 0;
	int wSize = pNode->Children.wSize;
	for ( int i = 0; i < wSize; i++ )
	{
		pTmp = (NODE *)RemoveQueueElement( &pNode->Children );
      RemoveNode( pTmp );
	}

	return;
}

#ifndef NULL
#define	NULL 0L
#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
ELEMENT	*RemoveQueueElement( QUEUE *pQueue )
{
	ELEMENT	*pElement;

	if ( pQueue->wSize == 0 )
	{
		return 0;
	}			 

	pElement = (ELEMENT *)pQueue->pHead;
	if ( !pElement )
	{
//   	MessageBeep(-1);
		return 0;
	}

	pQueue->pHead = pElement->pNext;
	pQueue->wSize--;
	
	return( pElement );
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void	InsertQueueElement( QUEUE *pQueue, ELEMENT *pNewElement )
{					   
	ELEMENT	*pElement;

	if ( pQueue->pHead == NULL )
		pQueue->pHead = pNewElement;
	
	else
	{
		pElement = (ELEMENT *)pQueue->pTail;
		pElement->pNext = pNewElement;
	}

	pNewElement->pNext = NULL;
	pQueue->pTail = pNewElement;

	pQueue->wSize++;

	return;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void	InitQueue( QUEUE *pQueue )
{
	pQueue->pHead = NULL;
	pQueue->pTail = NULL;
	pQueue->wSize = 0;
	return;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


SIGNED GameWindow::Message(const PegMessage &Mesg)
{
	PegMessage NewMessage;
	switch(Mesg.wType)
	{
	case SIGNAL(IDM_OPTION, PSF_CLICKED):
		CmOptions();
		break;
	case SIGNAL(IDM_PAUSE, PSF_CLICKED):
		CmPause();
		break;
	case SIGNAL(IDM_ABOUT, PSF_CLICKED):
		CmAbout();
		break;
	case SIGNAL(IDM_NEW, PSF_CLICKED):
		NewGame();
		break;
	case PM_LBUTTONDOWN:
		EvLButtonDown(0, Mesg.Point);
		break;
	case PM_TIMER:
		EvTimer(Mesg.iData);
		break;
	case PM_KEY:
		// ST driver doesn't seem to send Key Release message
		// so we fake it here.
		if(Mesg.iData == PK_MENU_KEY)
		{
			NewMessage.pTarget = NULL;
			NewMessage.wType = PM_KEY_RELEASE;
			NewMessage.iData = Mesg.iData;
			NewMessage.lData = Mesg.lData;
			PegThing::MessageQueue()->Push(NewMessage);
		}
		PegDecoratedWindow::Message(Mesg);
		break;
    case PM_CURRENT:
		PegDecoratedWindow::Message(Mesg);
		Presentation()->MoveFocusTree(GameBoard[FOCUSY][FOCUSX]);
        break;
	default:
		return(PegDecoratedWindow::Message(Mesg));
    }
	return 0;

}

GameWindow::~GameWindow()
{
	StartPlayer = PLAYER1;
	Screen()->ResetPalette();
}

OptionDialog::OptionDialog(char*)
{}

FILE *fptr;
int GameWindow::OpenTraceFile() 
{
	fptr = fopen("tracefile.txt","a");
	if(fptr == NULL)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}
int	GameWindow::CloseTraceFile() 
{
	fclose(fptr);
	return 0;
}
int	GameWindow::WriteTraceFile( char *s ) 
{
	if(OpenTraceFile())
	{
		fputs(s, fptr);
		fputs("\n", fptr);
		CloseTraceFile();
	}
	return 0;
}


int GameWindow::TraceBoard(int x0,int y0,int x1, int y1)
{
	char s[40];
	s[0] = '\0';
	for(int i = 0; i < NUM_SQUARES_Y; i++)
	{
		for(int j = 0; j < NUM_SQUARES_X; j++)
		{
			sprintf(s, "%s %d", s,GameBoard[i][j]->GetCurrentState());
		}
		WriteTraceFile(s);
		s[0] = '\0';
	}
	sprintf(s, "Player %d  From:(%d,%d) To:(%d,%d)\n",ActivePlayer, x0,y0,x1,y1);
	WriteTraceFile(s);
	return 0;
}

void GameWindow::Draw()
{
	PegDecoratedWindow::Draw();
	WriteScore(); 
//	Presentation()->MoveFocusTree(GameBoard[FOCUSY][FOCUSX]);
}

void GameWindow::EvKeyPress(UINT key)
{
	PegPoint pp;
	switch(key)
	{
		case PK_CR:
			pp.x = BOARD_MIN_X + (FOCUSX * GRID_SIZE);
			pp.y = BOARD_MIN_Y + (FOCUSY * GRID_SIZE);
			GameWindow::EvLButtonDown(0,pp);
			break;
		case PK_LEFT:
			FOCUSX--;
			if(FOCUSX < 0)
			{
				FOCUSX = 0;
			}
			Presentation()->MoveFocusTree(GameBoard[FOCUSY][FOCUSX]);
			break;
		case PK_RIGHT:
			FOCUSX++;
			if(FOCUSX > NUM_SQUARES_X - 1)
			{
				FOCUSX = NUM_SQUARES_X - 1;
			}
			Presentation()->MoveFocusTree(GameBoard[FOCUSY][FOCUSX]);
			break;
		case PK_LNUP:
			FOCUSY--;
			if(FOCUSY < 0)
			{
				FOCUSY = 0;
			}
			Presentation()->MoveFocusTree(GameBoard[FOCUSY][FOCUSX]);
			break;
		case PK_LNDN:
			FOCUSY++;
			if(FOCUSY > NUM_SQUARES_Y - 1)
			{
				FOCUSY = NUM_SQUARES_Y - 1;
			}
			Presentation()->MoveFocusTree(GameBoard[FOCUSY][FOCUSX]);
			break;
		default:
			break;
	
	}
}

void BolasSquare::EvKeyPress(UINT key)
{
	PegPoint pp;
	switch(key)
	{
		case PK_CR:
			pp.x = BOARD_MIN_X + (FOCUSX * GRID_SIZE);
			pp.y = BOARD_MIN_Y + (FOCUSY * GRID_SIZE);
			Owner->EvLButtonDown(0,pp);
			break;
		case PK_LEFT:
			FOCUSX--;
			if(FOCUSX < 0)
			{
				FOCUSX = 0;
			}
			Presentation()->MoveFocusTree(Owner->GameBoard[FOCUSY][FOCUSX]);
			break;
		case PK_RIGHT:
			FOCUSX++;
			if(FOCUSX > NUM_SQUARES_X - 1)
			{
				FOCUSX = NUM_SQUARES_X - 1;
			}
			Presentation()->MoveFocusTree(Owner->GameBoard[FOCUSY][FOCUSX]);
			break;
		case PK_LNUP:
			FOCUSY--;
			if(FOCUSY < 0)
			{
				FOCUSY = 0;
			}
			Presentation()->MoveFocusTree(Owner->GameBoard[FOCUSY][FOCUSX]);
			break;
		case PK_LNDN:
			FOCUSY++;
			if(FOCUSY > NUM_SQUARES_Y - 1)
			{
				FOCUSY = NUM_SQUARES_Y - 1;
			}
			Presentation()->MoveFocusTree(Owner->GameBoard[FOCUSY][FOCUSX]);
			break;
		default:
			break;
	
	}
}

BolasSquare::BolasSquare(GameWindow *inOwner, PegRect Rect, int numstates,int i, int j)
				: PegBitmapLight(Rect,numstates)
{
	Owner = inOwner;
	row = i;
	col = j;
	Selected = 0;
}

SIGNED BolasSquare::Message(const PegMessage &Mesg)
{
	switch(Mesg.wType)
	{
    case PM_CURRENT:
        PegBitmapLight::Message(Mesg);
        DrawFocusIndicator(TRUE);
        break;

    case PM_NONCURRENT:
        PegBitmapLight::Message(Mesg);
        EraseFocusIndicator();
        break;
	case PM_KEY:
		EvKeyPress(Mesg.iData);
		break;
	default:
		PegBitmapLight::Message(Mesg);
	}

	return 0;


}

void BolasSquare::Draw()
{
	BeginDraw();
	PegBitmapLight::Draw();
	if (StatusIs(PSF_CURRENT))
    {
        DrawFocusIndicator(FALSE);
    }
	EndDraw();
	DrawSelection();
}

#define HIGHLIGHT 3
void BolasSquare::DrawSelection()
{
	PegRect FocusRect = mClient;
	FocusRect--;
	if(Selected)
	{
		Invalidate(FocusRect);
		BeginDraw();
		Rectangle(FocusRect, RED, HIGHLIGHT);
		EndDraw();
	}

}

void BolasSquare::DrawFocusIndicator(BOOL bDraw)
{
	if(Selected)
	{
		return;
	}
    PegRect FocusRect = mClient;
    FocusRect--;

    if (bDraw)
    {
        Invalidate(FocusRect);
        BeginDraw();
        Rectangle(FocusRect, DARKGRAY, HIGHLIGHT);
        EndDraw();
    }
    else
    {
        Rectangle(FocusRect, DARKGRAY, HIGHLIGHT);
    }
	MoveToFront(this);

}

void BolasSquare::EraseFocusIndicator(void)
{
    if (!StatusIs(PSF_VISIBLE))
    {
        return;
    }

	if(Selected)
	{
		return;
	}
    PegRect FocusRect = mClient;
    FocusRect--;
    Invalidate(FocusRect);
	PegBitmapLight::Draw();
}

/*--------------------------------------------------------------------------*/

BolasAboutScreen::BolasAboutScreen(PegRect Rect)
	: PegDialog(Rect)//, LS(SID_ABOUT))
{
	PegPrompt *pp1, *pp2;
	PegRect ChildRect;
	PEGCHAR *ps1,*ps2,*ps3;
	char s1[64];
	ps1 = new PEGCHAR[64];
	ps2 = new PEGCHAR[64];
	ps3 = new PEGCHAR[64];

	SetColor(PCI_NORMAL, BLACK);

	int iLeft = Rect.wLeft;//-20;
	int iTop  = Rect.wTop;// + 20;

    ChildRect.Set(iLeft + 4, iTop + 6, iLeft + 204, iTop + 99);
    Add(new PegIcon(ChildRect, &gbLogoBitmap));
    ChildRect.Set(iLeft + 264, iTop + 70, iLeft + 335, iTop + 90);
	strcpy(s1,"Bolas v1.0");
	AsciiToUnicode(ps1,s1);
	pp1 = new PegPrompt(ChildRect, ps1, 0, FF_NONE|AF_TRANSPARENT);
	pp1->SetColor(PCI_NTEXT, WHITE);
	Add(pp1);
    ChildRect.Set(iLeft + 110, iTop + 90, iLeft + 335, iTop + 110);
	strcpy(s1,"Built with the PEG gui library v1.80");
	AsciiToUnicode(ps2,s1);
	pp2 = new PegPrompt(ChildRect, ps2, 0, FF_NONE|AF_TRANSPARENT);
	pp2->SetColor(PCI_NTEXT, WHITE);
	Add(pp2);
    ChildRect.Set(iLeft + 10, iTop + 110, iLeft + 335, iTop + 130);
	strcpy(s1,"Copyright 2002 Jeff Millington, Swell Software, Inc.");
	AsciiToUnicode(ps3,s1);
	pp2 = new PegPrompt(ChildRect, ps3, 0, FF_NONE|AF_TRANSPARENT);
	pp2->SetColor(PCI_NTEXT, WHITE);
	Add(pp2);
    ChildRect.Set(iLeft + 122, iTop + 135, iLeft + 222, iTop + 155);
    Add(new PegTextButton(ChildRect, LS(SID_OK), IDB_OK, AF_ENABLED|TJ_CENTER));

}

BolasAboutScreen::~BolasAboutScreen()
{

}

SIGNED BolasAboutScreen::Message(const PegMessage &Mesg)
{
	switch(Mesg.wType)
	{	
		case 0:
			
		default:
			return PegDialog::Message(Mesg);
	}	
	return 0;
}
/*--------------------------------------------------------------------------*/
BolasHelpScreen::BolasHelpScreen(PegRect Rect)
	: PegDialog(Rect)
{
/*
	PegRect ChildRect;
	PEGCHAR *ps1,*ps2,*ps3;

	ps1 = new PEGCHAR[64];
	ps2 = new PEGCHAR[64];
	ps3 = new PEGCHAR[64];

	int iLeft = Rect.wLeft-20;
	int iTop  = Rect.wTop + 20;

    ChildRect.Set(iLeft + 5, iTop + 10, iLeft + 334, iTop + 30);
	strcpy(ps1,"To play:  Move your marbles around the");
	Add(new PegPrompt(ChildRect, ps1, 0, FF_NONE|AF_TRANSPARENT));
    ChildRect.Set(iLeft + 5, iTop + 30, iLeft + 334, iTop + 50);
	strcpy(ps2,"board trying to capture the opponent's ");
	Add(new PegPrompt(ChildRect, ps2, 0, FF_NONE|AF_TRANSPARENT));
 
   ChildRect.Set(iLeft + 5, iTop + 50, iLeft + 334, iTop + 70);
	strcpy(ps3,"marbles by landing adjacent to them.");//Num Games = Number of games per match");
	Add(new PegPrompt(ChildRect, ps3, 0, FF_NONE|AF_TRANSPARENT));
/*
    ChildRect.Set(iLeft + 5, iTop + 70, iLeft + 334, iTop + 90);
	strcpy(ps3,"Copyright 2002 Swell Software, Inc");
	Add(new PegPrompt(ChildRect, ps3, 0, FF_NONE|AF_TRANSPARENT));
*
    ChildRect.Set(iLeft + 122, iTop + 80, iLeft + 222, iTop + 100);
    Add(new PegTextButton(ChildRect, LS(SID_OK), IDB_OK, AF_ENABLED|TJ_CENTER));
*/

    PegRect ChildRect;
    PegThing *pChild1;

	int iLeft = Rect.wLeft;//-20;
	int iTop  = Rect.wTop;// + 20;

    mReal.Set(iLeft, iTop, iLeft + 443, iTop + 306);
//    InitClient();
    SetColor(PCI_NORMAL, BLACK);
//    AddStatus(PSF_MOVEABLE);

    ChildRect.Set(iLeft + 172, iTop + 254, iLeft + 278, iTop + 282);
    Add(new PegTextButton(ChildRect, LS(SID_OK), IDB_OK, AF_ENABLED|TJ_CENTER));
    ChildRect.Set(iLeft + 37, iTop + 214, iLeft + 353, iTop + 233);
    pChild1 = new PegPrompt(ChildRect, LS(SID_YOULOSE));
    ((PegPrompt *) pChild1)->SetColor(PCI_NTEXT, WHITE);
    Add(pChild1);

    ChildRect.Set(iLeft + 37, iTop + 194, iLeft + 401, iTop + 213);
    pChild1 = new PegPrompt(ChildRect, LS(SID_OVER_ADJ));
    ((PegPrompt *) pChild1)->SetColor(PCI_NTEXT, WHITE);
    Add(pChild1);

    ChildRect.Set(iLeft + 17, iTop + 174, iLeft + 435, iTop + 193);
    pChild1 = new PegPrompt(ChildRect, LS(SID_JUMP_HELP));
    ((PegPrompt *) pChild1)->SetColor(PCI_NTEXT, WHITE);
    Add(pChild1);

    ChildRect.Set(iLeft + 17, iTop + 144, iLeft + 377, iTop + 163);
    pChild1 = new PegPrompt(ChildRect, LS(SID_NUM_HELP));
    ((PegPrompt *) pChild1)->SetColor(PCI_NTEXT, WHITE);
    Add(pChild1);

    ChildRect.Set(iLeft + 213, iTop + 88, iLeft + 387, iTop + 107);
    pChild1 = new PegPrompt(ChildRect, LS(SID_LANDING));
    ((PegPrompt *) pChild1)->SetColor(PCI_NTEXT, WHITE);
    Add(pChild1);

    ChildRect.Set(iLeft + 4, iTop + 6, iLeft + 204, iTop + 99);
    Add(new PegIcon(ChildRect, &gbLogoBitmap));
    ChildRect.Set(iLeft + 213, iTop + 68, iLeft + 394, iTop + 87);
    pChild1 = new PegPrompt(ChildRect, LS(SID_THEOPP));
    ((PegPrompt *) pChild1)->SetColor(PCI_NTEXT, WHITE);
    Add(pChild1);

    ChildRect.Set(iLeft + 213, iTop + 48, iLeft + 435, iTop + 67);
    pChild1 = new PegPrompt(ChildRect, LS(SID_AROUND));
    ((PegPrompt *) pChild1)->SetColor(PCI_NTEXT, WHITE);
    Add(pChild1);

    ChildRect.Set(iLeft + 213, iTop + 28, iLeft + 411, iTop + 47);
    pChild1 = new PegPrompt(ChildRect, LS(SID_TOPLAY));
    ((PegPrompt *) pChild1)->SetColor(PCI_NTEXT, WHITE);
    Add(pChild1);


}

BolasHelpScreen::~BolasHelpScreen()
{

	
}

SIGNED BolasHelpScreen::Message(const PegMessage &Mesg)
{
	switch(Mesg.wType)
	{	
		case SIGNAL(IDB_OK, PSF_CLICKED):
			return PegDialog::Message(Mesg);

		default:
			return PegDialog::Message(Mesg);
	}	
	return 0;
}



// Don't include PegAppInitialize when building for ST
#ifndef PEGOS20
void PegAppInitialize(PegPresentationManager *pPresent)
{
	PegRect Rect;
	Rect.Set(0,0,300,300);
	GameWindow *gw = new GameWindow(Rect);
	pPresent->Add(gw);		

}
#endif