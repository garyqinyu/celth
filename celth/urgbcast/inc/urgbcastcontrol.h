#ifndef URGBCASTCONTROL_H__
#define URGBCASTCONTROL_H__


#ifdef __cplusplus
extern "C" {
#endif



typedef enum _EMERGE_CMD_TYPE
{
	EMERGE_CMD_TURN_OFF =0x00,
	EMERGE_CMD_TURN_ON,
	EMERGE_CMD_SET_PLAY_VOL,
	EMERGE_CMD_PLAY_EMERGE_INFO_FROM_TS,
	EMERGE_CMD_PLAY_EMERGE_INFO_ON_LOCAL,
	EMERGE_CMD_TRAN_UART_DATA,
	EMERGE_CMD_MAX,
	
		
}EMERGE_CMD_TYPE;


typedef struct Emerge_Play_Local_Param_s
{
	
}Emerge_Play_Local_Param_t;

typedef struct Emerge_Play_Stream_Param_s
{
	CELTH_U16 AudioPid;

	CELTH_U32 Freq;
	CELTH_U32 Symbolrate;
}Emerge_Play_Stream_Param_t;


typedef struct Emerge_Set_Valume_Param_s
{
	CELTH_U8 vol;
}Emerge_Set_Valume_Param_t;

typedef struct Emerge_Turn_Param_s
{
	CELTH_U32 dummy;
}Emerge_Turn_Param_t;

typedef struct Emerge_TranData_Param_s
{
	CELTH_U32 dummy;
}Emerge_TranData_Param_t;


typedef struct Emerge_Invalid_Param_s
{
	CELTH_U32 dummy;
}Emerge_Invalid_Param_s;




typedef struct Emerge_Cmd_s
{
	EMERGE_CMD_TYPE cmd_type;
	
	union {
	
	Emerge_Play_Local_Param_t  EmergePlayLocal;
	Emerge_Play_Stream_Param_t EmergePlayStream;
	Emerge_Set_Valume_Param_t  EmergeSetVal;
	Emerge_TranData_Param_t    EmergeTranData; 
	Emerge_Turn_Param_t        EmergeTurn;
	Emerge_Invalid_Param_s     EmergeInvalid;

	}EmergeCmdParam;


}Emerge_Cmd_t;






#ifdef __cplusplus
}
#endif

#endif /* URGBCASTCONTROL_H__ */

