/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// anim.hpp - bitmap tables definition for animation in TVDecorated buttons
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

extern PegFont Pronto_10;
extern PegFont Pronto_12;
extern PegFont Pronto_16;
extern PegFont Pronto_26;
extern PegFont A12RFont;
extern PegFont korean;
extern PegBitmap gbBulletBitmap;
extern PegBitmap gbBullet1Bitmap;
extern PegBitmap gbBullet2Bitmap;
extern PegBitmap gbBullet3Bitmap;
extern PegBitmap gbBullet4Bitmap;
extern PegBitmap gbBullet5Bitmap;
extern PegBitmap gbLangBitmap;
extern PegBitmap gbMainBitmap;
extern PegBitmap gbMain1Bitmap;
extern PegBitmap gbPictBitmap;
extern PegBitmap gbPict1Bitmap;
extern PegBitmap gbPictpBitmap;
extern PegBitmap gbWinbarBitmap;
extern PegBitmap gbInoutBitmap;
extern PegBitmap gbRCBitmap;
extern PegBitmap gbTuningBitmap;

#define gbMainAudioBitmap	gbTuningBitmap

extern PegBitmap gbAdvAudioBitmap;
extern PegBitmap gbMainAudio1Bitmap;
extern PegBitmap gbAdvAudio1Bitmap;
extern PegBitmap gbEqualizerBitmap;

#define gbHeadPhoneBitmap		gbAdvAudioBitmap 

extern UCHAR PegCustomPalette[256*3];

extern UCHAR gbCurrentLanguage;
extern PegBitmap gbHalfMuteBitmap;
extern PegBitmap gbFullMuteBitmap;
extern PegBitmap gbVolBarBckgBitmap;
extern PegBitmap gbVolIconBitmap;

extern PegBitmap gbRighiconBitmap;
extern PegBitmap gbLefticonBitmap;
extern PegBitmap gbRC_txtBitmap;
extern PegBitmap gbRC_twbBitmap;
extern PegBitmap gbRC_menBitmap;

#undef EXTERNAL_DEFINITION
#ifdef __MAINMENUMASTER /* this should be defined in MainMenu.cpp only */
	#define EXTERNAL_DEFINITION
#else
	#define EXTERNAL_DEFINITION extern
#endif

EXTERNAL_DEFINITION PegBitmap* tAnimationTable[]
#ifdef __MAINMENUMASTER
={
	&gbBulletBitmap,
	&gbBullet1Bitmap,
	&gbBullet2Bitmap,
	&gbBullet3Bitmap,
	&gbBullet4Bitmap,
	&gbBullet5Bitmap
}
#endif
;


/* end of file */

