/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pliteral.cpp - Literal strings used by PEG. Defined for either UNICODE
//  or normal 8-bit ASCII encoding.
//
// Author: Kenneth G. Maxwell
//
// Copyright (c) 1997-2000 Swell Software 
//              All Rights Reserved.
//
// Unauthorized redistribution of this source code, in whole or part,
// without the express written permission of Swell Software
// is strictly prohibited.
//
// Notes:
//
// This file contains every string literal used by PEG. Each string is
// encoded both in 8-bit and in 16-bit form.
// 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include "peg.hpp"

#ifdef PEG_UNICODE

PEGCHAR lsOK[] = {'O','K','\0'};
PEGCHAR lsNO[] = {'N','o','\0'};
PEGCHAR lsYES[] = {'Y','e','s','\0'};
PEGCHAR lsABORT[] = {'A','b','o','r','t','\0'};
PEGCHAR lsCANCEL[] = {'C','a','n','c','e','l','\0'};
PEGCHAR lsRETRY[] =  {'R','e','t','r','y','\0'};
PEGCHAR lsCLOSE[] = {'C','l','o','s','e','\0'};
PEGCHAR lsAPPLY[] = {'A','p','p','l','y', 0};
PEGCHAR lsTEST[]   = {'A','\0'};
PEGCHAR lsMINIMIZE[] = {'M','i','n','i','m','i','z','e','\0'};
PEGCHAR lsMAXIMIZE[] = {'M','a','x','i','m','i','z','e','\0'};
PEGCHAR lsNULL[]     = {'\0'};
PEGCHAR lsRESTORE[]  = {'R','e','s','t','o','r','e','\0'};
PEGCHAR lsSPACE[]    = {' ','\0'};
PEGCHAR lsHELP[]     = {'H','e','l','p', 0};

#ifdef  PEG_IMAGE_CONVERT
PEGCHAR lsNotAllowed[] = {'I','n','l','i','n','e', ' ','C','o','n','v','e','r','s','i','o','n',' ',
    'o','f', ' ','2','4','b','p','p',' ','i','m','a','g','e','s',' ',
    'i','s',' ','n','o','t',' ','s','u','p','p','o','r','t','e','d','\0'};
PEGCHAR lsBadMagic[] = {'E','r','r','o','r',' ','r','e','a','d','i',
    'n','g',' ','m','a','g','i','c',' ','n','u','m','b','e','r','.','\0'};
PEGCHAR lsNotAGif[] = {'N','o','t',' ','a',' ','G','I','F',' ','f',
    'i','l','e','.','\0'};
PEGCHAR lsBadGifVer[] = {'B','a','d',' ','v','e','r','s','i','o','n',
    ' ','n','u','m','b','e','r',',',' ','n','o','t',' ',
    '\'','8','7','a','\'',' ','o','r',' ','\'','8','9','a','\'','.','\0'};
PEGCHAR lsScrnDescFail[] = {'F','a','i','l','e','d',' ','t','o',
    ' ','r','e','a','d',' ','s','c','r','e','e','n',' ',
    'd','e','s','c','r','i','p','t','o','r','.','\0'};
PEGCHAR lsGifRdErr[] = {'R','e','a','d',' ','e','r','r','o','r',' ',
    'o','n',' ','i','m','a','g','e',' ','d','a','t','a','.','\0'};
PEGCHAR lsBadColormap[] = {'B','a','d',' ','C','o','l','o','r','m','a','p','\0'};
PEGCHAR lsOutOfMem[] = {'O','u','t',' ','O','f',' ',
    'M','e','m','o','r','y','\0'};
PEGCHAR lsUnsupported[] = {'U','n','s','u','p','p','o','r','t','e','d',
    ' ','I','m','a','g','e',' ','T','y','p','e',0};
#endif

#else

PEGCHAR lsOK[] = "OK";
PEGCHAR lsNO[] = "No";
PEGCHAR lsYES[] = "Yes";
PEGCHAR lsABORT[] = "Abort";
PEGCHAR lsCANCEL[] = "Cancel";
PEGCHAR lsRETRY[] = "Retry";
PEGCHAR lsCLOSE[] = "Close";
PEGCHAR lsAPPLY[] = "Apply";
PEGCHAR lsTEST[]   = "A";
PEGCHAR lsMINIMIZE[] = "Minimize";
PEGCHAR lsMAXIMIZE[] = "Maximize";
PEGCHAR lsNULL[]     = "";
PEGCHAR lsRESTORE[]  = "Restore";
PEGCHAR lsSPACE[]    = " ";
PEGCHAR lsHELP[] = "Help";

