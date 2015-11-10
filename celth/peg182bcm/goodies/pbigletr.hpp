/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pbigletr.hpp - A prompt class that draws the first letter in a different
//      font than the rest of the string.
//
// Author: Kenneth G. Maxwell
//
// Version: 1.04
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

class BigLetterPrompt : public PegPrompt
{
    public:
        /*--------------------------------------------------------*/
        // Provide all of the same constructors as PegPrompt.
        // Some of these may be removed if they are not used:
        /*--------------------------------------------------------*/
        BigLetterPrompt(PegRect &Rect, char *Text, WORD wId);
        BigLetterPrompt(SIGNED iLeft, SIGNED iTop, SIGNED iWidth, char *Text,
            WORD wId);
        BigLetterPrompt(SIGNED iLeft, SIGNED iTop, char *Text, WORD wId);

        void Draw(void);

    private:
};


