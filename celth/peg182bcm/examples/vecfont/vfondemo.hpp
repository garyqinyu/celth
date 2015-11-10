/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vfondemo.hpp - Example using vector fonts.
//
// Author: Kenneth G. Maxwell
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
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

enum MyMenuIds {
            IDB_BOLD_FONT = 1,
            IDB_ITALIC_FONT,
            IDB_FS_6,
            IDB_FS_7,
            IDB_FS_8,
            IDB_FS_9,
            IDB_FS_10,
            IDB_FS_12,
            IDB_FS_14,
            IDB_FS_16,
            IDB_FS_18,
            IDB_FS_20,
            IDB_FS_24,
            IDB_FS_30,
            IDB_FS_36,
            IDB_FS_40
        };

class FontWin : public PegDialog
{
	public:
		FontWin(const PegRect &Rect);
        ~FontWin();
		SIGNED Message(const PegMessage &Mesg);

	private:
        

        void ResetFont(void);
        void AppendLine(void);

        PegTextBox *mpText;
        PegFont *mpFont;
        SIGNED miSize;
        SIGNED miCount;
        BOOL   mbBold;
        BOOL   mbItalic;
};


