
#include "peg.hpp"
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

ROMDATA UCHAR ucgreydotBitmap[143] = {
0x0f,0xff,0x04,0xff,0x04,0xe0,0x05,0xff,0x03,0xff,0x80,0xe0,0x04,0x08,0x80,0xe0,
0x04,0xff,0x02,0xff,0x83,0xe0,0x08,0x08,0xe0,0x03,0x08,0x80,0xe0,0x03,0xff,0x87,
0xff,0xff,0xe0,0x08,0x08,0x0f,0x0f,0xe0,0x03,0x08,0x80,0xe0,0x02,0xff,0x86,0xff,
0xe0,0x08,0x08,0x0f,0x0f,0xe0,0x05,0x08,0x82,0xe0,0xff,0xff,0x85,0xff,0xe0,0x08,
0xe0,0x0f,0xe0,0x06,0x08,0x82,0xe0,0xff,0xff,0x84,0xff,0xe0,0x08,0x08,0xe0,0x07,
0x08,0x82,0xe0,0xff,0xff,0x81,0xff,0xe0,0x0a,0x08,0x82,0xe0,0xff,0xff,0x81,0xff,
0xe0,0x0a,0x08,0x82,0xe0,0xff,0xff,0x82,0xff,0xff,0xe0,0x08,0x08,0x80,0xe0,0x02,
0xff,0x02,0xff,0x80,0xe0,0x06,0x08,0x80,0xe0,0x03,0xff,0x03,0xff,0x80,0xe0,0x04,
0x08,0x80,0xe0,0x04,0xff,0x04,0xff,0x04,0xe0,0x05,0xff,0x0f,0xff,0x0f,0xff,};
PegBitmap gbGreyDotBitmap = { 0x11, 8, 16, 16, 0x000000ff, (UCHAR *) ucgreydotBitmap};

