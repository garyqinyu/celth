/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// StatusBar: Custom popup status bar created for example Robot application.
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
//	Automatically maintains data and time using Borland time functions. This
//  would need to be modifed to build and run this demo using other tools.
//
//	Time Remaining is a ClockPrompt that becomes a child of whichever
//  prompt needs to have time remaining displayed.
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include "peg.hpp"
#ifndef PEGSMX
#include "stdio.h"
#include "time.h"
#endif
#include "statbar.hpp"

char *Months[12] =
{
"JAN",
"FEB",
"MAR",
"APR",
"MAY",
"JUN",
"JUL",
"AUG",
"SEP",
"OCT",
"NOV",
"DEC"
};

/*--------------------------------------------------------------------------*/
// StatusBar- A custom status bar class, placed at the top of the Robot
// window. Just a PegThing with a set of stock objects added to it.
/*--------------------------------------------------------------------------*/
StatusBar::StatusBar(const PegRect &Rect) : PegThing(Rect, 0, FF_RAISED)
{
	AddStatus(PSF_VIEWPORT);
    InitClient();

	// get an initial date string:

#ifndef PEGSMX
	char DateBuff[20];
    struct tm *newtime;	
    time_t long_time;
    time( &long_time );
    newtime = localtime( &long_time ); /* Convert to local time. */
	sprintf(DateBuff, "%02d %s %d", newtime->tm_mday, Months[newtime->tm_mon],
		newtime->tm_year + 1900);
#endif

	// add the system status prompt:

	PegRect PromptRect;
	PromptRect.Set(mClient.wLeft + 2, mClient.wTop + 3,
        mClient.wLeft + 140, mClient.wTop + 22);
	mpSysStatus = new PegPrompt(PromptRect, "Idle", 0, FF_RECESSED|TJ_CENTER|TT_COPY);
	mpSysStatus->SetColor(PCI_NORMAL, BLACK);
	mpSysStatus->SetColor(PCI_NTEXT, GREEN);
	mpSysStatus->SetFont(&SysFont);
	Add(mpSysStatus);

    PromptRect.wLeft = PromptRect.wRight + 4;
    PromptRect.wRight = PromptRect.wLeft + 95;

   #ifndef PEGSMX
	mpDatePrompt = new PegPrompt(PromptRect, DateBuff, 0, FF_RECESSED|TJ_CENTER|TT_COPY);
   #else
	mpDatePrompt = new PegPrompt(PromptRect, "Jan 1, 2000", 0, FF_RECESSED|TJ_CENTER|TT_COPY);
   #endif
	mpDatePrompt->SetColor(PCI_NORMAL, BLACK);
	mpDatePrompt->SetColor(PCI_NTEXT, CYAN);
	mpDatePrompt->SetFont(&SysFont);
	Add(mpDatePrompt);

    PromptRect.wLeft = PromptRect.wRight + 4;
    PromptRect.wRight = PromptRect.wLeft + 85;
	mpTimePrompt = new ClockPrompt(PromptRect);
	Add(mpTimePrompt);

    PromptRect.wLeft = PromptRect.wRight + 4;
    PromptRect.wRight = PromptRect.wLeft + 60;
    Add(new PegTextButton(PromptRect, "Exit", IDB_CLOSE));
    
    // add the system details prompt:

	PromptRect.Set(mClient.wLeft + 2, mClient.wTop + 25, mClient.wLeft + 190,
        mClient.wTop + 45);
	mpSysDetails = new PegPrompt(PromptRect, 
		"Proximity Probe Distance", 0, FF_RECESSED|TJ_LEFT|TT_COPY);
	mpSysDetails->SetColor(PCI_NORMAL, BLACK);
	mpSysDetails->SetColor(PCI_NTEXT, RED);
	mpSysDetails->SetFont(&SysFont);
	Add(mpSysDetails);
}


