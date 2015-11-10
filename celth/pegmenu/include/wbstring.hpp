

enum STRING_IDS {
    SID_Help_Prompt_Help = 0,
    SID_FactorySetMenu_Prompt_Title,
    SID_Help_TextBox_ButtonComment,
    SID_Help_TextBox_BarComment,
    SID_MainMenu_Prompt_Picture,
    SID_MainMenu_Prompt_Sound,
    SID_MainMenu_Prompt_SuperFunction,
    SID_MainMenu_Prompt_SuperSetting,
    SID_MainMenu_Prompt_Factory,
    SID_PictureMenu_Prompt_Brightness,
    SID_PictureMenu_Prompt_Contrast,
    SID_PictureMenu_Prompt_Tint,
    SID_PictureMenu_Prompt_Sharpness,
    SID_PictureMenu_Prompt_Color,
    SID_PictureMenu_Prompt_ColorTemp,
    SID_SoundMenu_Prompt_Volume,
    SID_SuperSettingMenu_Prompt_DCI,
    SID_SuperSettingMenu_Prompt_Zoom,
    SID_SuperSettingMenu_Prompt_Help,
    SID_SuperFuctionMenu_Prompt_EPG,
    SID_SuperFunctionMenu_Prompt_Parent,
    SID_SuperFuctionMenu_Prompt_CC,
    SID_SuperFuctionMenu_Prompt_CA,
    SID_FactoryMenu_Prompt_FactroySelect,
    SID_FactoryMenu_WhiteBalance,
    SID_FactoryMenu_Prompt_Geometry,
    SID_SelectWindow_TextButton_Text,
    SID_Help_TextButton_SelectComment,
    SID_SuperSetMenu_SelectDCI_On,
    SID_SuperSetMenu_SelectDCI_Off,
    SID_SuperSetMenu_Zoom_43,
    SID_SuperSetMenu_Zoom_169,
    SID_SuperSetMenu_SelectHelp_On,
    SID_SuperSetMenu_SelectHelp_Off,
    SID_SuperSetMenu_Prompt_DCI_Short,
    SID_SuperSetMenu_Prompt_Geo,
    SID_SuperSetMenu_Zoom_Panorama,
    SID_SoundMenu_Prompt_Mute,
    SID_SoundMenu_Prompt_Mute_On,
    SID_SoundMenu_Prompt_Mute_Off,
    SID_SuperSettingMenu_Prompt_Input,
    SID_SuperSettingMenu_Input_Tuner,
    SID_SuperSettingMenu_Input_CVBS,
    SID_SuperSettingMenu_Input_YCrCb,
    SID_VideoMenu_Prompt_GreenBoost,
    SID_VideoMenu_Prompt_EdgeReplace,
    SID_VideoMenu_Prompt_DCI,
    SID_VideoMenu_Prompt_Peaking,
    SID_VideoMenu_Prompt_AutoFlesh,
    SID_VideoMenu_Prompt_Tint,
    SID_VideoMenu_Prompt_Saturation,
    SID_VideoMenu_Prompt_DCI_Short,
    SID_VideoMenu_AutoFlash_USA,
    SID_VideoMenu_AutoFlash_Japan
};


const PEGCHAR  *LookupString(WORD wSID);

#define LS(a) LookupString(a)
