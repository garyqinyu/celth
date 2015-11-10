#ifndef URGBCAST_H__
#define URGBCAST_H__


#ifdef __cplusplus
extern "C" {
#endif


CELTH_VOID emerge_Platform_Init(CELTH_VOID* context);
CELTH_VOID emerge_PsipmtControlInit();




/* the emerge broadcast threads start and stop*/

CELTH_VOID emerge_PsipmtMonitor_start();

CELTH_VOID emerge_PsipmtChangeHandle_start();

CELTH_VOID emerge_Psiegbtmonitor_start();
CELTH_VOID emerge_Psiegbtmonitor_stop();

CELTH_VOID emerge_egbtChangHandle_start();


CELTH_VOID emerge_ebActtionHandle_start();


CELTH_VOID emerge_UartDataMonitor_start();


/* the emerge broadcast data structure interface */


/* the emerge broadcast pmt table info */


CELTH_BOOL  EBcastDataInfoInit();

emerge_pmttable_info* GetPmtTableInfo();

CELTH_VOID  SetPmtTableInfo(emerge_pmttable_info* info);

/* the emerge broadcast egbt table info */

emerge_channel_info*  GetOldEgbtTableInfo();

CELTH_VOID  SetOldEgbtTableInfo(emerge_channel_info* info);

emerge_channel_info*  GetNewEgbtTableInfo();

CELTH_VOID  SetNewEgbtTableInfo(emerge_channel_info* info);

CELTH_BOOL  SetNewEgbtTableInfo2OldEgbtTableInfo();

CELTH_BOOL ComapareEgbtTable2NewEgbtTable(emerge_channel_info* info);
CELTH_BOOL ComapareNewEgbtTable2OldEgbtTable();



/*************************************************************************/
/* command analysis interface */
/**************************************************************************/

/* should consider the following state:

 IF the last command is TURN_OFF, only TURN_ON is reasonable one for the right now command.

 */
 /******************************************************************************/

 



Emerge_Cmd_t  EmergeBcast_CommandAnalysis(emerge_channel_info* info );

CELTH_VOID EmergeBcast_CommandPointerReset();


/*************************************************************************/

/* send the cmd to the control thread*/

CELTH_BOOL SendBcastMsg2control(Emerge_Cmd_t cmd);












#ifdef __cplusplus
}
#endif

#endif /* URGBCAST_H__ */


