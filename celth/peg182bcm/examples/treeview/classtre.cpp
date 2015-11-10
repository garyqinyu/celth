/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// classtre.hpp - Example using PegTreeView that displays the PEG library 
//                hierarchy.
//
// Author: Jim DeLisle
//
// Copyright (c) 1997-1999 Swell Software 
//              All Rights Reserved.
//
// Unauthorized redistribution of this source code, in whole or part,
// without the express written permission of Swell Software
// is strictly prohibited.
//
// Notes:
//
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include "peg.hpp"
#include "classtre.hpp"

/*--------------------------------------------------------------------------*/
// Bitmaps used for the tree nodes
/*--------------------------------------------------------------------------*/
extern PegBitmap gbdotBitmap;
extern PegBitmap gbgreendotBitmap;
extern PegBitmap gbgreydotBitmap;
extern PegBitmap gbpegtopBitmap;
extern PegBitmap gbreddotBitmap;

/*--------------------------------------------------------------------------*/
// Add a new ClassTreeWin to the presentation manager
/*--------------------------------------------------------------------------*/
void PegAppInitialize(PegPresentationManager* pPresentation)
{
	PegRect Rect;

	Rect.Set(10, 10, 400, 400);

	pPresentation->Add(new ClassTreeWin(Rect));
}

/*--------------------------------------------------------------------------*/
// Peg Class library hierarchy divided into node levels.
/*--------------------------------------------------------------------------*/
static PEGCHAR szRootNodes[][20] =
	{ "PegThing",
	  "PegTextThing",
	  "PegImageConvert",
	  "PegMessageQueue",
	  "PegScreen",
	  "PegQuant",
	  '\0'
	};

static PEGCHAR szThingNodes[][20] =
	{ "PegIcon",
	  "PegSlider",
	  "PegHScroll",
	  "PegVScroll",
	  "PegGroup",
	  "PegTitle",
	  "PegStatusBar",
	  "PegPrompt",
	  "PegVPrompt",
	  "PegString",
	  "PegSpinButton",
	  "PegMenu",
	  "PegMenuButton",
	  "PegMenuBar",
	  "PegWindow",
	  "PegButton",
	  "PegChart",
	  '\0'
	};

static PEGCHAR szWindowNodes[][23] =
	{ "PegTable",
	  "PegTextBox",
	  "PegList",
	  "PegNotebook",
	  "PegPresentationManager",
	  "PegSpreadSheet",
	  "PegAnimationWindow",
	  "PegTreeView",
	  "PegDecoratedWindow",
	  '\0'
	};

static PEGCHAR szListNodes[][20] =
	{ "PegVertList",
	  "PegHorzList",
	  '\0'
	};

static PEGCHAR szVListNodes[][20] =
	{ "PegComboBox",
	  '\0'
	};

static PEGCHAR szDecWindowNodes[][20] =
	{ "PegDialog",
	  "PegMessageWindow",
	  '\0'
	};

static PEGCHAR szButtonNodes[][20] =
	{ "PegTextButton",
	  "PegMLTextButton",
	  "PegBitmapButton",
	  "PegCheckBox",
	  "PegRadioButton",
	  '\0'
	};

static PEGCHAR szChartNodes[][20] =
	{ "PegLineChart",
	  "PegMultiLineChart",
	  "PegStripChart",
	  '\0'
	};

static PEGCHAR szImageConvertNodes[][20] =
	{ "PegBmpConvert",
	  "PegGifConvert",
	  "PegJpgConvert",
	  '\0'
	};

/*--------------------------------------------------------------------------*/
// Construct the ClassTreeWin
/*--------------------------------------------------------------------------*/
ClassTreeWin::ClassTreeWin(const PegRect& Rect) :
	PegDecoratedWindow(Rect)
{
	Add(new PegTitle("PegTreeView Example", TF_SYSBUTTON|TF_CLOSEBUTTON));
	
	RemoveStatus(PSF_SIZEABLE);

	pTree = new PegTreeView(mClient, FF_THIN, "Peg Class Tree", &gbpegtopBitmap);

	if(pTree)
	{
		InitTreeView();
		Add(pTree);
	}
}

/*--------------------------------------------------------------------------*/
// Add all of the nodes to the tree using the PEGCHAR arrays from above.
// This demonstrates how we can add nodes to a view, then retreive that
// node later and add child nodes to it.
/*--------------------------------------------------------------------------*/
void ClassTreeWin::InitTreeView()
{
	// Add the 1st level nodes to the root
	PegTreeNode* pNode = pTree->TopNode();
	PegTreeNode* pSubNode;

	WORD wIndex = 0;
	while(szRootNodes[wIndex][0])
	{
		pNode->Add(new PegTreeNode(&szRootNodes[wIndex][0], &gbdotBitmap));
		wIndex++;
	}

	pSubNode = pTree->FindNode(1, "PegThing");
	if(pSubNode)
	{
		wIndex = 0;
		while(szThingNodes[wIndex][0])
		{
			pSubNode->Add(new PegTreeNode(&szThingNodes[wIndex][0], &gbgreendotBitmap));
			wIndex++;
		}
	}

	pSubNode = pTree->FindNode(1, "PegImageConvert");
	if(pSubNode)
	{
		wIndex = 0;
		while(szImageConvertNodes[wIndex][0])
		{
			pSubNode->Add(new PegTreeNode(&szImageConvertNodes[wIndex][0], &gbreddotBitmap));
			wIndex++;
		}
	}

	pSubNode = pTree->FindNode(2, "PegWindow");
	if(pSubNode)
	{
		wIndex = 0;
		while(szWindowNodes[wIndex][0])
		{
			pSubNode->Add(new PegTreeNode(&szWindowNodes[wIndex][0], &gbdotBitmap));
			wIndex++;
		}
	}

	pSubNode = pTree->FindNode(2, "PegButton");
	if(pSubNode)
	{
		wIndex = 0;
		while(szButtonNodes[wIndex][0])
		{
			pSubNode->Add(new PegTreeNode(&szButtonNodes[wIndex][0], &gbgreydotBitmap));
			wIndex++;
		}
	}

	pSubNode = pTree->FindNode(2, "PegChart");
	if(pSubNode)
	{
		wIndex = 0;
		while(szChartNodes[wIndex][0])
		{
			pSubNode->Add(new PegTreeNode(&szChartNodes[wIndex][0], &gbdotBitmap));
			wIndex++;
		}
	}

	pSubNode = pTree->FindNode(3, "PegDecoratedWindow");
	if(pSubNode)
	{
		wIndex = 0;
		while(szDecWindowNodes[wIndex][0])
		{
			pSubNode->Add(new PegTreeNode(&szDecWindowNodes[wIndex][0], &gbreddotBitmap));
			wIndex++;
		}
	}

	pSubNode = pTree->FindNode(3, "PegList");
	if(pSubNode)
	{
		wIndex = 0;
		while(szListNodes[wIndex][0])
		{
			pSubNode->Add(new PegTreeNode(&szListNodes[wIndex][0], &gbgreydotBitmap));
			wIndex++;
		}
	}

	pSubNode = pTree->FindNode(4, "PegVertList");
	if(pSubNode)
	{
		wIndex = 0;
		while(szVListNodes[wIndex][0])
		{
			pSubNode->Add(new PegTreeNode(&szVListNodes[wIndex][0], &gbreddotBitmap));
			wIndex++;
		}
	}
}


