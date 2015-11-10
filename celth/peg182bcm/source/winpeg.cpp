/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// winpeg.cpp - PegTask implementation for running PEG as a 
//    Win32 application. 
//
// Author: Kenneth G. Maxwell
//         Jim DeLisle
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
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include "peg.hpp"
//#include "winpeg.h"
#define INCLUDE_PEG_KEYTABLE
#include "pegkeys.hpp"
#undef INCLUDE_PEG_KEYTABLE

#if defined(_PEGWINSCREEN_)      // normal Win32 operation?
#define PEGSCREEN PegWinScreen
#elif defined(_L24SCREEN_)
#define PEGSCREEN L24Screen
#elif defined(_L16SCREEN_)       // TESTING LINEAR-16 DRIVER??
#define PEGSCREEN L16Screen      
#elif defined(_L8SCREEN_)        // TESTING LINEAR-8 DRIVER??
#define PEGSCREEN L8Screen
#elif defined(_L4SCREEN_)        // TESTING LINEAR-4 DRIVER??
#define PEGSCREEN L4Screen
#elif defined(_L2SCREEN_)        // TESTING LINEAR-2 DRIVER??
#define PEGSCREEN L2Screen
#elif defined(_MONOSCREEN_)      // TESTING MONO DRIVER?
#define PEGSCREEN MonoScreen
#elif defined(_PROMONOSCREEN_)
#define PEGSCREEN ProMonoScreen  // TESTING PROFILE-MODE MONOCHROME?
#elif defined(_PRO2SCREEN_)
#define PEGSCREEN Pro2Screen
#elif defined(_PRO4SCREEN_)
#define PEGSCREEN Pro4Screen
#elif defined(_PRO8SCREEN_)
#define PEGSCREEN Pro8Screen
#elif defined(_SED1356SCRN_)     // Running SDU1356 under Windows?
#define PEGSCREEN SED1356Screen
#elif defined(_1374SCR4_)
#define PEGSCREEN SED1374Screen4
#elif defined(_1376SCREEN8_)
#define PEGSCREEN SED1376Screen8
#elif defined(_RAYSCREEN_)
#define PEGSCREEN RayScreen
#elif defined(_FLIPMONOSCREEN_
#define PEGSCREEN FlipMonoScreen
#else
#error ** Error: no screen driver included **
#endif

#if defined(PEG_MULTITHREAD)
/*---------------------------------------------------------------------------*/
// Multithread/Message queue 
/*---------------------------------------------------------------------------*/
static PegQueue*               gpPegFreeMessages;
static PegQueue*               gpPegMesgInput;

DWORD                   PegTaskPtr;
CRITICAL_SECTION        PresentationCriticalSection;
CRITICAL_SECTION        PegTimerCriticalSection;
CRITICAL_SECTION        PegMessageCriticalSection;

static HANDLE ghDevWinSem;
static HANDLE ghWinMainSem;
static HINSTANCE ghInstance;

static PegPresentationManager* gpPresentation;
static PegMessageQueue*        gpMsgQueue;
static PegScreen*              gpScreen;

/*---------------------------------------------------------------------------*/
// Multithread prototypes
/*---------------------------------------------------------------------------*/
DWORD WINAPI Win32MesgThread(LPVOID lpData);

#endif  // PEG_MULTITHREAD

/*---------------------------------------------------------------------------*/
// Define this if we are running to build a script for an auto running
// application.
/*---------------------------------------------------------------------------*/
//#define RECORD_AUTO_SCRIPT 

#ifdef RECORD_AUTO_SCRIPT
#include "stdio.h"
#include "time.h"
void PrintMessage(const PegMessage& Mesg,  char *pType);
FILE *pOutFile = NULL;
#endif

/*---------------------------------------------------------------------------*/
// The input command line
/*---------------------------------------------------------------------------*/
char *gpPegCmdLine = NULL;

/*---------------------------------------------------------------------------*/
// Prototypes
/*---------------------------------------------------------------------------*/
extern void PegAppInitialize(PegPresentationManager *);
long FAR WINAPI WndProc(HWND, UINT, UINT, LONG);

/*---------------------------------------------------------------------------*/
// Handle to the development window
/*---------------------------------------------------------------------------*/
static HWND ghDevWnd;
const static WORD gwAboutPegID = 0x0f00;

