
#include "celth_stddef.h"

#include "celth_rtos.h"
#include "celth_tuner.h"

#include "celth_demux.h"


#include "urgbcastdef.h"

#include "urgbcastcontrol.h"
#include "urgbcast.h"
#include "urgbcastpsisearch.h"

#include "ts_priv.h"
#include "ts_egbt.h"
#include "ts_pat.h"

/*#define kMessageQueueSizeUserLimitation 4*/ 




static int ebPsi_patTimeout = 400;


static int ebPsi_pmtTimeout = 600;


#define PSI_PAT_SIZE    TS_PSI_MAX_PSI_TABLE_SIZE

static CELTH_VOID bcastPsiSearch_p_dumpBuffer(const uint8_t *p_bfr, unsigned bfrSize);

static CELTH_VOID bcastPsiSearch_procStreamDescriptors( const uint8_t *p_bfr, unsigned bfrSize, int streamNum, EMPID *ePidData );
static CELTH_BOOL bcastPsiSearch_procProgDescriptors( const uint8_t *p_bfr, unsigned bfrSize, emerge_table_info *tableInfo,int* pdescriptornum );
static CELTH_U8  bcastPsiSearch_getProgramMaps( const void *p_patBfr, unsigned pat_bfrSize,emerge_program_st *p_chanInfo );

static CELTH_VOID bcastPsiSearch_p_addEmergeChanInfo(CELTH_VOID *context, CELTH_U16 pmt_pid,CELTH_U16 pmt_program_num, const CELTH_VOID *pmt, CELTH_U32 size);

CELTH_BOOL bcastPsiSearch_procProgDescriptors( const uint8_t *p_bfr, unsigned bfrSize, emerge_table_info *tableInfo,int* pdescriptornum )
{
    int i;
    TS_PSI_descriptor descriptor;

	CELTH_BOOL bret=FALSE;

/*	CELTH_U8 version=(CELTH_U8)((p_bfr[5]>>1)&0x1F);*/


/*	printf("\nGet the PMT data is :\n");
	for(i=0;i<bfrSize;i++)
	{
		printf("%x\t",p_bfr[i]);
	}
	printf("\n OK PMT DATA PRINT FINISH++++++++++++++++++++++\n");
*/
    for( i = (*pdescriptornum), descriptor = TS_PMT_getDescriptor( p_bfr, bfrSize, i );
        descriptor != NULL;
        i++, descriptor = TS_PMT_getDescriptor( p_bfr, bfrSize, i ) )
    {
        switch (descriptor[0])
        {
        case EMERGE_BCAST_DESCRIPTION:
		{	printf("\nget the eb PMT descriptor at %x\n",i);
            tableInfo->eg_pid = ((descriptor[3] & 0x1F) << 8) + descriptor[4];  
			/*tableInfo->version = version;*/ 
			bret=TRUE;
			*pdescriptornum=i++;
            break;
		}	

        default:
            break;
        }
    }
		

	return bret;	
}


CELTH_VOID bcastPsiSearch_procStreamDescriptors( const uint8_t *p_bfr, unsigned bfrSize, int streamNum, EMPID *ePidData )
{
    int i;
    TS_PSI_descriptor descriptor;

    for( i = 0, descriptor = TS_PMT_getStreamDescriptor( p_bfr, bfrSize, streamNum, i );
        descriptor != NULL;
        i++, descriptor = TS_PMT_getStreamDescriptor( p_bfr, bfrSize, streamNum, i ) )
    {
        switch (descriptor[0])
        {
        case EMERGE_BCAST_DESCRIPTION:
            ePidData->emb_pid = ((descriptor[4] & 0x1F) << 8) + descriptor[5];
            break;

        default:
            break;
        }
    }
}

#define TS_PSI_VER_OFFSET 5
CELTH_BOOL bcastPsiSearch_p_addEmergeTableInfo(CELTH_VOID *context, CELTH_U16 pmt_pid,CELTH_U16 pmt_program_num, const CELTH_U8 *pmt, CELTH_U32 size)
{
    emerge_pmttable_info* p_chanInfo = (emerge_pmttable_info*)context;
    emerge_table_info emtable;
    int i;
	int descripornum = 0;
	CELTH_BOOL bret=FALSE;
    CELTHOSL_Memset(&emtable, 0, sizeof(emtable));
	p_chanInfo->version = (CELTH_U8)((pmt[TS_PSI_VER_OFFSET]>>1)&0x1F);
	p_chanInfo->pmt_pid = pmt_pid;
	p_chanInfo->pmt_program_num=pmt_program_num;
    /* The "if" comparision below is present to silence the Coverity from choosing the false path in the above "BDBG_ASSERT" line of code. */
	while(p_chanInfo->egtable_num<MAX_EMERGE_INFO_PER_PID)
	{
		if(bcastPsiSearch_procProgDescriptors(pmt, size, &emtable,&descripornum))
			{
        	emtable.map_pid = pmt_pid;

			CELTHOSL_Memcpy(&(p_chanInfo->egtable[p_chanInfo->egtable_num]),&emtable,sizeof(emtable));
			p_chanInfo->egtable_num++;

			
			bret=TRUE;
			}
		else
			{
				break;
			}
   	}
	return bret;
}