/*--------------------------------------------------------------------------*/
void StatusBar::Draw(void)
{
	BeginDraw();
	StandardBorder(LIGHTGRAY);
	DrawChildren();
    PegColor TextColor(BLACK, BLACK, CF_NONE);
    PegPoint Put;
    Put.y = mClient.wBottom - 20;
    Put.x = mClient.wLeft + 210;
    DrawText(Put, "Speed:", TextColor, &SysFont);
	EndDraw();
}

/*--------------------------------------------------------------------------*/
void StatusBar::StatusSet(char *Text)
{
	mpSysStatus->DataSet(Text);
	mpSysStatus->Draw();	
}


/*--------------------------------------------------------------------------*/
ClockPrompt::ClockPrompt(PegRect &Rect) :
	PegPrompt(Rect, "", 0, FF_RECESSED|TJ_CENTER)
{
	mlOldTime = -1;
	muClockType = CLOCKTYPE_DAYTIME;
	SetColor(PCI_NORMAL, BLACK);
	SetColor(PCI_NTEXT, CYAN);
	SetFont(&SysFont);
}

/*--------------------------------------------------------------------------*/
ClockPrompt::ClockPrompt(PegRect &Rect, LONG lInitial) :
	PegPrompt(Rect, "", 0, FF_NONE|TJ_CENTER)
{
	mlOldTime = lInitial;
	muClockType = CLOCKTYPE_COUNTDOWN;
	SetColor(PCI_NORMAL, BLACK);
	SetColor(PCI_NTEXT, CYAN);
	SetFont(&SysFont);
}


/*--------------------------------------------------------------------------*/
SIGNED ClockPrompt::Message(const PegMessage &Mesg)
{
	switch (Mesg.wType)
	{
	case PM_SHOW:
		UpdateTime();
		PegPrompt::Message(Mesg);

		if (muClockType == CLOCKTYPE_COUNTDOWN)
		{
			SetColor(PCI_NTEXT, Parent()->muColors[PCI_NTEXT]);
		}
		SetTimer(1, ONE_SECOND + 7, ONE_SECOND);
		break;

	case PM_TIMER:
		UpdateTime();
		break;

	case PM_HIDE:
		PegPrompt::Message(Mesg);
		KillTimer(1);
		break;

	default:
		PegPrompt::Message(Mesg);
		break;
	}
	return 1;
}


/*--------------------------------------------------------------------------*/
void ClockPrompt::UpdateTime(void)
{
#ifndef PEGSMX
	char cTimeBuff[20];
	
	if (muClockType == CLOCKTYPE_DAYTIME)
	{
		struct tm *newtime;
    	time_t long_time;

	    time( &long_time );                /* Get time as long integer. */

		if((LONG) long_time != mlOldTime)
		{
			mlOldTime = (LONG) long_time;

		    char am_pm[] = "AM";
		    newtime = localtime( &long_time ); /* Convert to local time. */

	    	if( newtime->tm_hour > 12 )        /* Set up extension. */
    	    	    strcpy( am_pm, "PM" );
		    if( newtime->tm_hour > 12 )        /* Convert from 24-hour */
    		        newtime->tm_hour -= 12;    /*   to 12-hour clock.  */
	    	if( newtime->tm_hour == 0 )        /*Set hour to 12 if midnight. */
    	    	    newtime->tm_hour = 12;

			sprintf(cTimeBuff, "%02d:%02d ", newtime->tm_hour, newtime->tm_min);
			strcat(cTimeBuff, am_pm);
			DataSet(cTimeBuff);
			Draw();
		}
	}
	else
	{
		// here for a countdown timer:

		if (!mlOldTime)
		{
			Destroy(this);
		}
		else
		{
			mlOldTime--;
			sprintf(cTimeBuff, "%02d:%02d", mlOldTime / 60, mlOldTime % 60);
			DataSet(cTimeBuff);
			Draw();
		}
	}
#endif
}


