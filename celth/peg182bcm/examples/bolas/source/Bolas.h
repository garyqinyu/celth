//----------------------------------------------------------------------------
// ObjectWindows - (C) Copyright 1991, 1993 by Borland International
//----------------------------------------------------------------------------

#ifndef BOLAS_H
#define BOLAS_H

#include "moves.h"
#include "tree.h"

#define UINT unsigned int

#define IDM_RESET	100
#define IDM_OPTION	101
#define IDM_ABOUT	102
#define IDM_PAUSE	103
#define IDM_STOP	104
#define IDM_NEW		105

#define IDB_HELP	141

#define NODE_SENTINEL -100
#define SCREEN_WIDTH  590
#define SCREEN_HEIGHT  395
#define GRID_SIZE  50
#define NUM_SQUARES_X 7
#define NUM_SQUARES_Y  7
#define BOARD_MIN_X  26
#define BOARD_MAX_X (BOARD_MIN_X + NUM_SQUARES_X * GRID_SIZE)
#define BOARD_MIN_Y  53
#define BOARD_MAX_Y (BOARD_MIN_Y + NUM_SQUARES_Y * GRID_SIZE)

#define SCOREBOARD_X (BOARD_MAX_X + 6)
#define SCOREBOARD_Y ((BOARD_MAX_Y + BOARD_MIN_Y) / 2)
#define SCOREBOARD_WIDTH  200
#define SCOREBOARD_HEIGHT  200

#define BOLAS_ID 500

#define ANI_WAIT1 0
#define ANI_WAIT2 1
#define ANI_NONE  2
#define ANI_UP1   3
#define ANI_UP2	  4
#define ANI_DOWN  5
#define ANI_OUT	  6

const int PLAYER1 = 0;
const int PLAYER2 = 1;
const int EMPTY   = 3;
const int MICRO1  = 4;
const int LITTLE1 = 5;
const int MEDIUM1 = 6;
const int MICRO2  = 7;
const int LITTLE2 = 8;
const int MEDIUM2 = 9;


class GameWindow;

class BolasSquare : public PegBitmapLight
{
  public:
	BolasSquare(GameWindow *inOwner, PegRect Rect, int numstates,int i,int j);
	~BolasSquare(){}
	void Draw();
	SIGNED Message(const PegMessage &Mesg);

	void DrawFocusIndicator(BOOL bDraw);
	void EraseFocusIndicator();
	void EvKeyPress(UINT key);
	void DrawSelection();
	int Selected;

