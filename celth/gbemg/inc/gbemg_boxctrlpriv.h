#ifndef _GBEMG_BOXCTRLTPRIV_H_
#define _GBEMG_BOXCTRLPRIV_H_


/* 
copyright reserved by 
celtech company Co Ltd,
*/
 


#ifdef __cplusplus
extern "C" {
#endif

typedef enum GBEMG_BOX_CTRL_CMD_TAG{

GBEMG_BOXCTRL_eUrgentshow = 0x01,
GBEMG_BOXCTRL_eValumeset,
GBEMG_BOXCTRL_ePowerturn,
GBEMG_BOXCTRL_eAmpiterturn,
GBEMG_BOXCTRL_eSettime,
GBEMG_BOXCTRL_eSetreturncircle,
GBEMG_BOXCTRL_eSetendreturnip,
GBEMG_BOXCTRL_eRequestrecordfile,
GBEMG_BOXCTRL_eCmdmax

}GBEMG_BOX_CTRL_CMD;


typedef enum GBEMG_BOXPOWER_TURN_e{

	BOX_POWER_OFF =0x00,
	BOX_POWER_ON,	
}GBEMG_BOXPOWER_TURN;

typedef enum GBEMG_BOXAMP_TURN_e{

	BOX_AMP_OFF =0x00,
	BOX_AMP_ON,	
}GBEMG_BOXAMP_TURN;




#ifdef __cplusplus
}
#endif

#else
#error "Multiple inclusions of gbemg_boxctrlpriv.h"

#endif