#if defined(PEG_MULTITHREAD)
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
		   		   LPSTR pCmd, int nCmdShow)
{
    PegTaskPtr = GetCurrentThreadId();

    // Initialize sync objects
    InitializeCriticalSection(&PresentationCriticalSection);
    InitializeCriticalSection(&PegTimerCriticalSection);
    InitializeCriticalSection(&PegMessageCriticalSection);
    ghWinMainSem = CreateSemaphore(NULL, 0, 1, NULL);

    // Create the Development Window/Win32 Message pump interface
    ghDevWinSem = CreateSemaphore(NULL, 0, 1, NULL);
    ghInstance = hInstance;
    DWORD dwThreadParm;
    HANDLE tMsgHandle = CreateThread(NULL, 0, 
                                     (LPTHREAD_START_ROUTINE)Win32MesgThread,
                                     NULL, 0, &dwThreadParm);
    WaitForSingleObject(ghDevWinSem, INFINITE);
    
	
    // Save any command line parameters. We don't use them, but the
    // application may want them:
    if (pCmd)
    {
        if (*pCmd)
        {
            gpPegCmdLine = new char[strlen(pCmd) + 1];
            strcpy(gpPegCmdLine, pCmd);
        }
    }

    // create the screen interface object:
    PegRect Rect;
    Rect.Set(0, 0, PEG_VIRTUAL_XSIZE - 1, PEG_VIRTUAL_YSIZE - 1);

   #ifndef _1376SCREEN8_
    gpScreen = new PEGSCREEN(ghDevWnd, Rect);
   #else
    gpScreen = CreatePegScreen();
   #endif

    PegThing::SetScreenPtr(gpScreen); 

    // create the PEG message Queue:
    gpMsgQueue = new PegMessageQueue();
    PegThing::SetMessageQueuePtr(gpMsgQueue);

    // create the screen manager:
    gpPresentation = new PegPresentationManager(Rect);
    PegThing::SetPresentationManagerPtr(gpPresentation);

   #ifdef PEG_FULL_CLIPPING
    gpScreen->GenerateViewportList(gpPresentation);
   #endif

   #ifdef RECORD_AUTO_SCRIPT
    pOutFile = fopen("pegas.txt", "w");
   #endif  // RECORD_AUTO_SCRIPT

    PegAppInitialize(gpPresentation);

    // re-load the screen, PresentationManager, and MessageQueue pointers
    // in case the application changed them (WindowBuilder does):
    PegThing *pTest = NULL;
    gpPresentation = pTest->Presentation();
    gpScreen = pTest->Screen();

	gpPresentation->Invalidate(gpPresentation->mReal);
	
    // Release the Win32MesgThread
    ReleaseSemaphore(ghWinMainSem, 1, NULL);

    ShowWindow(ghDevWnd, nCmdShow);
	UpdateWindow(ghDevWnd);
    //gpScreen->SetPointerType(PPT_NORMAL);
	
    gpPresentation->Execute();        // do message processing

	gpPresentation = pTest->Presentation();
	gpScreen = pTest->Screen();
	gpMsgQueue = pTest->MessageQueue();

	delete gpPresentation;
	delete gpMsgQueue;
    delete gpScreen;

    if (gpPegCmdLine)
    {
        delete gpPegCmdLine;
    }

   #ifdef RECORD_AUTO_SCRIPT
    if (pOutFile)
    {
        fclose(pOutFile);
    }
   #endif  

    // Clean up the sync objects
    CloseHandle(ghDevWinSem);
    CloseHandle(ghWinMainSem);
    DeleteCriticalSection(&PresentationCriticalSection);
    DeleteCriticalSection(&PegTimerCriticalSection);
    DeleteCriticalSection(&PegMessageCriticalSection);

    return(0);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
DWORD WINAPI Win32MesgThread(LPVOID lpData)
{
    static char szAppName[] = "Peg Win32MT Application";
    WNDCLASS wndclass;

    wndclass.style = CS_HREDRAW|CS_VREDRAW;
    wndclass.lpfnWndProc = WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = ghInstance;
    wndclass.hIcon = LoadIcon(NULL, MAKEINTRESOURCE(IDI_WINLOGO));
    wndclass.hCursor = NULL;
    wndclass.hbrBackground = (HBRUSH)(GetStockObject(WHITE_BRUSH));
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = szAppName;

    RegisterClass(&wndclass);

    ghDevWnd = CreateWindow(szAppName, "PEG Win32 MT Development Window",
         WS_VISIBLE|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_SYSMENU,
        20, 20, 
        PEG_VIRTUAL_XSIZE + 8,
        PEG_VIRTUAL_YSIZE + 25, 
        NULL, NULL, ghInstance, NULL);

    // Add an "About PEG" to the system menu
    HMENU hMenu = GetSystemMenu(ghDevWnd, FALSE);
    AppendMenu(hMenu, MFT_SEPARATOR, 0, NULL);
    AppendMenu(hMenu, MFT_STRING, gwAboutPegID, "About PEG...");

    //gpScreen->SetPointerType(PPT_NORMAL);
    SetCursor(LoadCursor(NULL, IDC_ARROW));
    SetTimer(ghDevWnd, 1, 50, NULL);

	static PegThing *SomeThing = NULL;
	static MSG msg;
    static int status;

    // Allow WinMain to continue
    ReleaseSemaphore(ghDevWinSem, 1, NULL);
    // Wait for WinMain to build all of the PEG objects before continuing.
    WaitForSingleObject(ghWinMainSem, INFINITE);

    while(1)
    {
        status = GetMessage(&msg, NULL, 0, 0);
        if (status)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            PegMessage NewMessage;
            NewMessage.wType = PM_EXIT;
            NewMessage.pTarget = SomeThing->Presentation();
            NewMessage.pSource = NULL;
            SomeThing->MessageQueue()->Push(NewMessage);
            ExitThread(0);
        }
    }
}

