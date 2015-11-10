

enum STRING_IDS {
    SID_1 = 0,
    SID_2,
    SID_3
};


const PEGCHAR  *LookupString1(WORD wSID);

#define LS1(a) LookupString1(a)

