#include "celth_stddef.h"
#include "celth_rtos.h"
#include "celth_list.h"
#include "celth_debug.h"


#include "gbemg_stddef.h"
#include "gbemg_gendef.h"

#include "gbemg_type.h"
#include "gbemg_index.h"
#include "gbemg_content.h"
#include "gbemg_emglist.h"


#include "gbemg_contentmacro.h"
#include "gbemg_contentpriv.h"




bool IsValidContentTable(U8* SegTable)
{
	if(SegTable)
	{
		if(GBEMG_CONT_SECTION_TABLE(SegTable)==GBEMG_CONT_TABELID)
		{
			return true;
		}
		
	}
	return false;
}



GBEMG_CaseContent_t* GBEMG_BuildContFromTable(U8* ContTable,U32 TableLength)
{
	GBEMG_CaseContent_t* pCont=NULL;
	
	U16 sectionlen;
	
	U8 lingualnum,i,j,lastsectnum;
	
	GBEMG_Content_Description_t* pContDescripor;
	GBEMG_Content_Description_t* pTempD;
	
	GBEMG_AuxData_t*   pAuxData;
	GBEMG_AuxData_t*   pAuxT;
	
	CelthApi_Debug_EnterFunction("GBEMG_BuildContFromTable");
			
	if(ContTable==NULL)
	{
		return pCont;
	}
	if(!IsValidContentTable(ContTable))
	{
		CelthApi_Debug_Err(("invalid cont table\n"));
		return pCont;
	}
	sectionlen=GBEMG_CONT_SECTION_LENGTH(ContTable);
	if(sectionlen>TableLength)
	{
		CelthApi_Debug_Err("error table length [%d], should be [%d]\n", TableLength,sectionlen);
		return pCont;
	}

/*	lastsectnum=TableLength/4096;

	if(lastsectnum!=0)
	{
		CelthApi_Debug_Err("there are [%d] section\n",lastsectnum);
	}
*/	
	pCont = CELTHOSL_MemoryAlloc(sizeof(GBEMG_CaseContent_t));
	
	if(pCont==NULL)
	{
		CelthApi_Debug_Err("error , no more memory to allocate for the content\n");
		
		return pCont;
	}
	
	pCont->emg_case_id = GBEMG_CONT_EBM_ID_CONFIRM(ContTable);
	
	CelthApi_Debug_Msg("pCont->emg_case_id=%x\n",pCont->emg_case_id);
	
	lingualnum = GBEMG_CONT_MULTILINGUAL_NUM(ContTable);
	pCont->emg_multilingual_description_num = GBEMG_CONT_MULTILINGUAL_NUM(ContTable);

	CelthApi_Debug_Msg("lingualnum=%x\n",lingualnum);
	
	pContDescripor = CELTHOSL_MemoryAlloc(lingualnum*sizeof(GBEMG_Content_Description_t));

	CelthApi_Debug_Msg("pContDescripor=%x\n",pContDescripor);

	CelthApi_Debug_Msg("pCont->emg_multilingual_description_num=%x\n",pCont->emg_multilingual_description_num);
	
	
	
	if(pContDescripor==NULL)
	{
		CelthApi_Debug_Err("error , no more memory to allocate for the descriptor or auxdata\n");
		
		CELTHOSL_MemoryFree(pCont);
		
		pCont=NULL;
		
		return pCont;
	}
	
	pCont->pemg_content_description = pContDescripor;
	
	/*pCont->pemg_auxiliary_data   = pAuxData;*/
	
	
	
	ContTable +=GBEMG_CONT_TABLE_HEADER_LENGTH;

	ContTable +=1;/*multilingual_content_length*/
	
	
	for(i=0;i<lingualnum;i++)
	{
		/* construct the pemg_content_description*/
		
		
		
		pTempD = (GBEMG_Content_Description_t*)(pContDescripor+i);

		CelthApi_Debug_Msg("pTempD=%x\n",pTempD);

		CelthApi_Debug_Msg("\ncontable data:\n");
		CelthApi_Debug_Msg("%x,%x,",ContTable[0],ContTable[1]);
		CelthApi_Debug_Msg("%x,%x,",ContTable[2],ContTable[3]);
		CelthApi_Debug_Msg("%x,%x,",ContTable[4],ContTable[5]);
		CelthApi_Debug_Msg("%x,%x,",ContTable[6],ContTable[7]);
		CelthApi_Debug_Msg("%x,%x,\n",ContTable[8],ContTable[9]);
		
		pTempD->language_type=GBEMG_CONTCONT_LANGUAGE_CODE(ContTable);
		
		pTempD->content_msg_length = GBEMG_CONTCONT_MSGTEXT_LENGTH(ContTable);
		
		pTempD->pcontent_msg_buffer = CELTHOSL_MemoryAlloc(sizeof(U8)*pTempD->content_msg_length);
		
		if(pTempD->pcontent_msg_buffer==NULL)
		{
			CelthApi_Debug_Err("alloc memory fail\n");
			
			CELTHOSL_MemoryFree(pContDescripor);
			
			
			CELTHOSL_MemoryFree(pCont);
			
			pCont=NULL;
			
			return pCont;
			
			
		}
		
		ContTable += 8;

		CelthApi_Debug_Msg("\ndata copy pTempD->pcontent_msg_buffer=%x,pTempD->content_msg_length=%x\n",pTempD->pcontent_msg_buffer,pTempD->content_msg_length);
		CELTHOSL_Memcpy(pTempD->pcontent_msg_buffer,&ContTable,pTempD->content_msg_length);
		
		ContTable += pTempD->content_msg_length;
		
		pTempD->agency_name_length = GBEMG_CONTCONT_AGENCYNAME_LENGTH(ContTable);

		if(pTempD->agency_name_length>256)
		{
			pTempD->agency_name_length=256;
		}		

		
		ContTable +=1;

		CelthApi_Debug_Msg("\ndata copy pTempD->agency_name=%x,pTempD->agency_name_length=%x\n",pTempD->agency_name,pTempD->agency_name_length);
		
		CELTHOSL_Memcpy(pTempD->agency_name,ContTable,pTempD->agency_name_length);
		
		ContTable += pTempD->agency_name_length;
		
		pTempD->auxiliary_data_num = GBEMG_CONTCONT_AUXILIARY_DATA_NUM(ContTable);
		
		
		pAuxData = CELTHOSL_MemoryAlloc(pTempD->auxiliary_data_num*sizeof(GBEMG_AuxData_t));
		
		
		if(pAuxData==NULL)
		{
			CelthApi_Debug_Err("No more memory for the auxiliary data\n ");
			
			
			CELTHOSL_MemoryFree(pTempD->pcontent_msg_buffer);
			
			CELTHOSL_MemoryFree(pContDescripor);
			
			
			CELTHOSL_MemoryFree(pCont);
			
			pCont=NULL;
			
			return pCont;
			
			
		}
		
		pTempD->pemg_auxiliary_data = pAuxData;

		CelthApi_Debug_Msg("pTempD->auxiliary_data_num=%x\n",pTempD->auxiliary_data_num);
		
		ContTable+=1;
		
		for(j=0;j<pTempD->auxiliary_data_num;j++)
		{
		     pAuxT = (GBEMG_AuxData_t*)(pAuxData+j);	
		     pAuxT->aux_data_type = GBEMG_CONTCONT_AUXILIARY_DATA_TYPE(ContTable);

		CelthApi_Debug_Msg("pAuxT->aux_data_type=%x\n",pAuxT->aux_data_type);
	
		     
		     
		     pAuxT->aux_data_length = GBEMG_CONTCONT_AUXILIARY_DATA_LENGTH(ContTable);
		     
		     ContTable += 4;
		     
		     pAuxT->paux_data_buffer = CELTHOSL_MemoryAlloc(pAuxT->aux_data_length*sizeof(U8));
		     
		     if(pAuxT->paux_data_buffer==NULL)
		     {
		     	
		     	CelthApi_Debug_Err("No more memory for the auxiliary data buffer\n");
			
			
			CELTHOSL_MemoryFree(pAuxData);
			
			CELTHOSL_MemoryFree(pTempD->pcontent_msg_buffer);
			
			
			
			CELTHOSL_MemoryFree(pContDescripor);
			
			
			CELTHOSL_MemoryFree(pCont);
			
			pCont=NULL;
			
			return pCont;
		     	
		     	
		     }

			CelthApi_Debug_Msg("\ndata copy pAuxT->paux_data_buffer=%x,pAuxT->aux_data_length=%x\n",pAuxT->paux_data_buffer,pAuxT->aux_data_length);	
		     
		     CELTHOSL_Memcpy(pAuxT->paux_data_buffer,ContTable,pAuxT->aux_data_length);
		     
		     ContTable += pAuxT->aux_data_length;
		    
		}
		
	}

	CelthApi_Debug_LeaveFunction("GBEMG_BuildContFromTable");
	
	return pCont;	
	
}


