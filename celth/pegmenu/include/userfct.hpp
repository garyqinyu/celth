
#ifndef _USERFCT_HPP
#define _USERFCT_HPP

#define LANGUAGE_NUMBER  1

typedef struct {
	int iStringID;
	const PEGCHAR* StringToDisplay[LANGUAGE_NUMBER]; /* string to display for a given language */
} HorizontalScrollText_t;

typedef struct {
	HorizontalScrollText_t* ptstTable;
	int iTableSize;
} HorizontalScrollTable_t;

extern HorizontalScrollTable_t  TNRStatus_Table;

const PEGCHAR* GetString(int iCurrentValue, HorizontalScrollTable_t* ptstScrollTable);

#endif /*_USERFCT_HPP*/