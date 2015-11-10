
#include <stdio.h>

#include "celth_stddef.h"


#include "urgbcastdef.h"


#ifndef bool 
#define bool CELTH_U8
#endif

#include "ts_priv.h"





#define EGBT_SECTION_LENGTH_OFFSET (1)
#define EGBT_INFO_OFFSET (3)


#define EGBT_SECTION_LENGTH(buf) (TS_READ_16(&buf[EGBT_SECTION_LENGTH_OFFSET])&0xFFF)


#define EGBT_DEVICE_SN_LENGTH (4)
#define EGBT_COMMAND_TYPE_LENGTH (1)     
#define EGBT_TIMESTAMP_LENGTH  (5)
#define EGBT_PRIVATE_PART_DATA_LENGTH_LENGTH (1)

#define EGBT_DEVICE_SN_OFFSET  (EGBT_INFO_OFFSET )
#define BGBT_COMMAND_TYPE_OFFSET (EGBT_INFO_OFFSET + EGBT_DEVICE_SN_LENGTH )
#define EGBT_TIMESTAMP_OFFSET  (EGBT_INFO_OFFSET + EGBT_DEVICE_SN_LENGTH + EGBT_COMMAND_TYPE_LENGTH )

#define EGBT_PRIVATE_PART_DATA_OFFSET (EGBT_INFO_OFFSET + EGBT_DEVICE_SN_LENGTH + EGBT_COMMAND_TYPE_LENGTH + EGBT_TIMESTAMP_LENGTH)

#define EGBT_PRIVATE_PART_DATA_LENGTH_OFFSET (EGBT_PRIVATE_PART_DATA_OFFSET + 1)

#define EGBT_PRIVATE_PART_DATA_DESCRIPTOR_OFFSET (EGBT_PRIVATE_PART_DATA_LENGTH_OFFSET + EGBT_PRIVATE_PART_DATA_LENGTH_LENGTH )

#define EGBT_PRIVATE_PART_DATA_ALLDESCRIPTOR_LENGTH(buf)(buf[EGBT_PRIVATE_PART_DATA_LENGTH_OFFSET]&0xFF)


#define EGBT_PRIVATE_PART_DATA_DESCRIPTOR_BASE(buf) (&buf[EGBT_PRIVATE_PART_DATA_LENGTH_OFFSET+EGBT_PRIVATE_PART_DATA_LENGTH_LENGTH])





#define EGBT_PRIVATE_PART_DATA_DESCRIPTOR_TAG_TO_BASE    0
#define EGBT_PRIVATE_PART_DATA_DESCRIPTOR_LENGTH_TO_BASE 1







#define EGBT_PRIVATE_PART_DATA_DESCRIPTOR_VOLUME_TO_BASE 2

#define EGBT_PRIVATE_PART_DATA_DESCRIPTOR_TRANID_TO_BASE 2
#define EGBT_PRIVATE_PART_DATA_DESCRIPTOR_ONETID_TO_BASE 4
#define EGBT_PRIVATE_PART_DATA_DESCRIPTOR_SERVID_TO_BASE 6

#define EGBT_PRIVATE_PART_DATA_DESCRIPTOR_LINKID_TO_BASE 8

#define EGBT_PRIVATE_PART_DATA_DESCRIPTOR_VIDEOPID_TO_BASE 9

#define EGBT_PRIVATE_PART_DATA_DESCRIPTOR_AUDIOPID_TO_BASE 11

#define EGBT_PRIVATE_PART_DATA_DESCRIPTOR_OTHERPID_TO_BASE 13

#define EGBT_PRIVATE_PART_DATA_DESCRIPTOR_NETID_TO_BASE 15

#define EGBT_PRIVATE_PART_DATA_DESCRIPTOR_SYMBOLRATE_TO_BASE 17

#define EGBT_PRIVATE_PART_DATA_DESCRIPTOR_FREQ_TO_BASE  19
 