  private:
	GameWindow *Owner;
	int row,col;

};

			 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class GameWindow : public PegDecoratedWindow {
  public:
	GameWindow(PegRect Rect);
	~GameWindow();

    SIGNED Message(const PegMessage &Mesg);
	void Draw();

	void        CreateTimer();
	void		CreateAnimationTimer();
	void		CreateFinishTimer();
	void		StopTimer();
	void		StopAnimationTimer();
	void		StopFinishTimer();

	void        DrawGameOver();
	void 		DrawOwner(PegPoint &p, int Owner);
	void        DrawScoreBoard();
	void		DrawBoard();
	void        WriteScore();
	void		ClearBoard( unsigned Brd[NUM_SQUARES_Y] );

	void		InitBoard( unsigned Brd[NUM_SQUARES_Y] );
	void		SetPlayerMode( int, int );
	void		ResetPlayerCount( int );
	void		CaptureBalls( unsigned Brd[NUM_SQUARES_Y],int p, int row, int col );
	void 		ClaimSquare( unsigned Brd[NUM_SQUARES_Y], int p, int row, int col );
	void		SetBoardValue( unsigned Brd[NUM_SQUARES_Y], int row, int col, int val ) ;

	void  		ComputersTurn();
	int			ComputeNextMove( int player, int mode, unsigned Brd[NUM_SQUARES_Y], int depth );
	int			ComputeNextMoveTree( NODE& Tree );
	void		PropagateValuesUp( NODE *pNode );
	void		MakeBestMoveTree( NODE& Tree );
	void		DeleteThirdPly( NODE *pNode );

	int			OpenTraceFile();
	int			CloseTraceFile();
	int			WriteTraceFile( char *s );
	int			TraceBoard(int x0,int x1,int y0, int y1);

	void		EvLButtonDown(UINT, PegPoint);
	BolasSquare *GameBoard[NUM_SQUARES_X][NUM_SQUARES_Y];
	BOOL        IsGameOver, IsPause, IsBusy, IsFirstTime, bTimerActive ;
	void EvKeyPress(UINT key);


  protected:
    void SetupWindow(PegRect Rect);

    // message response functions
    //
	void CmAbout();      
    void CmOptions();    
	void CmPause();       
	void CmResetGame();   
    void NewGame();
	void CmStopGame();     
    void EvDestroy();
	void EvTimer(UINT timerID);
	void EvAnimate();

  private:
	PegIcon*	Player1;
	PegIcon*	Player2;
	PegIcon*	Player1Small;
	PegIcon*	Player2Small;
	PegIcon*	Player1Turn;
	PegIcon*	Player2Turn;
	PegBitmap*	Player1Animate[5];
	PegBitmap*	Player2Animate[5];
	PegIcon*	Player1Volts;
	PegIcon*	Player2Volts;
	PegIcon*	Empty;
	PegIcon*	BlankTurn;
	 	
    PegIcon*	GameOver;
    PegIcon*	ScoreBoard;
	 //TCursor*    CursorDown;
	 //TCursor*    CursorUp;

	PegBitmapLight *Player1Arrow, *Player2Arrow;
	PegBitmapLight *Player1Ball, *Player2Ball;

	int AniGrid[NUM_SQUARES_X][NUM_SQUARES_Y];

  friend class OptionDialog;
};


/*--------------------------------------------------------------------------*/
class OptionDialog : public PegDialog {
  public:
    OptionDialog(char* name);
	OptionDialog(PegRect Rect, const PEGCHAR *name);
	
	 ~OptionDialog();
	 BOOL  CanClose();
	 void  SetupWindow();
	 void  Help();

     SIGNED Message(const PegMessage &Mesg);

	 PegRadioButton *p1rbh;
	 PegRadioButton *p1rba1;
	 PegRadioButton *p1rba2;
	 PegRadioButton *p1rba3;

	 PegRadioButton *p2rbh;
	 PegRadioButton *p2rba1;
	 PegRadioButton *p2rba2;
	 PegRadioButton *p2rba3;
	 PegCheckBox *pJump;
	 PegCheckBox *p1AlphaBeta;
	 PegCheckBox *p1IterativeDeepening;
	 PegCheckBox *p2AlphaBeta;
	 PegCheckBox *p2IterativeDeepening;
	 PegCheckBox *pBallMorph;
	 PegCheckBox *pTrace;
	 PegString	*p1Taper1;
	 PegString	*p1Taper2;
	 PegString	*p1Tapern;
	 PegString	*p2Taper1;
	 PegString	*p2Taper2;
	 PegString	*p2Tapern;
	 PegString	*p1SearchDepth;
	 PegString	*p2SearchDepth;
	 PegString	*pNumGames;
	 PegString  *p1Iter;
	 PegString  *p2Iter;

	GameWindow *pGame;


};

class BolasHelpScreen	: public PegDialog
{
  public:
	 BolasHelpScreen(PegRect Rect);
	 ~BolasHelpScreen();

     SIGNED Message(const PegMessage &Mesg);
};

class BolasAboutScreen	: public PegDialog
{
  public:
	 BolasAboutScreen(PegRect Rect);
	 ~BolasAboutScreen();

     SIGNED Message(const PegMessage &Mesg);
};



#endif   // BOLAS_H
