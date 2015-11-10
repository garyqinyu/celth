/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// grapdemo.hpp - Example using extended graphics functions.
//
// Author: Kenneth G. Maxwell
//
// Copyright (c) 1993-2002 Swell Software 
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


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class GraphicsWin : public PegDecoratedWindow
{
	public:

		GraphicsWin(const PegRect &Rect);
        void Draw(void);

	private:
};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class ArcWin : public PegDecoratedWindow
{
	public:

		ArcWin(const PegRect &Rect);
        void Draw(void);

	private:
};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class CircleWin : public PegDecoratedWindow
{
	public:

		CircleWin(const PegRect &Rect);
        void Draw(void);

	private:
};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class EllipseWin : public PegDecoratedWindow
{
	public:

		EllipseWin(const PegRect &Rect);
        void Draw(void);

	private:
};

