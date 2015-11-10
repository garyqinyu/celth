/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
// pfdialog.cpp
//
// PegFileDialog capable of being used for opening, saving and 'saving as' 
// a file on Unix style OS (uses Unix style dir functions, etc...).
// This class may be optionally included in the standard PEG library if
// needed, but is originally intended to be used along with the PEG
// utilities (WindowBuilder, ImageConvert and FontCapture).
//
// Author: Jim DeLisle
//
// Copyright (c) 1998-2000 Swell Software 
//              All Rights Reserved.
//
// Unauthorized redistribution of this source code, in whole or part,
// without the express written permission of Swell Software
// is strictly prohibited.
//
// Notes:
//
//	This class is intended for use on Unix style OS's only. For Win32
//  development platforms, use the standard Win32 file dialog box.
//
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include <stdlib.h>     // qsort
#include <unistd.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <pwd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

#include "peg.hpp"

#if defined(PEG_FILE_DIALOG)

#define INCLUDE_PFDIALOG_BITMAPS
#include "pfdialog.hpp"
#undef INCLUDE_PFDIALOG_BITMAPS

/*---------------------------------------------------------------------------*/
// External bitmaps
/*---------------------------------------------------------------------------*/
extern PegBitmap gbUpDirBitmap;
extern PegBitmap gbHomeBitmap;
extern PegBitmap gbRefreshBitmap;
extern PegBitmap gbFileSmallBitmap;
extern PegBitmap gbFilterBitmap;

/*---------------------------------------------------------------------------*/
// Strings (ASCII and Unicode) for the toolbar buttons and labels
/*---------------------------------------------------------------------------*/
#if defined(PEG_UNICODE)
static PEGCHAR gsHome[] = {'H', 'o', 'm', 'e', '\0'};
static PEGCHAR gsUp[] = {'U', 'p', '\0'};
static PEGCHAR gsRefresh[] = {'R', 'e', 'f', 'r', 'e', 's', 'h', '\0'};
static PEGCHAR gsViewAll[] = {'V', 'i', 'e', 'w', ' ', 'A', 'l', 'l', '\0'};
static PEGCHAR gsFilter[] = {'F', 'i', 'l', 't', 'e', 'r', '\0'};
static PEGCHAR gsDirectories[] = {'D', 'i', 'r', 'e', 'c', 't', 'o', 'r', 'i',
                                  'e', 's', '\0'};
static PEGCHAR gsFiles[] = {'F', 'i', 'l', 'e', 's', '\0'};
static PEGCHAR gsSelection[] = {'S', 'e', 'l', 'e', 'c', 't', 'i', 'o', 'n',
                                ':', '\0'};
static PEGCHAR gsFilterColon[] = {'F', 'i', 'l', 't', 'e', 'r', ':', '\0'};
static PEGCHAR gsFileError[] = {'F', 'i', 'l', 'e', ' ', 'E', 'r', 'r', 'o',
                                'r', '\0'};
static PEGCHAR gsSelectFile[] = {'P', 'l', 'e', 'a', 's', 'e', ' ', 's', 'e',
                                 'l', 'e', 'c', 't', ' ', 'a', ' ', 'f', 'i',
                                 'l', 'e', ' ', 'n', 'a', 'm', 'e', '.', '\0'};
static PEGCHAR gsEnterFile[] = {'P', 'l', 'e', 'a', 's', 'e', ' ', 'e', 'n',
                                 't', 'e', 'r', ' ', 'a', ' ', 'f', 'i',
                                 'l', 'e', ' ', 'n', 'a', 'm', 'e', '.', '\0'};
static PEGCHAR gsNoActiveFilter[] = {'N', 'o', ' ', 'F', 'i', 'l', 't', 'e',
                                     'r', ' ', 'A', 'c', 't', 'i', 'v', 'e',
                                     '\0'};
static PEGCHAR gsOverwriteOk[] = {'T', 'h','e', ' ', 's', 'e', 'l', 'e', 't',
                                  'e', 'd', ' ', 'f', 'i', 'l', 'e', ' ',
                                  'e', 'x', 'i', 's', 't', 's', '.', ' ',
                                  'O', 'v', 'e', 'w', 'r', 'i', 't', 'e', ' ',
                                  'i', 't', '?', '\0' };
static PEGCHAR gsNoCreateFile[] = {'U', 'n', 'a', 'b', 'l', 'e', ' ', 't', 'o',
                                   ' ', 'c', 'r', 'e', 'a', 't', 'e', ' ', 't',
                                   'h', 'e', ' ', 'f', 'i', 'l', 'e', '.', ' ',
                                   'P', 'l', 'e', 's', 'e', ' ', 'c',
                                   'h', 'e', 'c', 'k', ' ', 'y', 'o', 'u', 'r',
                                   ' ', 'p', 'e', 'r', 'm', 'i', 's', 's', 'i',
                                   'o', 'n', 's', '.', '\0' };
#else
static PEGCHAR gsHome[] = "Home";
static PEGCHAR gsUp[] = "Up";
static PEGCHAR gsRefresh[] = "Refresh";
static PEGCHAR gsViewAll[] = "View All";
static PEGCHAR gsFilter[] = "Filter";
static PEGCHAR gsDirectories[] = "Directories";
static PEGCHAR gsFiles[] = "Files";
static PEGCHAR gsSelection[] = "Selection:";
static PEGCHAR gsFilterColon[] = "Filter:";
static PEGCHAR gsFileError[] = "File Error";
static PEGCHAR gsSelectFile[] = "Please select a file name.";
static PEGCHAR gsEnterFile[] = "Please enter a file name.";
static PEGCHAR gsNoActiveFilter[] = "No Filter Active";
static PEGCHAR gsOverwriteOk[] = "The selected file exists. Overwrite it?";
static PEGCHAR gsNoCreateFile[] = "Unable to create the file. Please check "
                                  "your permissions.";

