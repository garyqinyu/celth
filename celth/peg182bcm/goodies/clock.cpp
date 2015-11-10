/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// clockprompt - A little prompt class to display the time of day.
//
// Author: Kenneth G. Maxwell
//
//
// Copyright (c) 1997-1998 Swell Software 
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

#include "peg.hpp"
#include "clock.hpp"
#include "time.h"

/*--------------------------------------------------------------------------*/

ClockPrompt::ClockPrompt(PegRect &Rect, WORD wStyle) :
    PegPrompt(Rect, "", 0, wStyle)
{
    // put the font you would like to use here:
    mpFont = &MenuFont;
}

/*--------------------------------------------------------------------------*/
// Override the message handler to do some custom operations:
/*--------------------------------------------------------------------------*/
SIGNED ClockPrompt::Message(const PegMessage &Mesg)
{
    PegMessage NewMessage;

    switch(Mesg.wType)
    {
    case PM_SHOW:
        UpdateTimeDisplay();        // set initial value before I am visible
        PegPrompt::Message(Mesg);
        SetTimer(1, ONE_SECOND / 2, ONE_SECOND / 2);
        break;

    case PM_TIMER:
        UpdateTimeDisplay();        // check time 2 times a second
        break;

    case PM_HIDE:
        KillTimer(1);
        PegPrompt::Message(Mesg);
        break;
                            
    default:
        PegPrompt::Message(Mesg);
        break;
    }
    return 0;
}

/*--------------------------------------------------------------------------*/
// UpdateTimeDisplay: create new time display string without using sprintf.
// sprintf causes problems for many systems.
/*--------------------------------------------------------------------------*/
void ClockPrompt::UpdateTimeDisplay(void)
{
	char cTimeBuff[20];
    char *pPut = cTimeBuff;

	struct tm *newtime;
    time_t long_time;

	time( &long_time );                /* Get time as long integer. */

	if((LONG) long_time != mlOldTime)
	{
		mlOldTime = (LONG) long_time;
	    char am_pm[] = " AM";
	    newtime = localtime( &long_time ); /* Convert to local time. */

	   	if( newtime->tm_hour > 12 )        /* Set up extension. */
        {
            strcpy( am_pm, " PM" );
            newtime->tm_hour -= 12;
        }
	   	if( newtime->tm_hour == 0 )        /*Set hour to 12 if midnight. */
        	    newtime->tm_hour = 12;

        if (newtime->tm_hour < 10)
        {
            *pPut++ = '0';
        }

        itoa(newtime->tm_hour, pPut, 10);
        pPut = &cTimeBuff[2];
        *pPut++ = ':';

        if (newtime->tm_min < 10)
        {
            *pPut++ = '0';
        }
        itoa(newtime->tm_min, pPut, 10);
        pPut = &cTimeBuff[5];
        *pPut++ = ':';

        if (newtime->tm_sec < 10)
        {
            *pPut++ = '0';
        }
        itoa(newtime->tm_sec, pPut, 10);
        strcat(pPut, am_pm);

		DataSet(cTimeBuff);     // member function of PegTextThing

        if (StatusIs(PSF_VISIBLE))
        {
    		Draw();
        }
    }
}

