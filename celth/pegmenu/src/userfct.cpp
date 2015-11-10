#include "peg.hpp"
#include "userfct.hpp"

HorizontalScrollText_t tstTNRStatus[5];
HorizontalScrollTable_t TNRStatus_Table;

				for(iLoop = 0; iLoop<LANGUAGE_NUMBER; iLoop++)
            	{
            	tstTNRStatus[0].StringToDisplay[iLoop] = LS(SID_TNR_Auto);
            	tstTNRStatus[1].StringToDisplay[iLoop] = LS(SID_TNR_High);
            	tstTNRStatus[2].StringToDisplay[iLoop] = LS(SID_TNR_Medium);
            	tstTNRStatus[3].StringToDisplay[iLoop] = LS(SID_TNR_Min);
            	tstTNRStatus[4].StringToDisplay[iLoop] = LS(SID_TNR_OFF);
            	}
            TNRStatus_Table.ptstTable = tstTNRStatus;
            TNRStatus_Table.iTableSize = sizeof(tstTNRStatus)/sizeof(HorizontalScrollText_t);


