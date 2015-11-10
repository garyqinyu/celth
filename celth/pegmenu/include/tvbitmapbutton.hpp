#ifndef ST_TVBITMAPBUTTON_HPP
#define ST_TVBITMAPBUTTON_HPP

class ToolTip;

class TVBitmapButton : public PegBitmapButton
{
public:
	 TVBitmapButton(PegRect &Rect, PegBitmap *bm, WORD wId = 0, WORD wStyle = AF_ENABLED);
	 virtual ~TVBitmapButton();
	 virtual SIGNED Message(const PegMessage &Mesg);
	 void Draw(void);

	 void ShowTip();
	 void SetTipString(const PEGCHAR * pTipString);
	 void SetToolTip(ToolTip * pToolTip);

private:
	const PEGCHAR * m_pTipString;
	ToolTip * m_pToolTip;
};

#endif