#else   // PEG_MULTITHREAD
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
		   		   LPSTR pCmd, int nCmdShow)
{
    static char szAppName[] = "Peg Application";
    HWND hwnd;

    WNDCLASS wndclass;

    // Save any command line parameters. We don't use them, but the
    // application may want them:

    if (pCmd)
    {
        if (*pCmd)
        {
            gpPegCmdLine = new char[strlen(pCmd) + 1];
            strcpy(gpPegCmdLine, pCmd);
        }
    }

    if (!hPrevInstance)
    {
        wndclass.style = CS_HREDRAW|CS_VREDRAW;
        wndclass.lpfnWndProc = WndProc;
        wndclass.cbClsExtra = 0;
        wndclass.cbWndExtra = 0;
        wndclass.hInstance = hInstance;
        wndclass.hIcon = LoadIcon(NULL, MAKEINTRESOURCE(IDI_WINLOGO));
        //wndclass.hIcon = LoadIcon(NULL, "pegapp.ico");
        wndclass.hCursor = NULL;
        wndclass.hbrBackground = (HBRUSH)(GetStockObject(WHITE_BRUSH));
        wndclass.lpszMenuName = NULL;
        wndclass.lpszClassName = szAppName;
        RegisterClass(&wndclass);
    }

    hwnd = CreateWindow(szAppName, "PEG Development Window",
         WS_VISIBLE|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_SYSMENU|WS_SIZEBOX,
        20, 20, 
        PEG_VIRTUAL_XSIZE + 8,
        PEG_VIRTUAL_YSIZE + 25, NULL, NULL, hInstance, NULL);

    // create the screen interface object:

    PegRect Rect;
    Rect.Set(0, 0, PEG_VIRTUAL_XSIZE - 1, PEG_VIRTUAL_YSIZE - 1);

   #ifndef _1376SCREEN8_
    PegScreen *pScreen = new PEGSCREEN(hwnd, Rect);
   #else
    PegScreen *pScreen = CreatePegScreen();
   #endif

    PegThing::SetScreenPtr(pScreen);
 
   #ifdef PEG_PRINTER_SUPPORT

    // create the printer interface object:
    PegPrint *pPrint = CreatePegPrint( Rect );
    PegThing::SetPrintPtr(pPrint);

   #endif

    // create the PEG message Queue:

    PegMessageQueue *pMsgQueue = new PegMessageQueue();
    PegThing::SetMessageQueuePtr(pMsgQueue);

    // create the screen manager:

    PegPresentationManager *pPresentation = new PegPresentationManager(Rect);
    PegThing::SetPresentationManagerPtr(pPresentation);

   #ifdef PEG_FULL_CLIPPING
    pScreen->GenerateViewportList(pPresentation);
   #endif

    PegAppInitialize(pPresentation);

    // re-load the screen, PresentationManager, and MessageQueue pointers
    // in case the application changed them (WindowBuilder does):

    PegThing *pTest = NULL;
    pPresentation = pTest->Presentation();
    pScreen = pTest->Screen();

	pPresentation->Invalidate(pPresentation->mReal);

	SetTimer(hwnd, 1, 50, NULL);
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	pScreen->SetPointerType(PPT_NORMAL);

	pPresentation->Execute();        // do message processing

	pPresentation = pTest->Presentation();
	pScreen = pTest->Screen();
	pMsgQueue = pTest->MessageQueue();

	delete pPresentation;
	delete pMsgQueue;
    delete pScreen;

    if (gpPegCmdLine)
    {
        delete gpPegCmdLine;
    }

    return(0);
}


/*--------------------------------------------------------------------------*/
// PegIdleFunction- 
//
// This function gets called by PegPresentationManager when there is nothing
// left for PEG to do. For running with Windows, we return to the Windows
// message loop until a new Windows message arrives.
/*--------------------------------------------------------------------------*/
void PegIdleFunction(void)
{
	 PegThing *SomeThing = NULL;
	 MSG msg;
	 int status = GetMessage(&msg, NULL, 0, 0);
	 if (status)
	 {
		  TranslateMessage(&msg);
		  DispatchMessage(&msg);
	 }
	 else
	 {
		  PegMessage NewMessage;
		  NewMessage.wType = PM_EXIT;
		  NewMessage.pTarget = SomeThing->Presentation();
		  NewMessage.pSource = NULL;
		  SomeThing->MessageQueue()->Push(NewMessage);
	 }
}

