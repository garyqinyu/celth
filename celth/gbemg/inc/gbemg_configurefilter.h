
#ifndef _GBEMG_CONFIGFILTER_H_
#define _GBEMG_CONFIGFILTER_H_




/* 
copyright reserved by 
celtech company Co Ltd,
*/


#ifdef __cplusplus
extern "C" {
#endif



typedef struct filtermonitorparam_t{

CELTH_EventId_t ev;

CELTHDemux_SectionFilterHandle_t hf;


}filtermonitorparam_t;



filtermonitorparam_t* gbemg_begin_monitorindextable();
	




#ifdef __cplusplus
}
#endif



#endif