/* implement it later*/

GBEMG_CaseContent_t* GBEMG_BuildContFromMultiTable(U8* MultiContTable,U32 MultiTableLength)
{

 	int	i,secnum,seclength,totallength;

	U16 ebiid,nextebiid;

	U8 vernum,nextver,tableid;

	U8*  pTempbuf;
	U8* pTempbufHeader;

	GBEMG_CaseContent_t* pCont;

	CelthApi_Debug_EnterFunction("GBEMG_BuildContFromMultiTable");
	if(MultiContTable==NULL)
	{
		return NULL;
	}

	

	pTempbuf=(U8*)CELTHOSL_MemoryAlloc(MultiTableLength*sizeof(U8));

	pTempbufHeader=pTempbuf;

	CelthApi_Debug_Msg("\nAlloc BufSize=%x, pos=0x%08x\n",MultiTableLength,pTempbuf);

	if(pTempbuf==NULL)
	{
			return NULL;
	}

	totallength=0;
	
	secnum=MultiTableLength/4096;

	ebiid=GBEMG_CONT_EBM_ID_CONFIRM(MultiContTable);

	vernum=GBEMG_CONT_VERSION_NUM(MultiContTable);

	CELTHOSL_Memcpy(pTempbuf,MultiContTable,GBEMG_CONT_TABLE_HEADER_LENGTH);

	pTempbuf+=GBEMG_CONT_TABLE_HEADER_LENGTH;

	totallength+=GBEMG_CONT_TABLE_HEADER_LENGTH;

	CelthApi_Debug_Msg("secnum=%x,totallength=%x\n",secnum,totallength);

	CelthApi_Debug_Msg("\nebiid=%x,vernum=%x\n",ebiid,vernum);



CelthApi_Debug_Msg("\n0Get data sectbuf[30]=%x,sectbuf[31]=%x\n",*(MultiContTable+30),*(MultiContTable+31));	

CelthApi_Debug_Msg("\n1Get data sectbuf[30]=%x,sectbuf[31]=%x\n",*(MultiContTable+4096+30),*(MultiContTable+4096+31));

CelthApi_Debug_Msg("\n2Get data sectbuf[30]=%x,sectbuf[31]=%x\n",*(MultiContTable+2*4096+30),*(MultiContTable+2*4096+31));

CelthApi_Debug_Msg("\n3Get data sectbuf[30]=%x,sectbuf[31]=%x\n",*(MultiContTable+3*4096+30),*(MultiContTable+3*4096+31));			



	for(i=0;i<secnum;i++)
	{
		
		
		nextebiid=GBEMG_CONT_EBM_ID_CONFIRM(MultiContTable);
		nextver  = GBEMG_CONT_VERSION_NUM(MultiContTable);
		tableid = *(MultiContTable);

		CelthApi_Debug_Msg("\n i=%x, tableid=%x ",i,tableid);
		CelthApi_Debug_Msg("nextebiid=%x,nextver=%x\n",nextebiid,nextver);

  CelthApi_Debug_Msg("\nloop get data sectbuf[30]=%x,sectbuf[31]=%x\n",*(MultiContTable+30),*(MultiContTable+31));

		if((tableid==GBEMG_CONT_TABELID)&&(ebiid==nextebiid)&&(vernum==nextver))
		{
			seclength=GBEMG_CONT_SECTION_LENGTH(MultiContTable);

			CelthApi_Debug_Msg("\nthe content table seclength = %x\n",seclength);

	CelthApi_Debug_Msg("MultiContTable[6]=%x,MultiContTable[7]=%x\n",MultiContTable[GBEMG_CONT_TABLE_HEADER_LENGTH-3],MultiContTable[GBEMG_CONT_TABLE_HEADER_LENGTH-2]);

	CelthApi_Debug_Msg("MultiContTable[30]=%x,MultiContTable[31]=%x\n",MultiContTable[30],MultiContTable[31]);

			CELTHOSL_Memcpy(pTempbuf,(MultiContTable+GBEMG_CONT_TABLE_HEADER_LENGTH-1),seclength-4-3-5); /* we did not  care CRC 4,right now,delete the table id, section length 3, and ebm id, version number,section number, lastsection number 5*/

			
		}

		pTempbuf+=seclength-4-3-5;

		totallength+=seclength-4-3-5;

                CelthApi_Debug_Msg("the totallength = %x\n",totallength);
		
		MultiContTable+=4096;
		


		
	}
		
        CelthApi_Debug_Msg("the content table total length = %x\n",totallength);
	
	pCont=GBEMG_BuildContFromTable(pTempbufHeader,totallength);

	CELTHOSL_MemoryFree(pTempbufHeader);
	CelthApi_Debug_LeaveFunction("GBEMG_BuildContFromMultiTable");

	return pCont;
	
	
}





