#ifndef ST_TVTEXTBUTTON_HPP
#define ST_TVTEXTBUTTON_HPP

#define Toggle_up FALSE
#define Toggle_down TRUE
#define LANGUAGE_NUMBER  1

class ToolTip;

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


class TVTextButton : public PegTextButton
{
public:
	 TVTextButton(PegRect &Rect, PEGCHAR *text, WORD wId = 0, WORD wStyle = AF_ENABLED, int index =1, int current_value =0, HorizontalScrollTable_t *ptstScrollTable = NULL);
	 virtual ~TVTextButton();
	 virtual SIGNED Message(const PegMessage &Mesg);
	 void Draw(void);

/*	 void ShowTip();
	 void SetTipString(const PEGCHAR * pTipString);
	 void SetToolTip(ToolTip * pToolTip);*/
	 int GetCurrentToggleIndex();

private:
	int number_of_toggle_string;
	int current_toggle_index;

/*	const PEGCHAR ** m_pTipString;
	ToolTip * m_pToolTip;*/
	HorizontalScrollTable_t* m_ptstScrollTable;
	void StringToggle(bool directory);
};


#endif