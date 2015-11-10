/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// usertypes.hpp - user data types
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifndef __USERTYPE__
#define __USERTYPE__

#include "peg.hpp"

enum {
	TYPE_PEGTEXTCOLORBUT = FIRST_USER_CONTROL_TYPE,
	TYPE_PEGRADIOCOLORBUT,
	TYPE_PEGBITMAPCOLORBUT,
	TYPE_PEGCUSTSLIDER,
	TYPE_TVDECORATEDBUT
};

enum {
	TYPE_PEGBITMAPWINDOW = FIRST_USER_WINDOW_TYPE
};

enum {
	PM_REFRESH = FIRST_USER_MESSAGE,
	PM_KEYLEFT,
	PM_KEYRIGHT,
	PM_KEYUP,
	PM_KEYDOWN,
	PM_UPDATEVALUE,
	PM_REFRESHWINDOW,
	KEY_VALUE_INCREASE,
	KEY_VALUE_DECREASE,
	VOLUME_VALUE_INCREASE,
	VOLUME_VALUE_DECREASE,
	PK_MUTE_UNMUTE,
	PM_TUNINGFREQUENCYFOUND,
	PM_CLOSEFORSTANDBY
};

enum {
	kAutoAdjustBitmapSize,
	kCenterBitmap
};

enum {
	DRAW_FOCUS_TIMER = 1,
	ERASE_FOCUS_TIMER,
	POPUP_WINDOW_TIMER,
	G2ALIGN_TIMER,
	TUNING_SEARCH_TIMER,
	TIMER_ZOOM,
	TIMER_CHANNEL,
	TIMER_SOUND_STANDARD,
	kTimerAnimation,
	kTimerVolumeBar
};

#define FIRST_USER_ID 1000
// enums for the Menu windows. They MUST be classified by level, from main menu to sub-sub menu
enum {
	MAIN_MENU_ID=FIRST_USER_ID,		// main menu
	PICTURE_MENU_ID,							// sub menu
	MAIN_AUDIO_MENU_ID,                         // sub menu
	INSTALL_MENU_ID,							// sub menu
	SERVICE_MENU_ID,							// sub menu
	HELP_MENU_ID,									// sub menu
	PICTURE_PLUS_MENU_ID,					// sub-sub menu of picture
	TUNING_MENU_ID,								// sub-sub menu of install
	IO_MENU_ID,										// sub-sub menu of install
	LANGUAGE_MENU_ID,							// sub-sub menu of install
	SETUP_MENU_ID,								// sub-sub menu of service
	GEOMETRY_MENU_ID,							// sub-sub menu of service
	VIDEO_SERVICE_MENU_ID,				// sub-sub menu of service
	SERVICE_AUDIO_MENU_ID,				// sub-sub menu of service
	ADV_AUDIO_MENU_ID,
	PROGRESSWINDOW_ID,
	FIRST_POPUP_ID								// DO NOT REMOVE THIS LINE IT MUST BE THE LAST ONE OF THIS enum
};
// enums for the popups windows. It includes volume information such as volume bar and mute icons.
enum
{
	HALF_MUTE_ID = FIRST_POPUP_ID,
	FULL_MUTE_ID,
	POPUP_VOLBAR_ID,
	GENAGC_MESSAGE_ID,
	LOWAGC_MESSAGE_ID,
	POPUP_ZOOM_ID,
	POPUP_GENERALINFO_ID,
	POPUP_NOSIGNAL_ID,
	POPUP_AUDIOLANGUAGE_ID,
	POPUP_EQSOUNDPRESET_ID,
	POPUP_SOUNDSTANDARD_ID,
	POPUP_AVNAME_ID,
	POPUP_NOTELETEXT_ID,
	POPUP_RESTOREDEFAULT_ID,
	POPUP_VALUESTORED_ID,
	POPUP_PROGRAMSTORED_ID,
	POPUP_NOTELEWEB_ID,
	LAST_USER_ID // this line MUST be the last one of the enum
}; /* to be used to define IDs in menus */

#define MaxNumberOfWindow LAST_USER_ID - FIRST_USER_ID

#define PegBlueTransparent 254 /* color 254 should be initialized as blue transparent */

#define PopupTimout 5 /* 5 second timout for popup windows */

#endif