#endif

static char gsCurDir[] = ".";
static char gsUpDir[] = "..";

extern PEGCHAR lsOK[];
extern PEGCHAR lsCANCEL[];

/*---------------------------------------------------------------------------*/
// Static members of PegFileDialog
/*---------------------------------------------------------------------------*/
BOOL PegFileDialog::mbSortDirLowToHigh = TRUE;
BOOL PegFileDialog::mbSortFileLowToHigh = TRUE;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
PegFileFilter::PegFileFilter() :
    mwFilterCount(0),
    mpFilterName(NULL),
    mpFilterList(NULL)
{
}

/*---------------------------------------------------------------------------*/
PegFileFilter::PegFileFilter(const char* pFilter)
{
    mwFilterCount = 0;
    mpFilterName = NULL;
    mpFilterList = NULL;

    Set(pFilter);
}

/*---------------------------------------------------------------------------*/
PegFileFilter::~PegFileFilter()
{
    Reset();
}

/*---------------------------------------------------------------------------*/
WORD PegFileFilter::Set(const char* pFilter)
{
    if(!pFilter)
    {
        return(0);
    }

    if(mwFilterCount)
    {
        Reset();
    }

    // The format of the filter should be something like this:
    // "Filter Name\0*.bmp;*.jpg;*.gif;*.png\0"
    
    WORD wLen = strlen(pFilter);
    if(wLen)
    {
        mpFilterName = new char[wLen + 1];
        //strncpy(mpFilterName, pFilter, wLen + 1);
        strcpy(mpFilterName, pFilter);
    }
    else
    {
        return(0);
    }

    const char* pCur = (pFilter + wLen + 1);
    wLen = strlen(pCur) + 1;
    char* pFilters = new char[wLen];
    strcpy(pFilters, pCur);
    char* pToken = ";";

    pCur = strtok(pFilters, pToken);
    while(pCur)
    {
        mwFilterCount++;
        pCur = strtok(NULL, pToken);
    }

    if(mwFilterCount)
    {
        mpFilterList = new char*[mwFilterCount];
        pCur = (pFilter + strlen(mpFilterName) + 1);
        strcpy(pFilters, pCur);
        WORD wIndex = 0;
        const char* pCurFilter;

        pCur = strtok(pFilters, pToken);
        while(pCur)
        {
            pCurFilter = pCur + strlen(pCur) - 1;
            while(*pCurFilter != '.')
            {
                pCurFilter--;
            }

            if(pCurFilter >= pCur)
            {
                mpFilterList[wIndex] = new char[strlen(pCurFilter) + 1];
                strcpy(mpFilterList[wIndex], pCurFilter);
            }

            pCur = strtok(NULL, pToken);
            wIndex++;
        }
    }
    else
    {
        delete [] pFilters;
        delete [] mpFilterName;
        mpFilterName = NULL;

        return(0);
    }

    return(mwFilterCount);
}