#endif  // PEG_MULTITHREAD

/*--------------------------------------------------------------------------*/
static struct {
   WORD VK, PK;
} VK_to_PK[] = {
// only non ASCII chars should go here
 { VK_DELETE     , PK_DELETE     },
 { VK_INSERT     , PK_INSERT     },
 { VK_HOME       , PK_HOME       },
 { VK_END        , PK_END        },
 { VK_PRIOR      , PK_PGUP       },
 { VK_NEXT       , PK_PGDN       },
 { VK_UP         , PK_LNUP       },
 { VK_DOWN       , PK_LNDN       },
 { VK_LEFT       , PK_LEFT       },
 { VK_RIGHT      , PK_RIGHT      },
 { VK_MENU       , PK_ALT        },
 { VK_CONTROL    , PK_CTRL       },
 { VK_SHIFT      , PK_SHIFT      },
 { VK_CAPITAL    , PK_CAPS       },
 { VK_NUMLOCK    , PK_NUMLOCK    },
 { VK_SCROLL     , PK_SCROLLLOCK },
 { VK_F1         , PK_F1         },
 { VK_F2         , PK_F2         },
 { VK_F3         , PK_F3         },
 { VK_F4         , PK_F4         },
 { VK_F5         , PK_F5         },
 { VK_F6         , PK_F6         },
 { VK_F7         , PK_F7         },
 { VK_F8         , PK_F8         },
 { VK_F9         , PK_F9         },
 { VK_F10        , PK_F10        },
 { VK_F11        , PK_F11        },
 { VK_F12        , PK_F12        },
 { 0             , 0 }
};

static int LastKeyState;
static UCHAR KeyIsDown = 0;

/*--------------------------------------------------------------------------*/
static int VK_to_PegKey(PegMessage & NewMessage, UCHAR Ascii, int Down, WORD VK, DWORD KeyState)
{
   // get the key state, even if no key was pressed
   LastKeyState = 0;
   if (KeyState & CAPSLOCK_ON)
      LastKeyState |= KF_CAPS;
   if (KeyState & LEFT_ALT_PRESSED)
      LastKeyState |= KF_ALT;
   if (KeyState & LEFT_CTRL_PRESSED)
      LastKeyState |= KF_CTRL;
   if (KeyState & NUMLOCK_ON)
      LastKeyState |= KF_NUML;
   if (KeyState & RIGHT_ALT_PRESSED)
      LastKeyState |= KF_ALT;
   if (KeyState & RIGHT_CTRL_PRESSED)
      LastKeyState |= KF_CTRL;
   if (KeyState & SCROLLLOCK_ON)
      LastKeyState |= KF_SCROLL;
   if (KeyState & SHIFT_PRESSED)
      LastKeyState |= KF_SHIFT | KF_LSHIFT;

    NewMessage.lData = LastKeyState;

   // up or down?
   if (Down || (KeyState & 0x04000000))
      NewMessage.wType = PM_KEY;
   else
      NewMessage.wType = PM_KEY_RELEASE;

   // find special (non ASCII) keys
   NewMessage.iData = 0;
   if ((KeyState & 0x04000000) == 0)
   {
      for (int i=0; VK_to_PK[i].VK != 0; i++)
         if (VK_to_PK[i].VK == VK)
         {
            NewMessage.iData = VK_to_PK[i].PK;
            break;
         }
   }

   if ((VK == PK_TAB) && (NewMessage.lData & KF_CTRL))
      NewMessage.iData = PK_TAB;

   // find ASCII value
   if (NewMessage.iData == 0)
      if (Ascii)
         NewMessage.iData = Ascii;
      else
         if ((NewMessage.wType == PM_KEY_RELEASE) && KeyIsDown)
         {
            NewMessage.iData = KeyIsDown;
            KeyIsDown = 0;
         }
         else
            return 0; // no ASCII value; ignore

   if (NewMessage.wType == PM_KEY)
   {
      KeyIsDown = (UCHAR) NewMessage.iData;

      // check for special ASCII values
      if ((KeyState & 0x04000000) == 0)
      {
         switch (NewMessage.iData)
         {
            case 3: // Cntrl-C
               NewMessage.wType = PM_COPY;
               break;
            case 22: // Cntrl-V
               NewMessage.wType = PM_PASTE;
               break;
            case 24: // Cntrl-X
               NewMessage.wType = PM_CUT;
               break;
            case PK_INSERT:
               if (NewMessage.lData & (KF_SHIFT | KF_LSHIFT))
                  NewMessage.wType = PM_PASTE;  // shift insert
               if (NewMessage.lData & KF_CTRL)
                  NewMessage.wType = PM_COPY;   // Cntrl insert
               break;
         }
      }
    }

    // ignore any ALT special keys
    if ((NewMessage.lData & KF_ALT) && (NewMessage.iData >= 256))
       return 0;

    return 1;
}

