


#ifndef _STEDIT_INPUT
#define _STEDIT_INPUT

//#include "Pstring.hpp"
#include "pegtypes.hpp"


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class stEditInput : public PegString
{
    public:
        stEditInput(const PegRect &Rect);

};

class MutilSelectString : public PegString
{
	public:
		MutilSelectString(const PegRect &Rect);
		SIGNED Message(const PegMessage &Mesg);
		int GetModulationStatus(void);
	private:
		int stCurrentModulation;
};

#endif



