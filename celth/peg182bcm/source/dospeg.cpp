/*--------------------------------------------------------------------------*/
// dospeg.cpp- main startup module for running PEG as a standard DOS
//    executable. This module replaces PegTask for running with DOS, since
//  DOS is natively single-threaded anyway.
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
// This module creates the fundamental PEG objects (PegScreen,
// PegPresentation, PegMessageQueue), calls PegAppInitialize, and then
// PegPresentationManager::Execute(), which runs forever.
//
//
/*--------------------------------------------------------------------------*/


#include <stdlib.h>
#include <dos.h>
#include <bios.h>

#define __COLORS        // don't want conio colors
#include <conio.h>      //  for kbhit
#include <stdio.h>

#ifdef __WATCOMC__
#include "time.h"
#endif

#include "peg.hpp"

void PegAppInitialize(PegPresentationManager *);
void PollMouse(void);
void PollTime(void);
void PollKeyboard(void);

extern "C" {

int main(int, char **)
{
    // create the screen interface object:
    PegScreen *pScreen = CreatePegScreen();
    PegThing::SetScreenPtr(pScreen);
    
    // create the PEG message Queue:
    
    PegMessageQueue *pMsgQueue = new PegMessageQueue();
    PegThing::SetMessageQueuePtr(pMsgQueue);
    
    // create the screen manager:
    
    PegRect Rect;
    Rect.Set(0, 0, pScreen->GetXRes() - 1, pScreen->GetYRes() - 1);
    PegPresentationManager *pPresentation = new PegPresentationManager(Rect);
    PegThing::SetPresentationManagerPtr(pPresentation);
    pScreen->GenerateViewportList(pPresentation);

    PegAppInitialize(pPresentation);    
    pPresentation->Execute();

    // restore the timer interrupt:

    /*
    outp(0x43, 0x36);
    outp(0x40,0xff);
    outp(0x40,0xff);
    */

    delete pPresentation;
    delete pMsgQueue;
    delete pScreen;

    return 0;
}

}


/*--------------------------------------------------------------------------*/
// PegIdleFunction-
// 
// This function gets called by PegPresentationManager when there is nothing
// left for PEG to do. When running with DOS we just poll the mouse, the
// time, and the keyboard.
/*--------------------------------------------------------------------------*/

void PegIdleFunction(void)
{
    PollTime();

    #ifdef PEG_MOUSE_SUPPORT
    PollMouse();
    #endif

    #ifndef __WATCOMC__
    PollKeyboard();
    #endif
}


static unsigned long OldTime;

#if defined (_MSC_VER)
#include "time.h"
#endif

