/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// polywin.hpp - Demonstration of using the PegPolygonThing
//  
// Author: Jim DeLisle
//
// Copyright (c) 1997-2002 Swell Software 
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

#ifndef POLYGON_WINDOW
#define POLYGON_WINDOW

class PolygonWindow : public PegDecoratedWindow
{
    public:
        PolygonWindow();

        // Overrides
        SIGNED Message(const PegMessage& Mesg);

    private:
        void MovePolygon();

        Peg2DPolygon* mpPolygon;

        SIGNED miMoveX;
        SIGNED miMoveY;
        SIGNED miMoveAmount;
        SIGNED miRotDir;
        SIGNED miCurAngle;
};

#endif  // POLYGON_WINDOW

