

#include "peg.hpp"

/*---------------------------------------------------------------*/
ROMDATA PEGCHAR gsIDL_ENG_1[] = 
{   0x0042, 0x0055, 0x0054, 0x0054, 0x004f, 0x004e, 0};
ROMDATA PEGCHAR gsIDL_ENG_2[] = 
{   0x0042, 0x0055, 0x0054, 0x0054, 0x004f, 0x004e, 0};
ROMDATA PEGCHAR gsIDL_ENG_3[] = 
{   0x0042, 0x0055, 0x0054, 0x0054, 0x004f, 0x004e, 0};



/*---------------------------------------------------------------*/
ROMDATA PEGCHAR gsIDL_CHI_1[] = 
{   0x6309, 0x94ae, 0x0031, 0x0031, 0x0031, 0x0031, 0x0031, 0x0031, 0};
ROMDATA PEGCHAR gsIDL_CHI_2[] = 
{   0x6309, 0x94ae, 0x0031, 0x0031, 0x0031, 0x0031, 0x0031, 0x0031, 0};
ROMDATA PEGCHAR gsIDL_CHI_3[] = 
{   0x6309, 0x94ae, 0x0031, 0x0031, 0x0031, 0x0031, 0x0031, 0x0031, 0};



/*---------------------------------------------------------------*/
ROMDATA PEGCHAR *IDL_ENGLISH_Table[] = {
    gsIDL_ENG_1,
    gsIDL_ENG_2,
    gsIDL_ENG_3
};


/*---------------------------------------------------------------*/
ROMDATA PEGCHAR *IDL_CHINESE_Table[] = {
    gsIDL_CHI_1,
    gsIDL_CHI_2,
    gsIDL_CHI_3
};


/*---------------------------------------------------------------*/
ROMDATA PEGCHAR **wbStringTable1[2] = {
    IDL_ENGLISH_Table,
    IDL_CHINESE_Table
};

/*---------------------------------------------------------------*/
UCHAR gbCurrentLanguage1= 1;

ROMDATA PEGCHAR *LookupString1(WORD wSID)
{
    return wbStringTable1[gbCurrentLanguage1][wSID];
}