#ifdef PEG_IMAGE_CONVERT
PEGCHAR lsNotAllowed[] = "Inline Conversion of 24bpp images is not allowed";
PEGCHAR lsBadMagic[] = "Error reading magic number.";
PEGCHAR lsNotAGif[] = "Not a GIF file.";
PEGCHAR lsBadGifVer[] = "Bad version number, not '87a' or '89a'.";
PEGCHAR lsScrnDescFail[] = "Failed to read screen descriptor.";
PEGCHAR lsGifRdErr[] = "Read error on image data.";
PEGCHAR lsBadColormap[] = "Bad Colormap";
PEGCHAR lsOutOfMem[] = "Out Of Memory";
PEGCHAR lsUnsupported[] = "Unsupported Image Type";
#endif

#endif

#ifdef PEG_STRLIB

/*--------------------------------------------------------------------------*/
//
// The PEG string library- Not a full ANSI string manipulation library!! This
// library provides all of the string functions used by PEG, in a format
// capable of supporting both 8 and 16 bit characters.
//
/*--------------------------------------------------------------------------*/
PEGCHAR *PegStrCat(PEGCHAR *s1, const PEGCHAR *s2)
{
    PEGCHAR *sr = s1;
    while(*s1)
    {
        s1++;
    }
    while(*s2)
    {
        *s1++ = *s2++;
    }
    *s1 = *s2;
    return sr;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PEGCHAR *PegStrnCat(PEGCHAR *s1, const PEGCHAR *s2, int iMax)
{
    PEGCHAR *sr = s1;
    while(*s1)
    {
        s1++;
    }
    while(*s2 && iMax--)
    {
        *s1++ = *s2++;
    }
    if (iMax > 0)
    {
        *s1 = *s2;
    }
    return sr;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PEGCHAR *PegStrCpy(PEGCHAR *s1, const PEGCHAR *s2)
{
    PEGCHAR *sr = s1;
    while(*s2)
    {
        *s1++ = *s2++;
    }
    *s1 = *s2;
    return sr;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PEGCHAR *PegStrnCpy(PEGCHAR *s1, const PEGCHAR *s2, int iMax)
{
    PEGCHAR *sr = s1;
    while(*s2 && iMax--)
    {
        *s1++ = *s2++;
    }
    if (iMax > 0)
    {
        *s1 = *s2;
    }
    return sr;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
int PegStrCmp(const PEGCHAR *s1, const PEGCHAR *s2)
{
    PEGCHAR cVal2 = *s2++;
    while(cVal2)
    {
        if (*s1++ != cVal2)
        {
            if (s1[-1] < cVal2)
            {
                return -1;
            }
            return 1;
        }
        cVal2 = *s2++;
    }

    // if we get to here, then s2 ended
    if (*s1)            // s1 is still going??
    {
        return 1;
    }
    return 0;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
int PegStrnCmp(const PEGCHAR *s1, const PEGCHAR *s2, int iMax)
{
    int i, j;
    i = PegStrLen(s1);
    j = PegStrLen(s2);
    
    if (iMax > j)
    {
        iMax = j;
    }
    if (iMax > i)
    {
        iMax = i;
    }
    while(iMax--)
    {
        if (*s1++ != *s2++)
        {
            if (s1[-1] < s2[-1])
            {
                return -1;
            }
            return 1;
        }
    }
    return 0;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
int PegStrLen(const PEGCHAR *s1)
{
    int iLen =0;
    while(*s1++)
    {
        iLen++;
    }
    return iLen;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
long PegAtoL(const PEGCHAR *s1)
{
    long j = 0;
    int sign = 0;

    if (*s1 == '+')
    {
	    s1++;
    }
    else
    {
        if (*s1 == '-')
        {
	        s1++;
	        sign--;
	    }
    }
    while (*s1 >= '0' && *s1 <= '9')
    {
	    j = j * 10 + (*s1 - '0');
	    s1++;
    }
	if(sign)
	return(-j);
    else
	return (j);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
int PegAtoI(const PEGCHAR *s1)
{
    int j = 0;
    int sign = 0;

    if (*s1 == '+')
    {
	    s1++;
    }
    else
    {
        if (*s1 == '-')
        {
	        s1++;
	        sign--;
	    }
    }
    while (*s1 >= '0' && *s1 <= '9')
    {
	    j = j * 10 + (*s1 - '0');
	    s1++;
    }
	if(sign)
	return(-j);
    else
	return (j);
}

#ifdef PEG_UNICODE

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
int PegStrCmp(const char *s1, const char *s2)
{
    char cVal2 = *s2++;
    while(cVal2)
    {
        if (*s1++ != cVal2)
        {
            if (s1[-1] < cVal2)
            {
                return -1;
            }
            return 1;
        }
        cVal2 = *s2++;
    }

    if (*s1)        // if we get to here, then s2 terminated
    {
        return 1;   // s1 is still going
    }
    return 0;       // they both terminated
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
int PegStrnCmp(const char *s1, const char *s2, int iMax)
{
    int i, j;
    i = PegStrLen(s1);
    j = PegStrLen(s2);
    
    if (iMax > j)
    {
        iMax = j;
    }
    if (iMax > i)
    {
        iMax = i;
    }
    while(iMax--)
    {
        if (*s1++ != *s2++)
        {
            if (s1[-1] < s2[-1])
            {
                return -1;
            }
            return 1;
        }
    }
    return 0;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
int PegStrLen(const char *s1)
{
    int iLen =0;
    while(*s1++)
    {
        iLen++;
    }
    return iLen;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
int PegAtoI(const char *s1)
{
    int j = 0;
    int sign = 0;

    while(*s1 == ' ')
    {
        s1++;
    }
    if (*s1 == '+')
    {
	    s1++;
    }
    else
    {
        if (*s1 == '-')
        {
	        s1++;
	        sign--;
	    }
    }
    while (*s1 >= '0' && *s1 <= '9')
    {
	    j = j * 10 + (*s1 - '0');
	    s1++;
    }
	if(sign)
	return(-j);
    else
	return (j);

}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
long PegAtoL(const char *s1)
{
    long j = 0;
    int sign = 0;

    while(*s1 == ' ')
    {
        s1++;
    }

    if (*s1 == '+')
    {
	    s1++;
    }
    else
    {
        if (*s1 == '-')
        {
	        s1++;
	        sign--;
	    }
    }
    while (*s1 >= '0' && *s1 <= '9')
    {
	    j = j * 10 + (*s1 - '0');
	    s1++;
    }
	if(sign)
	return(-j);
    else
	return (j);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PEGCHAR *PegStrCpy(char *s1, const PEGCHAR *s2)
{
    UnicodeToAscii(s1, s2);
    return (PEGCHAR *) s1;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PEGCHAR *PegStrCpy(PEGCHAR *s1, const char *s2)
{
    AsciiToUnicode(s1, s2);
    return s1;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
char *PegStrCpy(char *s1, const char *s2)
{
    char *sr = s1;
    while(*s2)
    {
        *s1++ = *s2++;
    }
    *s1 = *s2;
    return sr;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
char *PegStrCat(char *s1, const char *s2)
{
    char *sr = s1;
    while(*s1)
    {
        s1++;
    }
    while(*s2)
    {
        *s1++ = *s2++;
    }
    *s1 = *s2;
    return sr;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
char *PegStrCat(char *s1, const PEGCHAR *s2)
{
    char *sr = s1;
    while(*s1)
    {
        s1++;
    }
    while(*s2)
    {
        *s1++ = (char) *s2++;
    }
    *s1 = (char) *s2;
    return sr;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PEGCHAR *PegStrCat(PEGCHAR *s1, const char *s2)
{
    PEGCHAR *sr = s1;

    while(*s1)
    {
        s1++;
    }
    while(*s2)
    {
        *s1++ = (PEGCHAR) *s2++;
    }
    *s1 = (PEGCHAR) *s2;

    return sr;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
char *PegStrnCat(char *s1, const PEGCHAR *s2, int iMax)
{
    char *sr = s1;
    while(*s1)
    {
        s1++;
    }
    while(*s2 && iMax--)
    {
        *s1++ = (char) *s2++;
    }
    *s1 = 0;

    return sr;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void UnicodeToAscii(char *s1, const PEGCHAR *s2)
{
    while(*s2)
    {
        *s1++ = (char) *s2++;
    }
    *s1 = '\0';
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void AsciiToUnicode(PEGCHAR *s1, const char *s2)
{
    while(*s2)
    {
        *s1++ = (PEGCHAR) *s2++;
    }
    *s1 = 0;
}

#endif      // PEG_UNICODE

#endif      // PEG_STRLIB



