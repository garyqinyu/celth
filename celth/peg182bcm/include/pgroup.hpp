/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pgroup.hpp - Definition of the PegGroup object.
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
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifndef _PEGGROUP_
#define _PEGGROUP_

class PegGroup : public PegThing, public PegTextThing
{
    public:
        PegGroup(PegRect &, const PEGCHAR *Text, WORD wStyle = AF_ENABLED);
        virtual ~PegGroup() {}
        virtual void Draw(void);
        virtual void Add(PegThing *Who, BOOL bDraw = TRUE);
        virtual SIGNED Message(const PegMessage &Mesg);
        virtual void Enable(BOOL bEnable);
        virtual PegThing *Remove(PegThing *pWhat, BOOL bDraw = TRUE);

    protected:
};


#endif

