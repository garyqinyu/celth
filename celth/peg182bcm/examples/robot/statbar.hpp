/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// statbar.hpp
//
// Copyright (c) 1997-2002, Swell Software, Inc.
//
// Notes:
//
//	The status bar is just a window with a bunch of PegPrompt objects on it.
//	Also added several public functions to allow run-time modification of
//	what is displayed in each region of the status bar.
//
//  ClockPrompt, also contained in this file, is a custom class derived from
//	PegPrompt to display TIME and TIME REMAINING fields.
//
//	Automatically maintains data and time.
//
//	Time Remaining is a ClockPrompt that becomes a child of whichever
//  prompt needs to have time remaining displayed.
//
//  Each prompt could 'go get' the information for itself, but I didn't know
//	if this would be avaiable. For now it is set up such that the status bar
//	parent window modifies the status bar fields through accessor functions.
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#define CLOCKTYPE_DAYTIME 1
#define CLOCKTYPE_COUNTDOWN 2

class ClockPrompt : public PegPrompt
{
	public:
		ClockPrompt(PegRect &Rect);

		ClockPrompt(PegRect &Rect, LONG lInitial);
		SIGNED Message(const PegMessage &Mesg);

	private:
		void UpdateTime(void);
		UCHAR muClockType;
		LONG mlOldTime;

};

class StatusBar : public PegThing
{
	public:
		StatusBar(const PegRect &Rect);
		void Draw(void);
		void StatusSet(char *Text);

	private:

		PegPrompt *mpSysStatus;
		PegPrompt *mpSysDetails;
		PegPrompt *mpDatePrompt;
        PegPrompt *mpControlSrc;
		ClockPrompt *mpTimePrompt;
        PegPrompt *mpModePrompt;
};