/*---------------------------------------------------------------------------*/
void PegFileFilter::Reset()
{
    if(mpFilterName)
    {
        delete [] mpFilterName;
        mpFilterName = NULL;
    }

    if(mpFilterList)
    {
        for(WORD i = 0; i < mwFilterCount; i++)
        {
            delete [] mpFilterList[i];
        }
        delete mpFilterList;
        mpFilterList = NULL;
        mwFilterCount = 0;
    }
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
PegListSortButton::PegListSortButton(PegRect& tRect, const PEGCHAR* pText,
                                     WORD wID, WORD wStyle) :
    PegTextButton(tRect, pText, wID, wStyle),
    mbStartLow(TRUE)
{
}

/*---------------------------------------------------------------------------*/
PegListSortButton::~PegListSortButton()
{
}

/*---------------------------------------------------------------------------*/
void PegListSortButton::Draw()
{
    BeginDraw();
    
    PegTextButton::Draw();
    
    // Draw the up/down triangle to indicate a sort order
    // The order in which the lines are drawn is important so the
    // light and dark overlap properly
    PegColor tColor(PCLR_LOWLIGHT, PCLR_LOWLIGHT, CF_NONE);
    PegRect tRect = mClient;
    WORD wPad = tRect.Height() >> 2;
    tRect.wTop += wPad;
    tRect.wBottom -= wPad;
    tRect.wRight -= wPad;
    tRect.wLeft = tRect.wRight - (tRect.Height() >> 1);
    
    WORD wMidX = tRect.wLeft + (tRect.Width() >> 1);

    if(mbStartLow)
    {
        Line(tRect.wLeft, tRect.wTop, tRect.wRight, tRect.wTop,
             tColor);
        Line(tRect.wLeft, tRect.wTop, wMidX, tRect.wBottom,
             tColor);
        tColor.uForeground = PCLR_HIGHLIGHT;
        Line(wMidX, tRect.wBottom, tRect.wRight, tRect.wTop,
             tColor);
    }
    else
    {
        tColor.uForeground = PCLR_HIGHLIGHT;
        Line(tRect.wLeft, tRect.wBottom, tRect.wRight, tRect.wBottom,
             tColor);
        Line(wMidX, tRect.wTop, tRect.wRight, tRect.wBottom, tColor);

        tColor.uForeground = PCLR_LOWLIGHT;
        Line(tRect.wLeft, tRect.wBottom, wMidX, tRect.wTop,
             tColor);
    }
    
    EndDraw();
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
PegFileDialog::PegFileDialog(const PEGCHAR* pTitle, SIGNED iLeft /*=-1*/, 
							 SIGNED iTop /*=-1*/) :
	PegDialog(pTitle, NULL, FF_THICK),
	mpLastValidDir(NULL),
    mbViewAll(FALSE),
    mbFilterActive(FALSE),
	mpFileNameBuffer(NULL), 
	mwFileNameBufferLen(0),
    mpDefNewExt(NULL),
    mpCurrentFilter(NULL),
    mpFilterList(NULL),
    mwFilterCount(0)
{
	PegRect Rect;
	Rect.Set(iLeft, iTop, iLeft + 420, iTop + 360);
	mReal = Rect;
	InitClient();

	if(iLeft == -1 && iTop == -1)
	{
		Presentation()->Center(this);
	}
    
    // Maybe later...
	//mtMinSize = mReal;
	//AddStatus(PSF_SIZEABLE);
    RemoveStatus(PSF_SIZEABLE);

	// Add the toolbar
	PegToolBar* pTb = new PegToolBar();
	PegToolBarPanel* pPanel;
	PegDecoratedButton* pButton;

    pPanel = new PegToolBarPanel();
	pButton = new PegDecoratedButton(0, 0, gsHome, &gbHomeBitmap, IDB_HOME, 
									 AF_ENABLED, TRUE);
	pPanel->Add(pButton);
	pButton = new PegDecoratedButton(0, 0, gsUp, &gbUpDirBitmap,
									 IDB_UPDIR, AF_ENABLED, TRUE);
	pPanel->Add(pButton);
	pTb->AddPanel(pPanel);

	pPanel = new PegToolBarPanel();
	pButton = new PegDecoratedButton(0, 0, gsRefresh, &gbRefreshBitmap,
									 IDB_REFRESH, AF_ENABLED, TRUE);
	pPanel->Add(pButton);
	pButton = new PegDecoratedButton(0, 0, gsViewAll, &gbFileSmallBitmap,
									 IDB_VIEWALL, AF_ENABLED, TRUE);
	pPanel->Add(pButton);
    pButton = new PegDecoratedButton(0, 0, gsFilter, &gbFilterBitmap,
                                     IDB_APPLYFILTER, AF_ENABLED, TRUE);
    pPanel->Add(pButton);
    pTb->AddPanel(pPanel);

	Add(pTb);

	// Add the scrolling window that shows the files
	//Rect.Shift(0, 24);
	//Rect.wBottom = mClient.wBottom - 60;
	WORD wPad = 4;
    Rect = mClient;
    Rect.Shift(wPad, wPad);
    Rect.wRight = Rect.wLeft + (Rect.Width() * 2 / 5 /*>> 1*/) - (wPad << 1);
    Rect.wBottom = Rect.wTop + 20;
    mpSortDir = new PegListSortButton(Rect, gsDirectories, IDB_SORTDIR,
                                      AF_ENABLED | TJ_LEFT | FF_NONE);
    mpSortDir->SetFont(&SysFont);
    AddToEnd(mpSortDir);
    
    Rect.wLeft = Rect.wRight + (wPad << 1);
    Rect.wRight = mClient.wRight - wPad;
    mpSortFile = new PegListSortButton(Rect, gsFiles, IDB_SORTFILE,
                                       AF_ENABLED | TJ_LEFT | FF_NONE);
    mpSortFile->SetFont(&SysFont);
    AddToEnd(mpSortFile);
    
    Rect.wRight = Rect.wLeft - wPad;
    Rect.wLeft = mClient.wLeft + wPad;
    Rect.wTop = Rect.wBottom;
    Rect.wBottom = Rect.wTop + (mClient.Height() - 110);

    mpDirList = new PegVertList(Rect, IDC_DIRLIST, FF_RECESSED);
	mpDirList->SetScrollMode(WSM_AUTOSCROLL | WSM_CONTINUOUS);
    AddToEnd(mpDirList);
    
    Rect.wLeft = Rect.wRight + wPad;
    Rect.wRight = mClient.wRight - wPad;
	mpFileList = new PegVertList(Rect, IDC_FILELIST, FF_RECESSED);
	mpFileList->SetScrollMode(WSM_AUTOSCROLL | WSM_CONTINUOUS);
	AddToEnd(mpFileList);
	
    Rect.wLeft = mClient.wLeft + wPad;
    Rect.wTop = Rect.wBottom + wPad;
    Rect.wRight = Rect.wLeft + TextWidth(gsSelection, DEF_PROMPT_FONT) + wPad;
    Rect.wBottom = Rect.wTop + 20;
    AddToEnd(new PegPrompt(Rect, gsSelection));
    
    Rect.wLeft = Rect.wRight;
    Rect.wRight = mClient.wRight - wPad;
    
    mpPresentDir = new PegPrompt(Rect, NULL, IDC_PRESENTDIR,
                                 TJ_LEFT | FF_NONE | TT_COPY);
    mpPresentDir->SetColor(PCI_NORMAL, muColors[PCI_NORMAL]);
    mpPresentDir->SetFont(&MenuFont);
    AddToEnd(mpPresentDir);
    
	Rect.wTop = Rect.wBottom + wPad;
    Rect.wBottom = Rect.wTop + 20;
    Rect.wLeft = mClient.wLeft + wPad;
	mpFileString = new PegString(Rect.wLeft, Rect.wTop, 
								(Rect.wRight - Rect.wLeft));
    mpFileString->RemoveStatus(PSF_ACCEPTS_FOCUS | PSF_SELECTABLE);
	AddToEnd(mpFileString);

    Rect.wTop = Rect.wBottom + (wPad << 1);
    Rect.wBottom = Rect.wTop + 20;
    Rect.wRight = Rect.wLeft + TextWidth(gsFilterColon, DEF_PROMPT_FONT) + wPad;
    AddToEnd(new PegPrompt(Rect, gsFilterColon));

    Rect.wLeft = Rect.wRight;
    Rect.wRight = Rect.wLeft + 200;
    mpFilter = new PegPrompt(Rect, NULL, 0, TJ_LEFT | FF_NONE | TT_COPY);
    mpFilter->SetColor(PCI_NORMAL, muColors[PCI_NORMAL]);
    mpFilter->SetFont(&MenuFont);
    AddToEnd(mpFilter);

	// Add the OK and Cancel buttons
	Rect = mClient;
	Rect.wBottom -= wPad;
	Rect.wRight -= 72;
	Rect.wTop = Rect.wBottom - 20;
	Rect.wLeft = Rect.wRight - 60;

	AddToEnd(new PegTextButton(Rect, lsOK, IDB_OK));
	//Rect.Shift(-66, 0);
	Rect.Shift(68, 0);
	AddToEnd(new PegTextButton(Rect, lsCANCEL, IDB_CANCEL));

}

/*---------------------------------------------------------------------------*/
PegFileDialog::~PegFileDialog()
{
    if(mpLastValidDir)
    {
        delete [] mpLastValidDir;
    }

    ResetFilter();
}

/*---------------------------------------------------------------------------*/
SIGNED PegFileDialog::Message(const PegMessage& Mesg)
{
    switch(Mesg.wType)
	{
	case SIGNAL(IDB_UPDIR, PSF_CLICKED):
		//TryChangeDir("..");
		TryChangeDir(gsUpDir);
		break;

	case SIGNAL(IDB_HOME, PSF_CLICKED):
		{
			passwd* tPasswd = getpwuid(getuid());
			if(tPasswd)
			{
				if(TryChangeDir(tPasswd->pw_dir))
				{
				}
			}
		}
		break;

    case SIGNAL(IDB_VIEWALL, PSF_CLICKED):
        mbViewAll = !mbViewAll;
        if(mbViewAll)
        {
            mbFilterActive = FALSE;
        }
        ClearDirList();
        RepopulateDirList();
        ClearFileList();
        ClearFileName();
        RepopulateFileList();
        UpdateFilter();
        break;

    case SIGNAL(IDB_APPLYFILTER, PSF_CLICKED):
        mbFilterActive = !mbFilterActive;
        if(mbFilterActive)
        {
            mbViewAll = FALSE;
        }
        ClearDirList();
        RepopulateDirList();
        ClearFileList();
        ClearFileName();
        RepopulateFileList();
        UpdateFilter();
        break;

    case SIGNAL(IDB_REFRESH, PSF_CLICKED):
        ClearDirList();
        RepopulateDirList();
        ClearFileList();
        RepopulateFileList();
        break;
        
    case SIGNAL(IDB_SORTDIR, PSF_CLICKED):
        mbSortDirLowToHigh = !mbSortDirLowToHigh;
        mpSortDir->SetStartLow(mbSortDirLowToHigh);
        ClearDirList();
        RepopulateDirList();
        break;

    case SIGNAL(IDB_SORTFILE, PSF_CLICKED):
        mbSortFileLowToHigh = !mbSortFileLowToHigh;
        mpSortFile->SetStartLow(mbSortFileLowToHigh);
        ClearFileList();
        ClearFileName();
        RepopulateFileList();
        break;
        
    case SIGNAL(IDC_DIRLIST, PSF_LIST_SELECT):
        {
            PegPrompt* pPrompt = (PegPrompt*)mpDirList->Find(Mesg.iData);
            if(pPrompt)
            {
               #if defined(PEG_UNICODE)
                char* pData = ConvertDataGet(pPrompt->DataGet());
                BuildNewDirName(pData);
                delete [] pData;
               #else
                BuildNewDirName(pPrompt->DataGet());
               #endif
            }
        } break;
        
    case SIGNAL(IDC_FILELIST, PSF_LIST_SELECT):
        {
            PegPrompt* pPrompt = (PegPrompt*)mpFileList->Find(Mesg.iData);
            if(pPrompt)
            {
                mpFileString->DataSet(pPrompt->DataGet());
                mpFileString->Draw();
            }
        } break;
        
    case SIGNAL(IDB_OK, PSF_CLICKED):
        {
            if(ValidateEntry())
            {
                return(PegDialog::Message(Mesg));
            }

        } break;

	default:
		return(PegDialog::Message(Mesg));
	}

	return 0;
}

/*---------------------------------------------------------------------------*/
SIGNED PegFileDialog::Execute()
{
    if(!mpFileNameBuffer || !mwFileNameBufferLen)
    {
        return(PFD_ERROR);
    }

    SIGNED iRetVal;

    iRetVal = PegDialog::Execute();
    
    return(iRetVal);
}

/*---------------------------------------------------------------------------*/
BOOL PegFileDialog::SetMode(SIGNED iOperation, PEGCHAR* pBuffer, 
                            WORD wBuffSize, const char* pStartNode /*=NULL*/)
{
    miOperation = iOperation;
    mpFileNameBuffer = pBuffer;
    mwFileNameBufferLen = wBuffSize;

    if(miOperation & PFD_FILESAVEAS)
    {
        mpFileString->AddStatus(PSF_ACCEPTS_FOCUS | PSF_SELECTABLE);
    }
    else
    {
        mpFileString->RemoveStatus(PSF_ACCEPTS_FOCUS | PSF_SELECTABLE);
    }
    
    BOOL bRetVal = FALSE;
    
    if(pStartNode)
    {
        bRetVal = TryChangeDir(pStartNode);
    }
    
    return(bRetVal);
}

/*---------------------------------------------------------------------------*/
void PegFileDialog::SetFilter(const char* pFilter)
{
    if(mwFilterCount)
    {
        ResetFilter();
    }

    StoreFilter(pFilter);

    if(mwFilterCount)
    {
        mpCurrentFilter = mpFilterList[0];
        mbFilterActive = TRUE;
        ClearDirList();
        RepopulateDirList();
        ClearFileList();
        ClearFileName();
        RepopulateFileList();
    }

    UpdateFilter();
}

/*---------------------------------------------------------------------------*/
int PegFileDialog::SortDirList(const void* pDir1, const void* pDir2)
{
    return(strcmp((const char*)pDir1, (const char*)pDir2));
}

/*---------------------------------------------------------------------------*/
int PegFileDialog::SortFileList(const void* pFile1, const void* pFile2)
{
    return(strcmp((const char*)pFile1, (const char*)pFile2));
}

/*---------------------------------------------------------------------------*/
BOOL PegFileDialog::TryChangeDir(const char* pPath)
{
	if(mpLastValidDir)
	{
		if(strcmp(pPath, mpLastValidDir) == 0)
		{
			// We're already there, move along...
			return TRUE;
		}
	}

	if((chdir(pPath)) == 0)
	{
		// Success
		if((strcmp(pPath, ".") == 0) ||
		   (strcmp(pPath, "..") == 0))
		{
			size_t tBuffSize = 1024;
			char* pBuffer = new char[1024];
			if((getcwd(pBuffer, tBuffSize)) == NULL)
			{
				// there was a problem
				if(errno == ERANGE)
				{
					tBuffSize = 2048;
					delete [] pBuffer;
					pBuffer = new char[2048];
					if((getcwd(pBuffer, tBuffSize)) == NULL)
					{
						perror("Error retreiving pwd\n");
						delete [] pBuffer;
						chdir(mpLastValidDir);		
						return FALSE ;
					}
					else
					{
						DWORD dwLen = strlen(pBuffer);
						delete [] mpLastValidDir;
						mpLastValidDir = new char[dwLen + 1];
						strcpy(mpLastValidDir, pBuffer);
						delete [] pBuffer;
					}
				}
				else
				{
					chdir(mpLastValidDir);
					delete [] pBuffer;
					return FALSE;
				}
			}
			else
			{
				DWORD dwLen = strlen(pBuffer);
				delete [] mpLastValidDir;
				mpLastValidDir = new char[dwLen + 1];
				strcpy(mpLastValidDir, pBuffer);
				delete [] pBuffer;
			}
		}
		else
		{
			DWORD dwLen = strlen(pPath);
			delete [] mpLastValidDir;
			mpLastValidDir = new char[dwLen + 1];
			strcpy(mpLastValidDir, pPath);
		}
	}
	else
	{
		return FALSE;
	}

    ClearDirList();
	RepopulateDirList();
    ClearFileList();
	RepopulateFileList();
	
	mpFileString->DataSet(NULL);
	mpFileString->Draw();
    
    UpdatePresentDir();

	return TRUE;
}

/*---------------------------------------------------------------------------*/
void PegFileDialog::RepopulateDirList()
{
    DIR* pDir;
	dirent* pEntry;
	struct stat	tStat;
    DWORD dwPos, dwCount;
	
	if((pDir = opendir(mpLastValidDir)) == NULL)
	{
		printf("Unable to open directory %s\n", mpLastValidDir);
		return;
	}
    
    dwPos = telldir(pDir);
    
    // Count how many entries
    DWORD dwEntries = 0;
    while((pEntry = readdir(pDir)) != NULL)
    {
        stat(pEntry->d_name, &tStat);
        if(S_ISDIR(tStat.st_mode))
        {
            if((strcmp(pEntry->d_name, gsCurDir) == 0) ||
               (strcmp(pEntry->d_name, gsUpDir) == 0))
            {
                continue;
            }

            if(mbViewAll)
            {
                dwEntries++;
            }
            else
            {
                if(!(pEntry->d_name[0] == '.'))
                {
                    dwEntries++;
                }
            }
        }
    }
    
    if(dwEntries == 0)
    {
        closedir(pDir);
        return;
    }
    
    char* pEntries = new char[dwEntries * PFD_BUFFERLEN];
    char* pCurEntry = pEntries;
    
    // Rewind the directory and get the names.
    seekdir(pDir, dwPos);
    
    while((pEntry = readdir(pDir)) != NULL)
    {
		stat(pEntry->d_name, &tStat);

		if(S_ISDIR(tStat.st_mode))
        {
            if((strcmp(pEntry->d_name, gsCurDir) == 0) ||
               (strcmp(pEntry->d_name, gsUpDir) == 0))
            {
                continue;
            }

            if(mbViewAll)
            {
                strcpy(pCurEntry, pEntry->d_name);
                pCurEntry += PFD_BUFFERLEN;
            }
            else
            {
                if(!(pEntry->d_name[0] == '.'))
                {
                    strcpy(pCurEntry, pEntry->d_name);
                    pCurEntry += PFD_BUFFERLEN;
                }
            }
		}
	}
    
    // Sort the list
    qsort(pEntries, dwEntries, PFD_BUFFERLEN, SortDirList);
    
    // Add the prompts to the file list
    pCurEntry = pEntries;
    PegPrompt* pPrompt;

   #if defined(PEG_UNICODE)
    PEGCHAR* pCurEntryData = new PEGCHAR[PFD_BUFFERLEN + 1];
   #endif

    for(dwCount = 0; dwCount < dwEntries; dwCount++)
    {
       #if defined(PEG_UNICODE)
        //strcpy(pCurEntryData, pCurEntry);
        AsciiToUnicode(pCurEntryData, pCurEntry);
        pPrompt = new PegPrompt(0, 0, pCurEntryData, dwCount + 1,
                                TJ_LEFT | TT_COPY | FF_NONE | AF_ENABLED);
       #else
        pPrompt = new PegPrompt(0, 0, pCurEntry, dwCount + 1,
                                TJ_LEFT | TT_COPY | FF_NONE | AF_ENABLED);
       #endif
        pPrompt->SetFont(&MenuFont);
        
        if(mbSortDirLowToHigh)
        {
            mpDirList->AddToEnd(pPrompt, FALSE);
        }
        else
        {
            mpDirList->Add(pPrompt, FALSE);
        }

        pCurEntry += PFD_BUFFERLEN;
    }
    
	closedir(pDir);
    delete [] pEntries;

   #if defined(PEG_UNICODE)
    delete [] pCurEntryData;
   #endif

    mpDirList->Draw();
}

/*---------------------------------------------------------------------------*/
void PegFileDialog::RepopulateFileList()
{
    DIR* pDir;
	dirent* pEntry;
	struct stat	tStat;
    DWORD dwPos, dwCount;
	
	if((pDir = opendir(mpLastValidDir)) == NULL)
	{
		printf("Unable to open directory %s\n", mpLastValidDir);
		return;
	}
    
    dwPos = telldir(pDir);
    
    // Count how many entries
    DWORD dwEntries = 0;
    char* pFileName = NULL;
    WORD wFileNameLen = 0;
    WORD wFilterLen = 0;
    char** pFilterList;
    while((pEntry = readdir(pDir)) != NULL)
    {
        stat(pEntry->d_name, &tStat);
        if(!(S_ISDIR(tStat.st_mode)))
        {
            if(mbViewAll)
            {
                dwEntries++;
            }
            else
            {
                if(mbFilterActive)
                {
                    pFileName = pEntry->d_name;
                    wFileNameLen = strlen(pFileName);
                    pFilterList = mpCurrentFilter->GetFilterList();
                    for(WORD i = 0; i < mpCurrentFilter->GetFilterCount(); i++)
                    {
                        wFilterLen = strlen(pFilterList[i]);
                        pFileName += (wFileNameLen - wFilterLen);
                        if(!(strncmp(pFileName, pFilterList[i], wFilterLen)))
                        {
                            dwEntries++;
                            break;
                        }
                        pFileName = pEntry->d_name;
                    }
                }
                else
                {
                    if(strncmp(pEntry->d_name, ".", 1) != 0)
                    {
                        dwEntries++;
                    }
                }
            }
        }
    }
    
    if(dwEntries == 0)
    {
        closedir(pDir);
        return;
    }
    
    char* pEntries = new char[dwEntries * PFD_BUFFERLEN];
    char* pCurEntry = pEntries;
    
    // Rewind the directory and get the names.
    seekdir(pDir, dwPos);
    
    while((pEntry = readdir(pDir)) != NULL)
    {
		stat(pEntry->d_name, &tStat);

		if(!(S_ISDIR(tStat.st_mode)))
		{
            if(mbViewAll)
            {
                strcpy(pCurEntry, pEntry->d_name);
                pCurEntry += PFD_BUFFERLEN;
            }
            else
            {
                if(mbFilterActive)
                {
                    pFileName = pEntry->d_name;
                    wFileNameLen = strlen(pFileName);
                    pFilterList = mpCurrentFilter->GetFilterList();
                    for(WORD i = 0; i < mpCurrentFilter->GetFilterCount(); i++)
                    {
                        wFilterLen = strlen(pFilterList[i]);
                        pFileName += (wFileNameLen - wFilterLen);
                        if(!(strncmp(pFileName, pFilterList[i], wFilterLen)))
                        {
                            strcpy(pCurEntry, pEntry->d_name);
                            pCurEntry += PFD_BUFFERLEN;
                            break;
                        }
                        pFileName = pEntry->d_name;
                    }
                }
                else
                {
                    //if(strncmp(pEntry->d_name, ".", 1) != 0)
                    if(!(pEntry->d_name[0] == '.'))
                    {
                        strcpy(pCurEntry, pEntry->d_name);
                        pCurEntry += PFD_BUFFERLEN;
                    }
                }
            }
		}
	}
    
    // Sort the list
    qsort(pEntries, dwEntries, PFD_BUFFERLEN, SortFileList);
    
    // Add the prompts to the file list
    pCurEntry = pEntries;
    PegPrompt* pPrompt;

   #if defined(PEG_UNICODE)
    PEGCHAR* pCurEntryData = new PEGCHAR[PFD_BUFFERLEN + 1];
   #endif

    for(dwCount = 0; dwCount < dwEntries; dwCount++)
    {
       #if defined(PEG_UNICODE)
        strcpy(pCurEntryData, pCurEntry);
        pPrompt = new PegPrompt(0, 0, pCurEntryData, dwCount + 1,
                                TJ_LEFT | TT_COPY | FF_NONE | AF_ENABLED);
       #else
        pPrompt = new PegPrompt(0, 0, pCurEntry, dwCount + 1,
                                TJ_LEFT | TT_COPY | FF_NONE | AF_ENABLED);
       #endif
        pPrompt->SetFont(&MenuFont);
        
        if(mbSortFileLowToHigh)
        {
            mpFileList->AddToEnd(pPrompt, FALSE);
        }
        else
        {
            mpFileList->Add(pPrompt, FALSE);
        }

        pCurEntry += PFD_BUFFERLEN;
    }
    
	closedir(pDir);
    delete [] pEntries;

   #if defined(PEG_UNICODE)
    delete [] pCurEntryData;
   #endif

    mpFileList->Draw();
}

/*---------------------------------------------------------------------------*/
void PegFileDialog::ClearDirList()
{
    PegThing* pThing = mpDirList->First();
    PegThing* pDelThing;
    
    while(pThing)
    {
        if(pThing->Type() == TYPE_PROMPT)
        {
            pDelThing = pThing;
            pThing = pThing->Next();
            mpDirList->Remove(pDelThing, FALSE);
            Destroy(pDelThing);
        }
        else
        {
            pThing = pThing->Next();
        }
    }
    
    mpDirList->Draw();
}

/*---------------------------------------------------------------------------*/
void PegFileDialog::ClearFileList()
{
    PegThing* pThing = mpFileList->First();
    PegThing* pDelThing;
    
    while(pThing)
    {
        if(pThing->Type() == TYPE_PROMPT)
        {
            pDelThing = pThing;
            pThing = pThing->Next();
            mpFileList->Remove(pDelThing, FALSE);
            Destroy(pDelThing);
        }
        else
        {
            pThing = pThing->Next();
        }
    }
    
    mpFileList->Draw();
}

/*---------------------------------------------------------------------------*/
void PegFileDialog::ClearFileName()
{
    mpFileString->DataSet(NULL);
    mpFileString->Draw();
}

/*---------------------------------------------------------------------------*/
void PegFileDialog::UpdatePresentDir()
{
   #if defined(PEG_UNICODE)
    WORD wLen = strlen(mpLastValidDir);
    PEGCHAR* pData = new PEGCHAR[wLen + 1];
    strcpy(pData, mpLastValidDir);
    mpPresentDir->DataSet(pData);
   #else
    mpPresentDir->DataSet(mpLastValidDir);
   #endif
    mpPresentDir->Draw();
}

/*---------------------------------------------------------------------------*/
BOOL PegFileDialog::BuildNewDirName(const char* pAppendDir)
{
    WORD wAppendLen = strlen(pAppendDir);
    WORD wCurLen = strlen(mpLastValidDir);
    
    char* pNewDir = new char[wAppendLen + wCurLen + 2];
    //memset(pNewDir, '\0', wAppendLen + wCurLen + 2);
    
    strcpy(pNewDir, mpLastValidDir);
    
    if(strncmp(&pNewDir[wCurLen - 1], "/", 1) != 0)
    {
        strcat(pNewDir, "/");
    }
    
    strcat(pNewDir, pAppendDir);
    
    TryChangeDir(pNewDir);
    
    delete [] pNewDir;
    
    return(TRUE);
}

/*---------------------------------------------------------------------------*/
char* PegFileDialog::BuildFileName()
{
   #if defined(PEG_UNICODE)
    char* pFile = ConvertDataGet(mpFileString->DataGet());
   #else
    const char* pFile = mpFileString->DataGet();
   #endif

    WORD wFileLen = strlen(pFile);
    WORD wDirLen = strlen(mpLastValidDir);
    
    char* pFileName = new char[wDirLen + wFileLen + 2];
    strcpy(pFileName, mpLastValidDir);
    
    if(strncmp(&pFileName[wDirLen - 1], "/", 1) != 0)
    {
        strcat(pFileName, "/");
    }
    
    strcat(pFileName, pFile);

   #if defined(PEG_UNICODE)
    delete [] pFile;
   #endif
    
    return(pFileName);
}

/*---------------------------------------------------------------------------*/
BOOL PegFileDialog::CheckFileExists(const char* pFileName)
{
    FILE* pFile;
    pFile = fopen(pFileName, "r");
    if(pFile == NULL)
    {
        return(FALSE);
    }
    else
    {
        fclose(pFile);
        return(TRUE);
    }
}

/*---------------------------------------------------------------------------*/
BOOL PegFileDialog::CheckFileCreate(const char* pFileName)
{
    FILE* pFile;
    pFile = fopen(pFileName, "w");
    if(pFile == NULL)
    {
        return(FALSE);
    }
    else
    {
        fclose(pFile);
        return(TRUE);
    }
}

/*---------------------------------------------------------------------------*/
char* PegFileDialog::AddDefExtToFileName(char* pFileName)
{
    WORD wLen = strlen(pFileName);
    if(!wLen)
    {
        return(NULL);
    }
    
    BOOL bFoundDirSep = FALSE;
    BOOL bFoundDot = FALSE;
    WORD wCur = wLen;

    while(wCur--)
    {
       if(pFileName[wCur] == '/')
       {
           bFoundDirSep = TRUE;
           break;
       }

       if(pFileName[wCur] == '.')
       {
           bFoundDot = TRUE;
           break;
       }
    }

    char* pNewFileName = NULL;
    if(bFoundDirSep && !bFoundDot)
    {
        WORD wExtLen = strlen(mpDefNewExt) + 2;
        pNewFileName = new char[wLen + wExtLen];
        strcpy(pNewFileName, pFileName);
        strcat(pNewFileName, ".");
        strcat(pNewFileName, mpDefNewExt);
    }

    return(pNewFileName);
}

/*---------------------------------------------------------------------------*/
BOOL PegFileDialog::ValidateEntry()
{
    BOOL bRetVal = FALSE;

    if(!(mpFileString->DataGet()))
    {
        PEGCHAR* pMsg = NULL;
        if(miOperation & PFD_FILEOPEN)
        {
            pMsg = gsSelectFile;
        }
        else if(miOperation & PFD_FILESAVEAS)
        {
            pMsg = gsEnterFile;
        }

        PegMessageWindow* pWin = new PegMessageWindow(gsFileError, pMsg);
        if(pWin)
        {
            pWin->Execute();
        }
        return(bRetVal);
    }

    char* pFileName = BuildFileName();

    if(GetOperation() & PFD_FILEOPEN)
    {
        BOOL bExists = TRUE;
        if(GetOperation() & PFD_FILEMUSTEXIST)
        {
            bExists = CheckFileExists(pFileName);
        }

        if(bExists)
        {
            if(strlen(pFileName) < mwFileNameBufferLen)
            {
                strcpy(mpFileNameBuffer, pFileName);
                bRetVal = TRUE;
            }
        }

    }
    else if(GetOperation() & PFD_FILESAVEAS)
    {
        char* pNewFileName;
        if(mpDefNewExt)
        {
            pNewFileName = AddDefExtToFileName(pFileName);
            if(pNewFileName)
            {
                delete pFileName;
                pFileName = pNewFileName;
            }
        }

        BOOL bExists = CheckFileExists(pFileName);

        BOOL bUserOkOverwrite = TRUE;
        if((GetOperation() & PFD_OVERWRITEPROMPT) && bExists)
        {
            PegMessageWindow* pWin = new PegMessageWindow(Title(),
                    gsOverwriteOk, MW_OK | MW_NO | FF_RAISED);
            SIGNED iVal = pWin->Execute();
            if(iVal == IDB_NO)
            {
                bUserOkOverwrite = FALSE;
            }
        }

        if(!bUserOkOverwrite)
        {
            delete pFileName;
            return(bRetVal);
        }

        BOOL bCreatedTestFile = TRUE;
        if((GetOperation() & PFD_CREATETESTFILE) && !bExists)
        {
            bCreatedTestFile = CheckFileCreate(pFileName);

            if(!bCreatedTestFile)
            {
                bCreatedTestFile = FALSE;

                PegMessageWindow* pWin = new PegMessageWindow(Title(),
                                                              gsNoCreateFile);
                pWin->Execute();
            }
        }

        if(!bCreatedTestFile)
        {
            delete pFileName;
            return(bRetVal);
        }

        if(strlen(pFileName) < mwFileNameBufferLen)
        {
            strcpy(mpFileNameBuffer, pFileName);
            bRetVal = TRUE;
        }

        bRetVal = TRUE;
    }

    delete pFileName;

    return(bRetVal);
}

#if defined(PEG_UNICODE)
/*---------------------------------------------------------------------------*/
char* PegFileDialog::ConvertDataGet(const PEGCHAR* pData)
{
    if(!pData)
    {
        return(NULL);
    }

    char* pCharData = new char[strlen(pData) + 1];
    UnicodeToAscii(pCharData, pData);

    return(pCharData);
}
#endif

/*---------------------------------------------------------------------------*/
void PegFileDialog::ResetFilter()
{
    if(mpFilterList)
    {
        if(mwFilterCount)
        {
            for(WORD i = 0; i < mwFilterCount; i++)
            {
                delete mpFilterList[i];
            }
        }
    }

    mpFilterList = NULL;
    mwFilterCount = 0;
    mpCurrentFilter = NULL;
}

/*---------------------------------------------------------------------------*/
void PegFileDialog::StoreFilter(const char* pFilter)
{
    if(pFilter == NULL)
    {
        return;
    }

    // We're expecting this to look like this:
    // "Filter Name\0*.bmp;*.jpg;*.gif;*.png\0Another Filter\0*.cmd\0\0"
    // Pretty much the same way it works in Win32, but, we're only 
    // applying a single filter, although we're storing all of them.
    // We're also not doing anything fancy for pattern matching, just
    // *.something (the file extension).

    WORD wCount = 0;
    const char* pCur = pFilter;
    const char* pLast = pCur;
    WORD wNumFilters = 0;
    BOOL bFirst = FALSE;
    
    while(1)
    {
        if(*pCur == '\0')
        {
            if(bFirst)
            {
                wNumFilters++;
                bFirst = FALSE;
            }
            else
            {
                bFirst = TRUE;
            }
        }

        pCur++;
        wCount++;

        if(*pCur == '\0' && *pLast == '\0')
        {
            break;
        }

        pLast = pCur;
    }

    pCur = pFilter;
    pLast = pCur;
    bFirst = FALSE;
    char* pCurFilter = NULL;
    WORD wCurLen = 0;
    WORD wCurCount = 0;

    if(wNumFilters)
    {
        mpFilterList = new PegFileFilter*[wNumFilters];

        for(WORD i = 0; i < wCount + 1; i++)
        {
            if(*pCur == '\0')
            {
                if(bFirst)
                {
                    pCurFilter = new char[wCurLen + 1];
                    memcpy(pCurFilter, pLast, wCurLen + 1);

                    mpFilterList[wCurCount] = new PegFileFilter(pCurFilter);

                    bFirst = FALSE;
                    wCurLen = 0;
                    delete [] pCurFilter;
                    pLast = pCur + 1;
                    wCurCount++;
                }
                else
                {
                    bFirst = TRUE;
                }
            }

            pCur++;
            wCurLen++;
        }

        mwFilterCount = wNumFilters;
    }
    else
    {
        ResetFilter();
    }
}

/*---------------------------------------------------------------------------*/
void PegFileDialog::UpdateFilter()
{
    if(mbFilterActive && mpCurrentFilter)
    {
        char cBuffer[1024];
        WORD wBufLen = 1024;
        
        FormatFilter(cBuffer, wBufLen, mpCurrentFilter);

       #if defined(PEG_UNICODE)
        PEGCHAR ucBuffer[1024];
        strcpy(ucBuffer, cBuffer);
        mpFilter->DataSet(ucBuffer);
       #else
        mpFilter->DataSet(cBuffer);
       #endif
        mpFilter->Draw();
    }
    else
    {
        mpFilter->DataSet(gsNoActiveFilter);
        mpFilter->Draw();
    }
}

/*---------------------------------------------------------------------------*/
void PegFileDialog::FormatFilter(char* pBuffer, WORD wBufLen, 
                                 const PegFileFilter* pFilter)
{
    if(!pFilter)
    {
        return;
    }

    WORD wFilterCount = pFilter->GetFilterCount();

    if(!wFilterCount)
    {
        return;
    }

    WORD wFilterLen = strlen(pFilter->GetFilterName());
    char** pCurFilter = pFilter->GetFilterList();
    WORD i;
    for(i = 0; i < wFilterCount; i++)
    {
        wFilterLen += strlen(pCurFilter[i]);
    }

    wFilterLen += 3 + (2 * wFilterCount);

    if(wFilterLen > wBufLen)
    {
        if(strlen(pFilter->GetFilterName()) < wBufLen)
        {
            strcpy(pBuffer, pFilter->GetFilterName());
            return;
        }
    }

    char pTempFilter[32];
    strcpy(pBuffer, pFilter->GetFilterName());
    strcat(pBuffer, " (");
    for(i = 0; i < wFilterCount; i++)
    {
        strcpy(pTempFilter, "*");
        strcat(pTempFilter, pCurFilter[i]);
        //sprintf(pTempFilter, "*.%s", pCurFilter[i]);
        if(i + 1 < wFilterCount)
        {
            strcat(pTempFilter, ",");
        }
        strcat(pBuffer, pTempFilter);
    }

    strcat(pBuffer, ")");
}

#endif  // PEG_FILE_DIALOG

