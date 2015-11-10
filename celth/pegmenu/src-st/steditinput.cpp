

#include "peg.hpp"
#include "steditinput.hpp"
#include "usertype.hpp"
#include "modulation.hpp"

stEditInput::stEditInput(const PegRect &Rect):PegString(Rect)
{

}



MutilSelectString::MutilSelectString(const PegRect &Rect):
PegString(Rect)
{
	stCurrentModulation = 0;
	DataSet(PegModulation_s[stCurrentModulation]);
}

SIGNED MutilSelectString::Message(const PegMessage &Mesg)
{
	switch (Mesg.wType)
	{
		case PM_KEY:
		        if (Mesg.iData == PK_LEFT)
		        {
		        	stCurrentModulation--;
				if (stCurrentModulation<0)
					stCurrentModulation = 0;
				DataSet(PegModulation_s[stCurrentModulation]);
				Draw();
		        }
			 else if (Mesg.iData == PK_RIGHT)
			{
					stCurrentModulation++;
					if(stCurrentModulation>5)
						stCurrentModulation = 5;
					DataSet(PegModulation_s[stCurrentModulation]);
					Draw();
			}
			else
			{
					PegString::Message(Mesg);
			}
			break;
		default:
			PegString::Message(Mesg);
			break;
	}

}

int MutilSelectString::GetModulationStatus(void)
{
	return stCurrentModulation;
}