/*--------------------------------------------------------------------------*/
void PollTime(void)
{  
   #if defined(_MSC_VER) || defined(__WATCOMC__)
   
    unsigned long tens = clock();    
    
   #else
   
    struct time t;
    gettime(&t);
    unsigned long tens = t.ti_hund;
    
   #endif
   
    PegThing *pt = NULL;


    if (tens != OldTime)
    {
        OldTime = tens;
        pt->MessageQueue()->TimerTick();
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// PollKeyboard- Read the keyboard using the BIOS. This version if used for
//   standalone DOS operation only. If you are running on an embedded target,
//   you will want to have an interrupt handler for your input device(s) that
//   directly sends PM_KEY and PM_KEY_RELEASE messages to PEG.
//
// Notes:
//
//   For best portability, this version does not directly hook the
//   keyboard interrupt. It uses (oh no!) the BIOS services. There are
//   limitations using the BIOS in terms of extended key scan code
//   information. For example, we must simulate key-release messages because
//   the BIOS does not report when a key is released, only when pressed. There
//   are other limitations as well. 
//
//   For embedded x86 targets using an RTOS, we always directly service the
//   keyboard interrupt, and use the lookup table found in the file
//   \peg\include\pegkeys.hpp to translate raw scan codes in to PM_KEY and
//   PM_KEY_RELEASE messages. To repeat, this version of the keyboard driver
//   is only used for standalone DOS operation.
//
//   For embedded targets using other types of key input, like a small keypad
//   matrix, you need to create a driver that sends PM_KEY and PM_KEY_RELEASE
//   message to PEG. How your keys are interpreted is up to you, i.e. you 
//   can send any key values you want to PEG. You might want to create a
//   translation table similar to the one in \peg\include\pegkeys.hpp to make
//   it easy to convert your scanned input value into a PEG key value.
//
//   If you are using only a touch screen or "soft keys", you don't need to
//   send ANY keyboard messages to PEG. You should also turn off
//   PEG_KEYBOARD_SUPPORT in \peg\include\peg.hpp to save a little code space.
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifdef __WATCOMC__

void PollKeyboard(void)
{
}

#else

int OldKeyVal = 0;
long OldStatus = 0;
long OldKeyTime;

#define INCLUDE_PEG_KEYTABLE
#include "pegkeys.hpp"
#undef INCLUDE_PEG_KEYTABLE

#define DOS_CTRL_TAB 0x94
#define DOS_CTRL_F4  0x61

void PollKeyboard(void)
{
    int ScanCode;
    int CtrlCode;
    int CtrlWord;
    int KeyVal;
    union REGS regs;
    PegMessage NewMessage(NULL, PM_KEY);
    PegThing *pt = NULL;

    #if defined(_MSC_VER)
    if (_kbhit())
    {
    #else
    if (kbhit())
    {
    #endif
        regs.h.ah = 0x10;
        int86 (0x16, &regs, &regs);
        ScanCode = regs.h.ah;
        regs.h.ah = 2;
        int86 (0x16, &regs, &regs);
        CtrlCode = regs.h.al;

        CtrlWord = 0;        
        if (CtrlCode & 0x04)
        {
            CtrlWord = KF_CTRL;
        }
        if (CtrlCode & 0x03)
        {
            CtrlWord |= KF_SHIFT;
        }

        KeyVal = 0;
        if (ScanCode < AT_KEYTABLE_LEN)
        {
            if (CtrlWord & KF_SHIFT)      // shift key pressed?
            {
                if (CtrlCode & 0x40)
                {
                    KeyVal = ATKeyTable[ScanCode][0];
                }
                else
                {
                    KeyVal = ATKeyTable[ScanCode][1];
                }
            }
            else
            {
                if (CtrlCode & 0x40)
                {
                    KeyVal = ATKeyTable[ScanCode][1];
                }
                else
                {
                    KeyVal = ATKeyTable[ScanCode][0];
                }
            }
        }
        else
        {
            if (ScanCode == DOS_CTRL_F4)
            {
                KeyVal = PK_F4;
                CtrlWord = KF_CTRL;
            }
        }

        if (KeyVal)
        {
            // convert the F4 key into a <CTRL-TAB> key, since without hooking
            // the keyboard interrupt directly it is impossible to receive
            // a CTRL-TAB key under DOS.

            if (KeyVal == PK_F4 && !CtrlWord)
            {
                KeyVal = PK_TAB;
                CtrlWord = KF_CTRL;
            }
            NewMessage.wType = PM_KEY;

            if (CtrlWord & KF_CTRL)
            {
                switch(KeyVal)
                {
                case 'x':
                case 'X':
                    NewMessage.wType = PM_CUT;
                    break;

                case 'c':
                case 'C':
                    NewMessage.wType = PM_COPY;
                    break;

                case 'v':
                case 'V':
                    NewMessage.wType = PM_PASTE;
                    break;
                }
            }
            NewMessage.iData = KeyVal;
            NewMessage.lData = CtrlWord;
            pt->MessageQueue()->Push(NewMessage);
            OldKeyVal = KeyVal;     // keep track of last key sent
            OldStatus = CtrlWord;   // and its flags
            OldKeyTime = OldTime;   // and when it was sent
        }
    }
    else
    {
        if (OldKeyVal && (OldTime - OldKeyTime > 10))
        {
            NewMessage.wType = PM_KEY_RELEASE;
            NewMessage.lData = OldStatus;
            NewMessage.iData = OldKeyVal;
            OldKeyVal = 0;
            pt->MessageQueue()->Push(NewMessage);
        }
    }
}

#endif

/*--------------------------------------------------------------------------*/
#define MOUSEXMIN 2
#define MOUSEYMIN 2

#ifdef SUPERVGA
#define MOUSEXMAX 1022
#define MOUSEYMAX 766
#elif defined(_PEGVGASCRN_)
#define MOUSEXMAX 638
#define MOUSEYMAX 478
#else
#define MOUSEXMAX 318
#define MOUSEYMAX 238
#endif

int xPos = MOUSEXMAX / 2;
int yPos = MOUSEYMAX / 2;
int oldx = -1;
int oldy = -1;
int OldButtons;


void PollMouse(void)
{
    PegMessage NewMessage;
    union REGS regs;
    PegThing *pt = NULL;
    int xDelta = 0, yDelta = 0;
    regs.x.ax = 0x03;
    int86 (0x33, &regs, &regs);

    int Buttons = regs.x.bx;

    regs.x.ax = 0x0b;
    int86 (0x33, &regs, &regs);

    xDelta = regs.x.cx;
    yDelta = regs.x.dx;

    xPos += xDelta;
    yPos += yDelta;

    if (xPos < MOUSEXMIN)
    {
        xPos = MOUSEXMIN;
    }
    if (xPos > MOUSEXMAX)
    {
        xPos = MOUSEXMAX;
    }
    if (yPos < MOUSEYMIN)
    {
        yPos = MOUSEYMIN;
    }
    if (yPos > MOUSEYMAX)
    {
        yPos = MOUSEYMAX;
    }

    if (OldButtons != Buttons)
    {
        if (Buttons & 0x01)
        {
            NewMessage.wType = PM_LBUTTONDOWN;
        }
        else
        {
            NewMessage.wType = PM_LBUTTONUP;
        }
        NewMessage.Point.x = xPos;
        NewMessage.Point.y = yPos;
        NewMessage.iData = Buttons;
        pt->MessageQueue()->Push(NewMessage);
    }
    else
    {
        if (oldx != xPos || oldy != yPos)
        {
            NewMessage.wType = PM_POINTER_MOVE;
            NewMessage.Point.x = xPos;
            NewMessage.Point.y = yPos;
            NewMessage.iData = Buttons;
            pt->MessageQueue()->Push(NewMessage);
        }
    }
    oldx = xPos;
    oldy = yPos;
    OldButtons = Buttons;
}

