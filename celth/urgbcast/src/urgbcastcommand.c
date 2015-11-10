
#include "celth_stddef.h"

#include "urgbcastdef.h"
#include "urgbcastcontrol.h"

#include "urgbcast.h"


CELTH_STATIC EMERGE_CMD_TYPE lastcmd=EMERGE_CMD_MAX;

CELTH_STATIC CELTH_U8  cmdpoint = 0; /* the current command point*/


CELTH_VOID EmergeBcast_CommandPointerReset()
{
	/*reset the command point to zero*/
	cmdpoint = 0;
}



Emerge_Cmd_t  EmergeBcast_CommandAnalysis(emerge_channel_info* info)
{
	Emerge_Cmd_t cmd ;

	cmd.cmd_type=EMERGE_CMD_MAX;
	cmd.EmergeCmdParam.EmergeInvalid.dummy=0xFF;

	
	if(info)
		{

		/* begin analysis command*/
		if(info->program_num<=cmdpoint)
			{
				goto exitreturn; 

				/*cmdpoint=0;*/
				
			}
		
			{


				if(info->emerge_program[cmdpoint].turn_on==0xFF)
					{
						if(info->emerge_program[cmdpoint].vol==0xFF)
							{
								if(lastcmd!=EMERGE_CMD_TURN_OFF)
								{cmd.cmd_type=EMERGE_CMD_PLAY_EMERGE_INFO_FROM_TS;

								cmd.EmergeCmdParam.EmergePlayStream.AudioPid   =info->emerge_program[cmdpoint].audio_pid;
								cmd.EmergeCmdParam.EmergePlayStream.Symbolrate =info->emerge_program[cmdpoint].symbolrate;
								cmd.EmergeCmdParam.EmergePlayStream.Freq       =info->emerge_program[cmdpoint].frequency;
									}
							}
						else
							{
								
							    cmd.cmd_type=EMERGE_CMD_SET_PLAY_VOL;
								cmd.EmergeCmdParam.EmergeSetVal.vol=info->emerge_program[cmdpoint].vol;
							}
					}
				else
					{
						if(info->emerge_program[cmdpoint].turn_on==0x01)
							{
								cmd.cmd_type=EMERGE_CMD_TURN_ON;
								cmd.EmergeCmdParam.EmergeTurn.dummy=0x01;
							}
						else
							{
							
								cmd.cmd_type=EMERGE_CMD_TURN_OFF;
								cmd.EmergeCmdParam.EmergeTurn.dummy=0x00;
							}
					}

		/* move the command pointer to next*/
				cmdpoint++;
		/* and set the command to the lastcmd*/
				lastcmd=cmd.cmd_type;
				
			}

		
		
			
		}

exitreturn:

	return cmd;
	

}

