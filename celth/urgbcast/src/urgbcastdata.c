
#include "celth_stddef.h"

#include "celth_mutex.h"

#include "celth_rtos.h"


#include "urgbcastdef.h"
#include "urgbcastcontrol.h"

#include "urgbcast.h"




static emerge_pmttable_info globalpmtinfo;


static emerge_channel_info  old_ebchannel_info;

static emerge_channel_info  new_ebchannel_info;


CELTH_STATIC CELTHOSL_SemId_t	pmttable_lock;


CELTH_STATIC CELTHOSL_SemId_t	oldebtable_lock;


CELTH_STATIC CELTHOSL_SemId_t	newebtable_lock;






CELTH_BOOL  EBcastDataInfoInit()
{


	pmttable_lock = CELTH_SemCreate( "urgentPmt", CELTHOSL_FifoNoTimeOut, 1 );

	if(pmttable_lock==NULL)
	{
		return FALSE;
	}

	
	oldebtable_lock = CELTH_SemCreate( "urgentoldebtable", CELTHOSL_FifoNoTimeOut, 1 );

	if(oldebtable_lock==NULL)
	{
		return FALSE;
	}

	
	newebtable_lock = CELTH_SemCreate( "urgentnewebtable", CELTHOSL_FifoNoTimeOut, 1 );

	if(newebtable_lock==NULL)
	{
		return FALSE;
	}


	CELTHOSL_Memset(&globalpmtinfo,0x00,sizeof(emerge_pmttable_info));

	CELTHOSL_Memset(&old_ebchannel_info,0x00,sizeof(emerge_channel_info));

	CELTHOSL_Memset(&new_ebchannel_info,0x00,sizeof(emerge_channel_info));

	
	return TRUE;




}
emerge_pmttable_info* GetPmtTableInfo()
{

	return &globalpmtinfo;
}

CELTH_VOID  SetPmtTableInfo(emerge_pmttable_info* info)
{
	if(info)
	{
	
	CELTH_SemWait( pmttable_lock, CELTHOSL_WaitForEver );

	CELTHOSL_Memcpy(&globalpmtinfo,info,sizeof(emerge_pmttable_info));

	CELTH_SemPost(pmttable_lock);
	}
	
}

emerge_channel_info*  GetOldEgbtTableInfo()
{
	return &old_ebchannel_info;
}

CELTH_VOID  SetOldEgbtTableInfo(emerge_channel_info* info)
{
	
	if(info)
	{
	
	CELTH_SemWait( oldebtable_lock, CELTHOSL_WaitForEver );

	CELTHOSL_Memcpy(&old_ebchannel_info,info,sizeof(emerge_channel_info));

	CELTH_SemPost(oldebtable_lock);
	}
}

emerge_channel_info*  GetNewEgbtTableInfo()
{
	return &new_ebchannel_info;
}

CELTH_VOID  SetNewEgbtTableInfo(emerge_channel_info* info)
{
	
	if(info)
	{
	
	CELTH_SemWait( newebtable_lock, CELTHOSL_WaitForEver );

	CELTHOSL_Memcpy(&new_ebchannel_info,info,sizeof(emerge_channel_info));

	CELTH_SemPost(newebtable_lock);
	}
}

CELTH_BOOL  SetNewEgbtTableInfo2OldEgbtTableInfo()
{

	
	CELTH_SemWait( oldebtable_lock, CELTHOSL_WaitForEver );

	CELTHOSL_Memcpy(&old_ebchannel_info,&new_ebchannel_info,sizeof(emerge_channel_info));

	CELTH_SemPost(oldebtable_lock);
	
}

CELTH_BOOL ComapareEgbtTable2NewEgbtTable(emerge_channel_info* info)
{
	emerge_channel_info* pnewEgbtTable;
	CELTH_BOOL ret=FALSE;
	if(info)
		{
			pnewEgbtTable=GetNewEgbtTableInfo();

			if((pnewEgbtTable->time_stamp.stamp0==info->time_stamp.stamp0)&&
				(pnewEgbtTable->time_stamp.stamp1==info->time_stamp.stamp1)&&
				(pnewEgbtTable->time_stamp.stamp2==info->time_stamp.stamp2)&&
				(pnewEgbtTable->time_stamp.stamp3==info->time_stamp.stamp3)&&
				(pnewEgbtTable->time_stamp.stamp4==info->time_stamp.stamp4))
				{

			ret = TRUE;
				}
		
		}
	return ret;
}


CELTH_BOOL ComapareNewEgbtTable2OldEgbtTable()
{
	
	emerge_channel_info* pnewEgbtTable;
	emerge_channel_info* poldEgbtTable;
	CELTH_BOOL ret=FALSE;

	pnewEgbtTable=GetNewEgbtTableInfo();
	poldEgbtTable=GetOldEgbtTableInfo();
/*
	printf("\n new[%x][%x][%x][%x][%x]\n",pnewEgbtTable->time_stamp.stamp0,pnewEgbtTable->time_stamp.stamp1,pnewEgbtTable->time_stamp.stamp2,pnewEgbtTable->time_stamp.stamp3,pnewEgbtTable->time_stamp.stamp4);
        printf("\n old[%x][%x][%x][%x][%x]\n",poldEgbtTable->time_stamp.stamp0,poldEgbtTable->time_stamp.stamp1,poldEgbtTable->time_stamp.stamp2,poldEgbtTable->time_stamp.stamp3,poldEgbtTable->time_stamp.stamp4);

*/

	
	if((pnewEgbtTable->time_stamp.stamp0==poldEgbtTable->time_stamp.stamp0)&&
		(pnewEgbtTable->time_stamp.stamp1==poldEgbtTable->time_stamp.stamp1)&&
		(pnewEgbtTable->time_stamp.stamp2==poldEgbtTable->time_stamp.stamp2)&&
		(pnewEgbtTable->time_stamp.stamp3==poldEgbtTable->time_stamp.stamp3)&&
		(pnewEgbtTable->time_stamp.stamp4==poldEgbtTable->time_stamp.stamp4))
		{
	
	ret = TRUE;
		}

	return ret;

	
	
}