#define EGBT_PRIVATE_PART_DATA_DESCRIPTOR_TAG(buf) (buf[EGBT_PRIVATE_PART_DATA_DESCRIPTOR_TAG_TO_BASE]&0xFF)


#define EGBT_COMMANDTYPE(buf) (CELTH_U8)((buf[BGBT_COMMAND_TYPE_OFFSET]>>4)&0x0F)  

#define EGBT_DEVICE_SN(buf)   (CELTH_U32)(TS_READ_32(&buf[EGBT_DEVICE_SN_OFFSET]))

#define EGBT_TIMESTAMP(buf)   (CELTH_U32)(TS_READ_32(&buf[EGBT_TIMESTAMP_OFFSET]))

#define EGBT_TIMESTAMP0(buf)   ((buf[EGBT_TIMESTAMP_OFFSET+0])&0xFF)
#define EGBT_TIMESTAMP1(buf)   ((buf[EGBT_TIMESTAMP_OFFSET+1])&0xFF)
#define EGBT_TIMESTAMP2(buf)   (CELTH_U8)((buf[EGBT_TIMESTAMP_OFFSET+2])&0xFF)
#define EGBT_TIMESTAMP3(buf)   (CELTH_U8)((buf[EGBT_TIMESTAMP_OFFSET+3])&0xFF)
#define EGBT_TIMESTAMP4(buf)   (CELTH_U8)((buf[EGBT_TIMESTAMP_OFFSET+4])&0xFF)






#define DEVICE_CITY_ID(x)     (CELTH_U8)(((CELTH_U32)x>>24)&0xFF)

#define DEVICE_COUNTY_ID (x)  (CELTH_U8)(((CELTH_U32)x>>18)&0xFF) 

#define DEVICE_TOWN_ID (x)    (CELTH_U8)(((CELTH_U32)x>>12)&0xFF) 

#define DEVICE_VILLAGE_ID (x) (CELTH_U8)(((CELTH_U32)x>>6)&0xFF)

#define DEVICE_DEVICE_ID (x)  (CELTH_U8)(((CELTH_U32)x)&0xFF)


#define EGBT_VIDEO_GET_PID(buf) (CELTH_U16)(TS_READ_16( &buf[EGBT_PRIVATE_PART_DATA_DESCRIPTOR_VIDEOPID_TO_BASE] ) & 0x1FFF)



#define EGBT_AUDIO_GET_PID(buf) (CELTH_U16)(TS_READ_16( &buf[EGBT_PRIVATE_PART_DATA_DESCRIPTOR_AUDIOPID_TO_BASE] ) & 0x1FFF)


#define EGBT_OTHER_GET_PID(buf) (CELTH_U16)(TS_READ_16( &buf[EGBT_PRIVATE_PART_DATA_DESCRIPTOR_OTHERPID_TO_BASE] ) & 0x1FFF)


#define EGBT_GET_SYMBOL(buf) (CELTH_U16)(TS_READ_16( &buf[EGBT_PRIVATE_PART_DATA_DESCRIPTOR_SYMBOLRATE_TO_BASE] ) & 0xFFFF)

#define EGBT_GET_FREQ(buf) (CELTH_U16)(TS_READ_16( &buf[EGBT_PRIVATE_PART_DATA_DESCRIPTOR_FREQ_TO_BASE] ) & 0xFFFF)






CELTH_BOOL Ts_Validate_EGBT(CELTH_CONST CELTH_U8 *egbt,CELTH_U32 egbtSize)
{
	
	int sectionlen = EGBT_SECTION_LENGTH(egbt)+3;
	
		
	return ((egbt[0]==0xA0)&&(sectionlen== egbtSize));
}


