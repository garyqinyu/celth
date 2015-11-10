
#ifndef BCASTDEFINE_H__
#define BCASTDEFINE_H__


#ifdef __cplusplus
extern "C" {
#endif


#ifndef uint8_t
#define uint8_t unsigned char
#endif

#ifndef uint16_t
#define uint16_t unsigned short
#endif

#ifndef uint32_t
#define uint32_t unsigned int
#endif




#define EMERGE_BCAST_DESCRIPTION  0xEB

#define EMERGE_BCAST_PMT_TABLEID  0xA0


#define EMERGE_BCAST_PMT_DES_TURN_ON  0x81
#define EMERGE_BCAST_PMT_DES_TURN_OFF 0x82

#define EMERGE_BCAST_PMT_DES_SET_VOL    0x83
#define EMERGE_BCAST_PMT_DES_PLAY_PRAM  0x84


#define EMERGE_BCAST_PMT_DES_CITY_ADDR       0x01
#define EMERGE_BCAST_PMT_DES_COUNTY_ADDR     0x02
#define EMERGE_BCAST_PMT_DES_TOWN_ADDR       0x03
#define EMERGE_BCAST_PMT_DES_VILLAGE_ADDR    0x04
#define EMERGE_BCAST_PMT_DES_ALL_ADDR        0x05



#define MAX_EMERGE_INFO_PER_PID 1/*8*/

#define MAX_EMERGE_INFO_CHANNEL  32


typedef struct
{
	CELTH_U16	pid;
	CELTH_U8	streamType;
	CELTH_U16  	emb_pid;
} EMPID;


/******************************************************************************
the struct is the main struct of emerge broadcast,

it get from the EM_BCAST_SECTION.

the EM_BCAST_SECTION has four descriptors:

tag is 0x81 ----mean turn on 

tag is 0x82 ----mean turn off

tag is 0x83 ----mean what is the valume value 

tag is 0x84 ----the information of the emerge broadcast, there are deviceid, command type, audio pid

*************************************************************

here we put the different descriptor into one struct 

named as emerge_channel_st.

used it as follow:


if ( turn_on == 0x00 || turn_on== 0x01)
{
	it is a turn state information.
}
else
{
	if(num_audio_pid == 0)
	{
	  it is the valume set information.
	}
	else
	{
		it is the emerge broadcast information .
	}
}


*******************************************************************************/

typedef struct emerge_table_info{


CELTH_U16	map_pid;
CELTH_U16	eg_pid;


}emerge_table_info;


typedef struct emerge_pmttable_info{
	
	CELTH_U8	version;
	CELTH_U16	pmt_pid;
	CELTH_U16       pmt_program_num;
	CELTH_U8    egtable_num;

	emerge_table_info egtable[MAX_EMERGE_INFO_PER_PID];
	
}emerge_pmttable_info;


typedef struct emerge_program_st
{
	

	
	/*************************************************************
	* the following info get from the description which tag is 0x81 and 0x82 , 
	* if get from the tag of 0x81 the turn_on field value is 0x01,
       * if get from the tag of 0x82 the turn_on field value is 0x00,
       * if the value is 0xff, the value is tri_state value, there is no turn on_off switch.
       * default value is 0xff !!!!!!!!!!!!!!!!!!
       **************************************************************/
	CELTH_U8    turn_on;
	
	/**************************************************************/
	/*the following info get from the description which tag is 0x83*/
	/**************************************************************/
	CELTH_U8    vol; /* from 0 to 128  from 0x00 to 0x80, invalid will set it as 0xFF*/
	
	/*********************************************************/
	/*the following info get from the description which  tag is 0x84*/
	/*********************************************************/
	CELTH_U16		video_pid;
	CELTH_U8	num_audio_pids;
	CELTH_U16		audio_pid;
	
	CELTH_U16		other_pid;
	CELTH_U16   symbolrate;
	CELTH_U16   frequency;

}emerge_program_st;


typedef struct device_sn_t{

	CELTH_U8 cityid;
	CELTH_U8 countyid;
	CELTH_U8 townid;
	CELTH_U8 villageid;
	CELTH_U8 deviceid;

	
}device_sn_t;

typedef struct time_stamp_st
{
	CELTH_U8 stamp0;
	CELTH_U8 stamp1;
	CELTH_U8 stamp2;
	CELTH_U8 stamp3;
	CELTH_U8 stamp4;
}time_stamp_st;


typedef struct emerge_channel_info{

device_sn_t  device;

CELTH_U8  command_type;

time_stamp_st time_stamp;


CELTH_U8   program_num; /* the number of the following invalid program */


emerge_program_st emerge_program[MAX_EMERGE_INFO_CHANNEL];


}emerge_channel_info;




#ifdef __cplusplus
}
#endif

#endif /* BCASTDEFINE_H__ */