/*--------------------------------------------------------------------------*/
static DWORD GetKBStatus(void)
{
   DWORD State = NULL;
   if (GetKeyState(VK_SHIFT) & 0x8000)
      State |= SHIFT_PRESSED;
   if (GetKeyState(VK_CONTROL) & 0x8000)
      State |= LEFT_CTRL_PRESSED;
   if (GetKeyState(VK_MENU) & 0x8000)
      State |= LEFT_ALT_PRESSED;
   if (GetKeyState(VK_CAPITAL) & 0x0001)
      State |= CAPSLOCK_ON;
   if (GetKeyState(VK_NUMLOCK) & 0x0001)
      State |= NUMLOCK_ON;
   if (GetKeyState(VK_SCROLL) & 0x0001)
      State |= SCROLLLOCK_ON;

   return State;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
long FAR WINAPI WndProc(HWND hwnd, UINT message, UINT wParam, LONG lParam)
{
	PAINTSTRUCT ps;
    PegMessage NewMessage;

	PegThing *SomeThing = NULL;
    int i;

	switch (message)
	{
	case WM_PAINT:
	    {
		BeginPaint(hwnd, &ps);
		EndPaint(hwnd, &ps);
		NewMessage.wType = PM_DRAW;
		NewMessage.pTarget = SomeThing->Presentation();
		SomeThing->MessageQueue()->Push(NewMessage);
		}
		return 0;

#if defined(PEG_MOUSE_SUPPORT) || defined(PEG_TOUCH_SUPPORT)
    case WM_LBUTTONDOWN:
	    {
		PEGSCREEN *pWin = (PEGSCREEN *) SomeThing->Screen();
		if (pWin)
		{
            SetCapture(hwnd);
			NewMessage.wType = PM_LBUTTONDOWN;
			NewMessage.Point.x = LOWORD(lParam) - pWin->mwWinRectXOffset;
			NewMessage.Point.y = HIWORD(lParam) - pWin->mwWinRectYOffset;
			NewMessage.iData = LastKeyState;
			SomeThing->MessageQueue()->Push(NewMessage);

            #ifdef RECORD_AUTO_SCRIPT
            PrintMessage(NewMessage, "PM_LBUTTONDOWN");
            #endif
        }
		}
		return 0;

    case WM_LBUTTONUP:
	    {
		ReleaseCapture();
		PEGSCREEN *pWin = (PEGSCREEN *) SomeThing->Screen();
		if (pWin)
		{
            NewMessage.wType = PM_LBUTTONUP;
		    NewMessage.Point.x = LOWORD(lParam) - pWin->mwWinRectXOffset;
		    NewMessage.Point.y = HIWORD(lParam) - pWin->mwWinRectYOffset;
		    NewMessage.iData = LastKeyState;
		    SomeThing->MessageQueue()->Push(NewMessage);

            #ifdef RECORD_AUTO_SCRIPT
            PrintMessage(NewMessage, "PM_LBUTTONUP");
            #endif
        }
		}
		return 0;
#endif

#if defined(PEG_MOUSE_SUPPORT)

    case WM_RBUTTONDOWN:
	    {
		PEGSCREEN *pWin = (PEGSCREEN *) SomeThing->Screen();
		if (pWin)
		{
		    NewMessage.wType = PM_RBUTTONDOWN;
            NewMessage.Point.x = LOWORD(lParam) - pWin->mwWinRectXOffset;
            NewMessage.Point.y = HIWORD(lParam) - pWin->mwWinRectYOffset;
            NewMessage.iData = LastKeyState;
		    SomeThing->MessageQueue()->Push(NewMessage);
		}
		}
		return 0;

    case WM_RBUTTONUP:
	    {
		PEGSCREEN *pWin = (PEGSCREEN *) SomeThing->Screen();
		if (pWin)
		{
            NewMessage.wType = PM_RBUTTONUP;
            NewMessage.Point.x = LOWORD(lParam) - pWin->mwWinRectXOffset;
            NewMessage.Point.y = HIWORD(lParam) - pWin->mwWinRectYOffset;
            NewMessage.iData = LastKeyState;
            SomeThing->MessageQueue()->Push(NewMessage);
        }
		}
		return 0;

    case WM_MOUSEMOVE:
	    {
		    PEGSCREEN *pWin = (PEGSCREEN *) SomeThing->Screen();
            if (pWin)
		    {
                NewMessage.wType = PM_POINTER_MOVE;
                NewMessage.Point.x = LOWORD(lParam) - pWin->mwWinRectXOffset;
		        NewMessage.Point.y = HIWORD(lParam) - pWin->mwWinRectYOffset;
		        NewMessage.iData = LastKeyState;
		        SomeThing->MessageQueue()->Fold(&NewMessage);
		    }
            #ifdef RECORD_AUTO_SCRIPT
            PrintMessage(NewMessage, "PM_POINTER_MOVE");
            #endif

        }
		return 0;

    case WM_SETCURSOR:
        {
            switch(wParam)
            {
            case PPT_NORMAL:
                SetCursor(LoadCursor(NULL, IDC_ARROW));
                break;

            case PPT_VSIZE:
                SetCursor(LoadCursor(NULL, IDC_SIZENS));
                break;

            case PPT_HSIZE:
                SetCursor(LoadCursor(NULL, IDC_SIZEWE));
                break;

            case PPT_NWSE_SIZE:
                SetCursor(LoadCursor(NULL, IDC_SIZENWSE));
                break;

            case PPT_NESW_SIZE:
                SetCursor(LoadCursor(NULL, IDC_SIZENESW));
                break;

            case PPT_IBEAM:
                SetCursor(LoadCursor(NULL, IDC_IBEAM));
                break;

            case PPT_HAND:
                SetCursor(LoadCursor(NULL, IDC_CROSS));
                break;
            }
        }
        return 0;
#endif

    case WM_SIZE:

       #ifdef _PEGWINSCREEN_
        {
		PegWinScreen *pWin = (PegWinScreen *) SomeThing->Screen();
		if (pWin)
		{
            pWin->SetNewWindowsWinSize(LOWORD(lParam), HIWORD(lParam));
        }
        }
       #endif
		return 0;

    case WM_KEYDOWN:
    case WM_KEYUP:
        if (VK_to_PegKey(NewMessage,
                         '\0',
                         message == WM_KEYDOWN,
                         wParam,
                         GetKBStatus()))
        {
            for (i=0; i < (lParam&0xFFFF); i++)
            {
                SomeThing->MessageQueue()->Push(NewMessage);
            }
            return 0;
        }
        else
        {
             break;
        }

    case WM_CHAR:
        if (VK_to_PegKey(NewMessage,
                         wParam,
                         1,
                         0,
                         GetKBStatus()))
        {
            for (i=0; i < (lParam&0xFFFF); i++)
            {
               SomeThing->MessageQueue()->Push(NewMessage);
            }
            return 0;
        }
        else
        {
            break;
        }

   #ifdef PEG_UNICODE
    case WM_IME_CHAR:
        {
        NewMessage.wType = PM_KEY;
        NewMessage.pTarget = NULL;
        WORD wUniVal = wParam;

        //if ((DWORD) GetKeyboardLayout(0) == MAKELANGID(LANG_JAPANESE, SUBLANG_NEUTRAL))
        //{
            // running on Japanese Windows, convert SJIS to Unicode:
    	    wUniVal  = SJISToUnicode(wUniVal);
        //}

        NewMessage.iData = wUniVal;
        NewMessage.lData = GetKBStatus();
        SomeThing->MessageQueue()->Push(NewMessage);
        return 0;
        }
   #endif

    case WM_TIMER:
        SomeThing->MessageQueue()->TimerTick();
		return 0;

    //case WM_SETTEXT:
    //    SetWindowText(hwnd, (LPCTSTR)lParam);
    //    return 0;

    case WM_SYSCOMMAND:
        {
            PegThing pThing;
            char cBuff[240];
            strcpy(cBuff, pThing.Version());

            if(wParam == gwAboutPegID)
            {
                MessageBox(ghDevWnd, cBuff, "About PEG", 
                           MB_OK | MB_ICONINFORMATION);
            }
        }
        break;

    case WM_CLOSE:
        {
		    PegPresentationManager *pPresent = SomeThing->Presentation();
            PegMessage ExitMessage(pPresent, PM_EXIT);
            ExitMessage.pSource = NULL;
			pPresent->Message(ExitMessage);
        }
		break;

	 case WM_DESTROY:
		  PostQuitMessage(0);
		  return 0;
	 }
	 return DefWindowProc(hwnd, message, wParam, lParam);
}


#ifdef RECORD_AUTO_SCRIPT
/*---------------------------------------------------------------------------*/
// PrintMessage
// 
// Note: This prints to stdout. If you would like to trap this into a file,
// redirect the output from the command line.
/*---------------------------------------------------------------------------*/
void PrintMessage(const PegMessage& Mesg,  char *pType)
{
    if (!pOutFile)
    {
        return;
    }
    static time_t tCurrent = time((time_t*)NULL);
    static time_t tLast;
    
    time_t tDiff;
    
    tLast = tCurrent;
    tCurrent = time((time_t*)NULL);
    
    tDiff = tCurrent - tLast;
    tDiff *= 18;

    fprintf(pOutFile, "{{%d, %d}, %ld, %s},\n", Mesg.Point.x, Mesg.Point.y,
            tDiff, pType);
}
#endif  // RECORD_AUTO_SCRIPT

#if defined(PEG_MULTITHREAD)
/*--------------------------------------------------------------------------*/
/*                    Message Queue Replacement Functions                   */
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PegQueue *CreateFreeMessagePool(void)
{
    PegQueue *pQueue = new PegQueue;
    pQueue->QueueSem = CreateSemaphore(NULL, 0, NUM_PEG_FREE_MSGS, NULL);
    pQueue->pFirst = NULL;
    pQueue->pLast   = 0;

    SIGNED i;
    PegMessage *pMessage;

    for (i = 0; i < NUM_PEG_FREE_MSGS; i++)
    {
        pMessage = new PegMessage();
        memset(pMessage, 0, sizeof(PegMessage));

        if (!i)
        {
            pQueue->pFirst = pMessage;
            pQueue->pLast = pMessage;
        }
        else
        {
            pMessage->Next = pQueue->pFirst;
            pQueue->pFirst = pMessage;
        }
    }
    return pQueue;
}

/*--------------------------------------------------------------------------*/
// ReturnToFreePool- called with LOCK_MESSAGE_QUEUE invoked.
/*--------------------------------------------------------------------------*/
void ReturnToFreePool(PegMessage *pMesg)
{
    pMesg->Next = NULL;
    if (gpPegFreeMessages->pLast)
    {
        gpPegFreeMessages->pLast->Next = pMesg;
        gpPegFreeMessages->pLast = pMesg;
    }
    else
    {
        gpPegFreeMessages->pFirst = gpPegFreeMessages->pLast = pMesg;
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PEG_QUEUE_TYPE CreateMessageQueue(void)
{
    PegQueue *pQueue = new PegQueue;
    pQueue->QueueSem = CreateSemaphore(NULL, 0, NUM_PEG_FREE_MSGS, NULL);
    pQueue->pFirst = NULL;
    pQueue->pLast   = 0;
    return (pQueue);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void DeleteMessageQueue(PEG_QUEUE_TYPE pq)
{
    PegQueue *pQueue = pq;
    PegMessage *pCurrent = pQueue->pFirst;
    PegMessage *pNext;

    while(pCurrent)
    {
        pNext = pCurrent->Next;
        delete (pCurrent);
        pCurrent = pNext;
    }
    CloseHandle(pQueue->QueueSem);
    delete pQueue;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void EnqueueMessage(void *pGet, PEG_QUEUE_TYPE pq)

{
    LOCK_MESSAGE_QUEUE
    PegQueue *pQueue = pq;
    PegMessage *In = (PegMessage *) pGet;

    PegMessage *pPut = gpPegFreeMessages->pFirst;

    if (!pPut)
    {
        UNLOCK_MESSAGE_QUEUE
        return;
    }

    gpPegFreeMessages->pFirst = pPut->Next;
    if (!gpPegFreeMessages->pFirst)
    {
        gpPegFreeMessages->pLast = NULL;
    }

    *pPut = *In;
    pPut->Next = NULL;

    if (pQueue->pLast)
    {
        pQueue->pLast->Next = pPut;
        pQueue->pLast = pPut;
        UNLOCK_MESSAGE_QUEUE
    }
    else
    {
        pQueue->pFirst = pPut;
        pQueue->pLast = pPut;
        UNLOCK_MESSAGE_QUEUE
    }
    ReleaseSemaphore(pQueue->QueueSem, 1, NULL);
}


/*--------------------------------------------------------------------------*/
// There is only 1 instance of the PegMessageQueue in this integration.
// This keeps the free message pools (1 for the application and 1 for
// hardware input messages).
/*--------------------------------------------------------------------------*/
PegMessageQueue::PegMessageQueue(void)
{
    // create the free message pool and exchanges to send
    // mouse and keyboard messages to PEG:
    
    gpPegFreeMessages = CreateFreeMessagePool();
    gpPegMesgInput = (PegQueue *) CreateMessageQueue();
    mpTimerList = NULL;
}

/*--------------------------------------------------------------------------*/
PegMessageQueue::~PegMessageQueue()
{
    DeleteMessageQueue(gpPegFreeMessages);
    DeleteMessageQueue(gpPegMesgInput);

    while(mpTimerList)
    {
    	PegTimer *pTimer = mpTimerList->pNext;
    	delete mpTimerList;
    	mpTimerList = pTimer;
    }
}


/*--------------------------------------------------------------------------*/
void PegMessageQueue::Push(PegMessage *In)
{

   #ifdef PEG_MULTITHREAD

    if (In->pTarget)
    {
    	if (In->pTarget->Presentation()->RouteMessageToTask(In))
    	{
	        return;
    	}
    }
   #endif

    EnqueueMessage(In, gpPegMesgInput);
}


/*--------------------------------------------------------------------------*/
void PegMessageQueue::Pop(PegMessage *Put)
{
    PegMessage *pGet;

    if (CURRENT_TASK != PEG_TASK_PTR)
    {
    	PegThing *pt = NULL;
    	PegQueue *pQueue = 
                       (PegQueue*)pt->Presentation()->GetCurrentMessageQueue();

	    if (pQueue)
        {
            WaitForSingleObject(pQueue->QueueSem, INFINITE);
            LOCK_MESSAGE_QUEUE

            pGet = pQueue->pFirst;
            pQueue->pFirst = pGet->Next;

            if (!pQueue->pFirst)
            {
                pQueue->pLast = NULL;
            }
            *Put = *pGet;
            ReturnToFreePool(pGet);
            UNLOCK_MESSAGE_QUEUE
            return;
	    }
	}

    while(1)
    {
        WaitForSingleObject(gpPegMesgInput->QueueSem, INFINITE);

        LOCK_MESSAGE_QUEUE

        pGet = gpPegMesgInput->pFirst;
        gpPegMesgInput->pFirst = pGet->Next;

        if (!pGet->Next)
        {
            gpPegMesgInput->pLast = NULL;
        }
        
        if (pGet->wType == PM_TIMER && !pGet->pTarget)
	    {
	        ReturnToFreePool(pGet);
            UNLOCK_MESSAGE_QUEUE
 	        TimerTick();
            //gbPegTimerEventPending = FALSE;
        }
        else
        {
	        *Put = *pGet;
            ReturnToFreePool(pGet);
            UNLOCK_MESSAGE_QUEUE
	        return;
	    }
    }
}


/*--------------------------------------------------------------------------*/
// Fold: Looks for duplicate message based on target and type. If a duplicate
// message is found, updates existing message to new value, otherwise
// pushes message.
//
// Note:
//
//  This routine is currently only called by the mouse service LSR, so 
//  it only checks the PegInput message queue. It is not necessary to
//  extend this service to support MULTITHREAD at this time.
/*--------------------------------------------------------------------------*/
void PegMessageQueue::Fold(PegMessage *In)
{
    PegQueue *pQueue = NULL;

    if (In->pTarget)
    {
        pQueue = gpPresentation->GetThingMessageQueue(In->pTarget);
    }

    if (!pQueue)
    {
        pQueue = gpPegMesgInput;
    }

    Fold(In, pQueue);
}

/*--------------------------------------------------------------------------*/
// Fold: Looks for duplicate message based on target and type. If a duplicate
// message is found, updates existing message to new value, otherwise
// pushes message.
//
// Note:
//
//  This routine is currently only called by the mouse service LSR, so 
//  it only checks the PegInput message queue. It is not necessary to
//  extend this service to support MULTITHREAD at this time.
/*--------------------------------------------------------------------------*/
void PegMessageQueue::Fold(PegMessage *In, PEG_QUEUE_TYPE pInQueue)
{
    PegQueue *pQueue = pInQueue;

    LOCK_MESSAGE_QUEUE

    PegMessage *pCurrent = pQueue->pFirst;

    while(pCurrent)
    {
        if (pCurrent->wType == In->wType &&
            pCurrent->pTarget == In->pTarget &&
            pCurrent->iData == In->iData)

        {
            pCurrent->Rect = In->Rect;
            UNLOCK_MESSAGE_QUEUE
            return;
        }
        pCurrent = pCurrent->Next;
    }

    // didn't find in queue, push it:

    UNLOCK_MESSAGE_QUEUE
    EnqueueMessage(In, pQueue);
}


/*--------------------------------------------------------------------------*/
// Purge: Removes any messages from the message queue which are targeted
//     for a deleted thing.
/*--------------------------------------------------------------------------*/
void PegMessageQueue::Purge(PegThing *Del)
{
    PegQueue *pQueue;

    pQueue = Del->Presentation()->GetThingMessageQueue(Del);

    if (!pQueue)
    {
        pQueue = gpPegMesgInput;
    }

    LOCK_MESSAGE_QUEUE

    PegMessage *pm = pQueue->pFirst;
    PegMessage *pPrev = NULL;
    PegMessage *pNext;

    while(pm)
    {
        if (pm->pTarget == Del)
 	    {
            WaitForSingleObject(pQueue->QueueSem, 0);
            if (pPrev)
            {
                pPrev->Next = pm->Next;
            }
            else
            {
                pQueue->pFirst = pm->Next;
                if (!pQueue->pFirst)
                {
                    pQueue->pLast = NULL;
                }
            }

            if (pQueue->pLast == pm)
            {
                pQueue->pLast = pPrev;
            }
            pNext = pm->Next;
            ReturnToFreePool(pm);
            pm = pNext;
        }
        else
        {
            pPrev = pm;
            pm = pm->Next;
        }
    }
    UNLOCK_MESSAGE_QUEUE
}

#endif  // PEG_MULTITHREAD