TS_PSI_descriptor Ts_EGBT_getPrivateDescriptor( CELTH_CONST CELTH_U8 *buf, CELTH_U32 egbtSize, CELTH_U8 descriptorNum )
{

const uint8_t *descriptorBase = EGBT_PRIVATE_PART_DATA_DESCRIPTOR_BASE(buf);
CELTH_U8 descriptorsLength = EGBT_PRIVATE_PART_DATA_ALLDESCRIPTOR_LENGTH(buf);

/* Any time we dereference memory based on the contents of live data,
we should check. */
if (descriptorBase - buf >= (int)egbtSize) {
	return NULL;
}
egbtSize -= (descriptorBase - buf);
if (egbtSize < descriptorsLength) {
	printf("Invalid descriptor length: %d>=%d", descriptorsLength, egbtSize);
	return NULL;
}
return TS_P_getDescriptor(descriptorBase, descriptorsLength, descriptorNum);



}

CELTH_BOOL Ts_parseEGBT(CELTH_CONST CELTH_U8 *egbt, CELTH_U32 egbtSize, emerge_channel_info *p_emergechanInfo)
{


	CELTH_U32 devicesn;

	int i;

	TS_PSI_descriptor  egbdescriptor;
	CELTH_BOOL bret=FALSE;
/*
	printf("\n################## ENTER Parse EGBT###############\n");
	printf("\n###########PRINT the EGBT Section#############\n");

	for(i=0;i<egbtSize;i++)
	{
		printf("%x\t",egbt[i]);
	}
	printf("\n############FINISH PRINT the EGBT Section#########\n");
*/
	
	if(p_emergechanInfo==NULL||egbt==NULL)
			{
				return bret;
			}
	if(!Ts_Validate_EGBT(egbt,egbtSize))
		{
			printf("\n##############Invalid EGBT packege###########\n");
			return bret;
		}

	/* first set the program_num as zero*/
	p_emergechanInfo->program_num = 0x00;

	p_emergechanInfo->command_type = EGBT_COMMANDTYPE(egbt);
	

	p_emergechanInfo->time_stamp.stamp0 = EGBT_TIMESTAMP0(egbt);

	
	p_emergechanInfo->time_stamp.stamp1 = EGBT_TIMESTAMP1(egbt);

	
	p_emergechanInfo->time_stamp.stamp2 = EGBT_TIMESTAMP2(egbt);
	
	p_emergechanInfo->time_stamp.stamp3 = EGBT_TIMESTAMP3(egbt);
	
	p_emergechanInfo->time_stamp.stamp4 = EGBT_TIMESTAMP4(egbt);

	devicesn= EGBT_DEVICE_SN(egbt);


/*	p_emergechanInfo->device.cityid=DEVICE_CITY_ID(x);*/

	p_emergechanInfo->device.cityid = (CELTH_U8)((devicesn>>24)&0x3F);

/*	p_emergechanInfo->device.countyid=DEVICE_COUNTY_ID(devicesn);*/
	
	p_emergechanInfo->device.countyid = (CELTH_U8)((devicesn>>18)&0x3F);


	
/*	p_emergechanInfo->device.townid=DEVICE_TOWN_ID(devicesn);*/
	p_emergechanInfo->device.townid = (CELTH_U8)((devicesn>>12)&0x3F);


	
/*	p_emergechanInfo->device.villageid=DEVICE_VILLAGE_ID(devicesn);*/
	p_emergechanInfo->device.villageid = (CELTH_U8)((devicesn>>6)&0x3F);


	
	/*p_emergechanInfo->device.deviceid=DEVICE_DEVICE_ID(devicesn);*/
	p_emergechanInfo->device.deviceid = (CELTH_U8)((devicesn)&0x3F);



	for(i=0;i<MAX_EMERGE_INFO_CHANNEL;i++)
	{
		
		egbdescriptor = Ts_EGBT_getPrivateDescriptor(egbt,egbtSize,i);

		if(egbdescriptor!=NULL)
			{
				p_emergechanInfo->program_num++;

			switch(EGBT_PRIVATE_PART_DATA_DESCRIPTOR_TAG(egbdescriptor))
				{
				case EMERGE_BCAST_PMT_DES_TURN_ON:
					{
						p_emergechanInfo->emerge_program[i].turn_on=1;
						p_emergechanInfo->emerge_program[i].num_audio_pids = 0;
							break;
					}
				case EMERGE_BCAST_PMT_DES_TURN_OFF:
					{
						p_emergechanInfo->emerge_program[i].turn_on=0;
						p_emergechanInfo->emerge_program[i].num_audio_pids = 0;
							break;
					}
				case EMERGE_BCAST_PMT_DES_SET_VOL:
					{
						p_emergechanInfo->emerge_program[i].turn_on=0xFF;
						p_emergechanInfo->emerge_program[i].vol = egbdescriptor[2];
						p_emergechanInfo->emerge_program[i].num_audio_pids = 0;
							break;

					}
				case EMERGE_BCAST_PMT_DES_PLAY_PRAM:
					{
						p_emergechanInfo->emerge_program[i].turn_on=0xFF;
						p_emergechanInfo->emerge_program[i].vol = 0xFF;
						p_emergechanInfo->emerge_program[i].num_audio_pids = 1;
						p_emergechanInfo->emerge_program[i].video_pid = EGBT_VIDEO_GET_PID(egbdescriptor);
						p_emergechanInfo->emerge_program[i].audio_pid = EGBT_AUDIO_GET_PID(egbdescriptor);
						p_emergechanInfo->emerge_program[i].other_pid = EGBT_OTHER_GET_PID(egbdescriptor);
						p_emergechanInfo->emerge_program[i].symbolrate = EGBT_GET_SYMBOL(egbdescriptor);
						p_emergechanInfo->emerge_program[i].frequency = EGBT_GET_FREQ(egbdescriptor);
							break;
					}
				default:
					{
						
						p_emergechanInfo->program_num--; /* invalid program, the number should decrease*/
						CELTH_U8 tag=EGBT_PRIVATE_PART_DATA_DESCRIPTOR_TAG(egbdescriptor);
						printf("now there are error tag=%x\n",tag);
						break;

					}
				}
			}
		else
			{
				break;
			}
		}
/*	printf("\n######################## EGBT Parse FINISH ###############\n");


	printf("\n############## EGBT parse result ##############\n");

	printf("\n### command_type[%x],device.cityid[%x] device.countyid[%x],device.townid[%x],device.villageid[%x],device.deviceid[%x]#####\n",p_emergechanInfo->command_type,p_emergechanInfo->device.cityid,p_emergechanInfo->device.countyid,p_emergechanInfo->device.townid,

			p_emergechanInfo->device.villageid,p_emergechanInfo->device.deviceid);

	printf("\n##### program_num[%x],stamp0[%x],stamp1[%x],stamp2[%x],stamp3[%x],stamp4[%x]#####\n",p_emergechanInfo->program_num,p_emergechanInfo->time_stamp.stamp0,p_emergechanInfo->time_stamp.stamp1,p_emergechanInfo->time_stamp.stamp2,p_emergechanInfo->time_stamp.stamp3,p_emergechanInfo->time_stamp.stamp4);

	for(i=0;i<p_emergechanInfo->program_num;i++)
{
	printf("\n##### audio_pid[%x],turn_on[%x],vol[%x]###########\n",p_emergechanInfo->emerge_program[i].audio_pid,p_emergechanInfo->emerge_program[i].turn_on,p_emergechanInfo->emerge_program[i].vol);

	printf("\n##### freq[%x],symbolrate[%x],video_pid[%x],audio_pid_num[%x]\n",p_emergechanInfo->emerge_program[i].frequency,p_emergechanInfo->emerge_program[i].symbolrate,p_emergechanInfo->emerge_program[i].video_pid,p_emergechanInfo->emerge_program[i].num_audio_pids);
		
}		
*/	
	if(p_emergechanInfo->program_num!=0)
	{
		bret=TRUE;
	}
	return bret;
	
	

}

