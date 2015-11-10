/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// RoboButton.hpp: Custom button class.
//
// Copyright (c) 1997-2002, Swell Software, Inc.
//
// Notes:
//
// Overidden Draw() function, for wide borders and unique style.
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


class RoboButton : public PegButton
{
	public:
		RoboButton(PegRect &, WORD wId, char *Text);
		void Draw(void);
        void DataSet(char *Text)
        {
            mpText = Text;
            Screen()->Invalidate(mClient);
        }

	private:
		char *mpText;
			
};
