/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pstpchrt.hpp - Strip chart class definition.
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
// Notes: Example for ESC West
// TODO: Add into the Peg ChartLib
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifndef INC_PEGSTRIPCHART_
#define INC_PEGSTRIPCHART_


// Style flags. Will be incorporated into pegtypes.hpp later
const WORD SC_SCROLLED =    0x0100;     // Scrolled view where the head is always
                                        // the right client edge. Data is added 
                                        // on the right and removed from the left.
const WORD SC_PAGED =       0x0200;     // Paged view that flips when the head
                                        // reaches the right client edge. Data
                                        // is added on the right and removed
                                        // from the left. Does not support a 
                                        // history.
const WORD SC_DRAWXAXIS =   0x0400;     // Draw a horizontal line representing
                                        // the x axis where y = 0
const WORD SC_DRAWAGED =    0x0800;     // Only used in SC_PAGED mode. draw the
                                        // aged data from the previous screen
                                        // in a darker color

// Thing type byte. Will also be added to pegtypes.hpp later,
// in the proper order. This is just inordinately high so there
// are no possiblities of conflicts until it's merged in. Also,
// we should group all of the HMI controls together and perhaps
// define a low and high to easily determine if the object is
// an HMI control...?
const UCHAR TYPE_HMI_STRIPCHART     = 212;

// Forward reference
class PegStripChart;

class PegStripChartLine
{
public:
    PegStripChartLine(PegStripChart* parent, BYTE id, PegColor color, 
                      WORD historySize = 0);
    virtual ~PegStripChartLine();

    BOOL AddData(WORD data);

    // Accessors
    PegStripChartLine* GetNext() const { return mpNext; }
    BYTE GetID() const { return mcID; }
    WORD GetHead() const { return mwHead; }
    WORD GetTail() const { return mwTail; }
    WORD GetViewHead() const { return mwViewHead; }
    WORD GetViewTail() const { return mwViewTail; }
    WORD GetHistorySize() const { return mwHistorySize; }
    WORD* GetData() const { return mpData; }
    WORD GetData(WORD index) { return mpData[index]; }
    PegColor GetColor() const { return mColor; }

    // Mutators
    void SetNext(PegStripChartLine* line) { mpNext = line; }
    PegColor SetColor(PegColor color) { mColor = color; return mColor; }
    
private:
    BYTE        mcID;
    WORD        mwHead;
    WORD        mwTail;
    WORD        mwViewHead;
    WORD        mwViewTail;
    WORD        mwHistorySize;
    WORD        mwRecordLength;
    WORD*       mpData;
    PegColor    mColor;
    PegStripChartLine* mpNext;
    PegStripChart* mpParent;
};

class PegStripChart : public PegWindow
{
public:
    PegStripChart(const PegRect& rect, WORD style, WORD xExtent, LONG minY, LONG maxY);
    ~PegStripChart();

    void Draw();
    SIGNED Message(const PegMessage& msg);

    // returns the indice of the new line
    BYTE AddLine(const PegColor& color, WORD historySize);
    // returns success/failure
    BOOL RemoveLine(BYTE id);

    // returns the new, converted data or (UINT)-1 if it blew.
    // this is a wrapper for PegStripChartLine::AddData(WORD),
    // which passes back a boolean to describe it's data 
    // adding experience...
    WORD AddData(BYTE id, LONG rawData);
    // called by AddData to prepare the data for
    // passing over to the line 
    //WORD NormalizeData(LONG rawData);

    //WORD CalculateYScale();
    SIGNED ValToPointY(LONG lVal);
    SIGNED IndexToPoint(WORD wIndex);
    
    WORD GetClientDataWidth() const { return mwClientDataWidth; }
    
private:
    WORD mwXExtent;
    LONG mlMinY;
    LONG mlMaxY;
    //WORD mwDataPerY;
    WORD mwClientDataWidth;
    BYTE mcLineID;
    PegStripChartLine* mpLines;
};

#endif // INC_PEGSTRIPCHART_