CELTH_INT bcastPsiSearch_getPAT(CELTH_VOID *buffer, CELTH_INT  bufferSize)
{

	CELTHDemux_SectionFilterHandle_t  hPatFilter;

	Celth_Demux_SectionFilter_s  patFilterSettings;

	int timeout;
	
	int i;


	
	unsigned char sectionbuf[256];

	

	CELTH_ErrorCode_t   errcode;

	unsigned patSize = 0;

	errcode = CelthApi_Demux_AllocateSectionFilter(NULL,&hPatFilter);

	if(errcode != CELTH_NO_ERROR)
		{
			return 0;
		}
	errcode = CelthApi_Demux_SetSectionFilterPid(hPatFilter,0x0);/* set pat pid to filter*/

	if(errcode != CELTH_NO_ERROR)
		{
			return 0;
		}

	CelthApi_Demux_SectionSetDefaultFilterSettings(&patFilterSettings);
	
	errcode = CelthApi_Demux_SetSectionFilter(hPatFilter,&patFilterSettings);

	
	if(errcode != CELTH_NO_ERROR)
			{
				return 0;
			}

	errcode = CelthApi_Demux_EnableSectionFilter(hPatFilter);

	
	if(errcode != CELTH_NO_ERROR)
			{
				CelthApi_Demux_FreeSectionFilter(hPatFilter);
				return 0;
			}

	for( timeout=ebPsi_patTimeout;timeout > 0 && patSize == 0; timeout-- )
	{

		CelthApi_Demux_GetSection(hPatFilter,&sectionbuf,&patSize,256);

		
        if (patSize) {
            if (patSize > bufferSize)
                patSize = bufferSize;

            CELTHOSL_Memcpy(buffer, sectionbuf, patSize);
		break;	
			
        }
        else {
            CELTHOSL_Sleep(10);
        }
	}

	CelthApi_Demux_DisableSectionFilter(hPatFilter);
	CelthApi_Demux_FreeSectionFilter(hPatFilter);
	return patSize;
}


static CELTH_U8  bcastPsiSearch_getProgramEGBTMaps( const void *p_patBfr, unsigned pat_bfrSize,emerge_pmttable_info *p_chanInfo )
{

return bcastPsiSearch_getPMTs(p_patBfr, pat_bfrSize, bcastPsiSearch_p_addEmergeTableInfo, p_chanInfo);

}


/************************************************************************************/

/*get the emerge table pid */
/************************************************************************************/

CELTH_U8 bcastPsiSearch_getEGBTChannelInfo( emerge_pmttable_info *p_emergetblechanInfo)
{

    CELTH_U8     pat[PSI_PAT_SIZE];
    size_t      patSize;
    TS_PSI_header header;

   /* Blocking call to get PAT */
    patSize = bcastPsiSearch_getPAT( pat, PSI_PAT_SIZE);

    /* If there's no PAT, return but don't print an error because this can happen
    normally. */
    if (patSize <= 0)
{	
        return -1;
}
 /*   printf(("Parsing PAT header"));*/
    TS_PSI_getSectionHeader( pat, &header );
   /* printf(("Finished Parsing PAT header"));*/
    p_emergetblechanInfo->egtable_num     = 0;

    return bcastPsiSearch_getProgramEGBTMaps( pat, patSize, p_emergetblechanInfo);

}
/************************************************************************************/
/* just get the emerge broadcast PMTs */
/*************************************************************************************/

