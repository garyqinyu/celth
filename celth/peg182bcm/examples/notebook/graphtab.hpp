/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// graphtab.hpp - A simple class for diplaying text and a small graphic.
//    This class is used by the notebook demo to illustrate using custom
//    notebook tabs. This could be used for many other things as well.
//
// Author: Kenneth G. Maxwell
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


class GraphicTab : public PegThing, public PegTextThing
{
    public:
        GraphicTab(PegBitmap *pBm, char *Text = NULL);
        void Draw(void);

    private:
        PegBitmap *mpBitmap;
};

