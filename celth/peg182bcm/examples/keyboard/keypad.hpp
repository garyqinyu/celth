/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//    Copyright (c) 1999-2002, Swell Software, Inc.
//  
//    All Rights Reserved
//  
//  Notes:
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class KeypadButton :  public PegTextButton
{
    public:

        KeypadButton(PegRect &, char *text, WORD wId = 0,
            WORD wStyle = AF_ENABLED|BF_REPEAT|BF_DOWNACTION|TT_COPY);
    private:
        void DrawFocusIndicator(BOOL bDraw);
        void EraseFocusIndicator(void);
};

#ifdef NO_MOUSE

struct MoveEntry {
    char cUpper;
    char cLower;
    char cGoUp;
    char cGoDown;
    char cGoLeft;
    char cGoRight;
};

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class KeyboardWin : public PegDialog
{
    public:
        KeyboardWin();
        SIGNED Message(const PegMessage &Mesg);
        enum ChildIds {
            IDB_CLEAR = 1,
            IDB_SHIFT,
            IDB_LETTER,
            IDB_BACK,
            IDP_VALUE,
        };

    private:
        void ToggleShift(void);

        #ifdef NO_MOUSE
        void CheckMoveFocus(SIGNED iKey);
        KeypadButton *FindCurrent(void);
        MoveEntry *FindMoveEntry(PegThing *pCurrent);
        void MoveToLetter(KeypadButton *pCurrent, MoveEntry *pMove,
            int iDir);
        void MoveSpecial(KeypadButton *pCurrent, int iDir);
        #endif

        //PegPrompt *mpValue;
        PegString *mpValue;
        BOOL mbCaps;
        KeypadButton *mpZKey;
        KeypadButton *mpQuestionKey;
        KeypadButton *mpSpaceKey;
        KeypadButton *mpOneKey;

};