CELTH_CHAR	bcastPsiSearch_getPMTs(
	const CELTH_VOID *pat, CELTH_INT  patSize,
	bcastPsiSearch_PMT_callback callback,
	CELTH_VOID *context)
{

	int             i;
	CELTH_U8     	retValue;
    size_t          bfrSize;
    int             num_programs;
    int             num_programs_to_get;
    int             num_messages_received;
    bool            message_received;
    int             timeout;
    
    CELTH_U16        *pmt_pidArray;
    TS_PAT_program  program;
    int             curProgramNum;

	
	CELTHDemux_SectionFilterHandle_t*  hPmtFilterArray;

	Celth_Demux_SectionFilter_s  pmtFilterSettings;

	
	CELTH_U32*  ppmtlengthArray;

	curProgramNum = 0;


	
    num_programs = TS_PAT_getNumPrograms(pat);

	
    if( num_programs > MAX_EMERGE_INFO_CHANNEL )
    {
        printf("Maximum number of programs exceeded in urgentbroadcast: %d > %d",
            num_programs, MAX_EMERGE_INFO_CHANNEL);
        num_programs = MAX_EMERGE_INFO_CHANNEL;
    }

	
    hPmtFilterArray = (CELTHDemux_SectionFilterHandle_t *)CELTHOSL_MemoryAlloc( sizeof(CELTHDemux_SectionFilterHandle_t) * num_programs );
    CELTHOSL_Memset(hPmtFilterArray, 0, sizeof(CELTHDemux_SectionFilterHandle_t) * num_programs);
	
    pmt_pidArray = (CELTH_U16 *)CELTHOSL_MemoryAlloc( sizeof(CELTH_U16) * num_programs );
    CELTHOSL_Memset(pmt_pidArray, 0, sizeof(CELTH_U16) * num_programs);

	ppmtlengthArray =  (CELTH_U32 *)CELTHOSL_MemoryAlloc( sizeof(CELTH_U32) * num_programs );
	CELTHOSL_Memset(ppmtlengthArray, 0, sizeof(CELTH_U32) * num_programs);

	
    retValue = TS_PAT_getProgram( pat, patSize, curProgramNum, &program );
	
    curProgramNum++;


		if( retValue == 0 )
		{
			while( num_programs )
			{
				/* Always try to read the max number of pids at the same time */
				num_programs_to_get = num_programs;
				num_messages_received = 0;
	
				for( i = 0; i < num_programs_to_get; i++ )
				{
					if( program.program_number == 0 )
					{
						/* This is the network PID, so ignore it */
						num_messages_received++;
					}
					else
					{
						   
						
						CelthApi_Demux_AllocateSectionFilter(NULL,&hPmtFilterArray[i]);
		
							
						if(hPmtFilterArray[i]==NULL)
						{
							
							num_programs_to_get = i-1;
							
							if( num_programs_to_get == 0 )
							{
								/* Abort due to not being able to enable messages */
	/*							BRCM_DBG_MSG(("Unable to enable any messages!")); */
								num_programs = 0;
							}
							break;
						}
						CelthApi_Demux_SetSectionFilterPid(hPmtFilterArray[i],program.PID);		


						CelthApi_Demux_SectionSetDefaultFilterSettings(&pmtFilterSettings);
							
						pmtFilterSettings.mask[0]=0x00;

		
						pmtFilterSettings.coefficient[0]=0x02;/*pmt table id*/

						pmtFilterSettings.coefficient[3] = (program.program_number & 0xFF00) >> 8;
						pmtFilterSettings.coefficient[4] = program.program_number & 0xFF;

						
						pmt_pidArray[i]=program.PID;

						CelthApi_Demux_SetSectionFilter(hPmtFilterArray[i],&pmtFilterSettings);
						if(CelthApi_Demux_EnableSectionFilter(hPmtFilterArray[i])!=0)
						{
							
							pmt_pidArray[i]=0;
							CelthApi_Demux_DisableSectionFilter(hPmtFilterArray[i]);
							CelthApi_Demux_FreeSectionFilter(hPmtFilterArray[i]);

							
							
						}
		
					}				
	
					/* We are finished with this program association so go to the next */
	
					/* TODO: Check for error */
					TS_PAT_getProgram( pat, patSize, curProgramNum, &program );
					curProgramNum++;
				}
			/*	printf("parpare get data of num_received=%d, num_to get=%d",num_messages_received, num_programs_to_get );	*/
	
				/* Now we have enabled our pid channels, so wait for each one to get some data */
				timeout = ebPsi_pmtTimeout;
				while( num_messages_received != num_programs_to_get && timeout != 0 )
				{
					message_received = 0;
					/* Check each of the pid channels we are waiting for */
					for( i = 0; i < num_programs_to_get; i++ )
					{
						CELTH_U8 buffer[256];
						
					/*	  BDBG_MSG(("start get data of %d",i));*/

					CelthApi_Demux_GetSection(hPmtFilterArray[i],&buffer,&ppmtlengthArray[i],256);
					

						if(ppmtlengthArray[i]!=0)
						{
							/* don't call bmessage_read_complete because we're stopping anyway */
	
							message_received = 1;
							num_messages_received++;
	
				/*			printf("\nPMT: %d %d (%02x %02x %02x %02x %02x)\n", i, ppmtlengthArray[i],
								(unsigned char *)buffer[0],((unsigned char *)buffer[1]),((unsigned char *)buffer[2]),((unsigned char*)buffer[3]),((unsigned char*)buffer[4]));*/
	
							if (!TS_PMT_validate(buffer, ppmtlengthArray[i])) {
								bcastPsiSearch_p_dumpBuffer(buffer, ppmtlengthArray[i]);
							}
							else {
								/* Give the PMT to the callback */
								(*callback)(context, pmt_pidArray[i],program.program_number, buffer, ppmtlengthArray[i]);
							}
	
							/* cannot stop until after the data has been parsed because
							we're not copying the data into a local buffer */
							CelthApi_Demux_DisableSectionFilter(hPmtFilterArray[i]);
						}
					}
					if( !message_received )
					{
						CELTHOSL_Sleep(10);
						timeout--;
					}
				}
	
				/* Now disable our pid channels */
				for( i = 0; i < num_programs_to_get; i++ )
				{
					if(hPmtFilterArray[i])
					{

						CelthApi_Demux_DisableSectionFilter(hPmtFilterArray[i]);
						CelthApi_Demux_FreeSectionFilter(hPmtFilterArray[i]);
					}
				}
				num_programs -= num_programs_to_get;
			}
		}


/*	CELTHOSL_MemoryFree(hPmtFilterArray);*/
	
	CELTHOSL_MemoryFree(pmt_pidArray);

	CELTHOSL_MemoryFree(ppmtlengthArray);
	

	return 0;
}



CELTH_BOOL bcastSearch_CheckPmtData(CELTH_U8* pmt, CELTH_U16 pmt_pid,CELTH_U16 pmt_program_num,CELTH_INT pmtSize,emerge_pmttable_info* pmttable)
{

	CELTH_BOOL bret=FALSE;
	if(pmt==NULL)
		{
			return bret;
		}
	 if(pmttable)
	 	{
	 		return bret;
	 	}

	
	if (!TS_PMT_validate(pmt, pmtSize)) {
		printf("\nInvalid PMT data detected: pmtlength 0x%x\n",pmtSize);
		bcastPsiSearch_p_dumpBuffer(pmt, pmtSize);
		return bret;
	}
	else {

		bret= bcastPsiSearch_p_addEmergeTableInfo((CELTH_VOID*)pmttable,pmt_pid,pmt_program_num,pmt,pmtSize);
	}
	return bret;

}




/*
CELTH_Error_Code  bcastPsiSearch_getEGBTs(CELTH_VOID* context,emerge_pmttable_info* pegbtInfo)
{
	
	CELTHDemux_SectionFilterHandle_t*  hEgbtFilterArray;

	Celth_Demux_SectionFilter_s  egbtFilterSettings;

	int i;


	if(pegbtInfo==NULL)
	{
		return CELTHERR_ERROR_BAD_PARAMETER;
	}
    hEgbtFilterArray = (CELTHDemux_SectionFilterHandle_t *)CELTHOSL_MemoryAlloc( sizeof(CELTHDemux_SectionFilterHandle_t) * pegbtInfo->egtable_num);

	if(hEgbtFilterArray==NULL)
	{
		return CELTHERR_ERROR_NO_MEMORY;
	}
	
    CELTHOSL_Memset(CELTHDemux_SectionFilterHandle_t, 0, sizeof(CELTHDemux_SectionFilterHandle_t) * pegbtInfo->egtable_num);

	
	for(i=0;i<pegbtInfo->egtable_num;i++)
	{
	
		CelthApi_Demux_AllocateSectionFilter(NULL,&hEgbtFilterArray[i]);


	
		CelthApi_Demux_SetSectionFilterPid(hEgbtFilterArray[i],pegbtInfo->egtable[i].eg_pid);		


		CelthApi_Demux_SectionSetDefaultFilterSettings(&egbtFilterSettings);
							
		egbtFilterSettings.mask[0]=0x00;
	
		egbtFilterSettings.coefficient[0]=0xA0;
		CelthApi_Demux_SetSectionFilter(hEgbtFilterArray[i],&egbtFilterSettings);

		CelthApi_Demux_EnableSectionFilter(hEgbtFilterArray[i]);
	}
	
	
	
	
}

*/









CELTH_VOID bcastPsiSearch_setTimeout( CELTH_INT patTimeout, CELTH_INT pmtTimeout )
{
	return ;
}

CELTH_VOID bcastPsiSearch_getTimeout( CELTH_INT *patTimeout, CELTH_INT *pmtTimeout )	
{
	return ;
}

CELTH_U8 bcastPsiSearch_getEmergeChannelInfo( emerge_channel_info *p_emergechanInfo)
{
	return 0;
}


CELTH_VOID bcastPsiSearch_p_dumpBuffer(const uint8_t *p_bfr, unsigned bfrSize)
{
    unsigned i;
    for (i=0;i<bfrSize;i++)
        printf("%02X", p_bfr[i]);
    printf("\n");
}










