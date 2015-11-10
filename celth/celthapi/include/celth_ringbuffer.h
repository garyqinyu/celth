#ifndef __CELTH_RINGBUFFER_H
#define __CELTH_RINGBUFFER_H

#ifdef __cplusplus

extern "C" {

#endif

#include "celth_stddef.h"





typedef struct Celth_ringbuf  
{  
    CELTH_CHAR *buffer;         
    CELTH_INT wr_pointer;       
    CELTH_INT rd_pointer;     
    CELTH_INT data_size;       /* buffer size */ 
    CELTH_INT size;            /* rb size  */
	CELTHOSL_SemId_t	rw_lock;
}Celth_ringbuf;  
  

/* ring buffer functions */  
CELTH_Status_t celth_rb_init (Celth_ringbuf **ppbuf, CELTH_INT allocsize);  
CELTH_VOID celth_rb_destroy (Celth_ringbuf * pbuf);  
CELTH_INT celth_rb_write (Celth_ringbuf *pbuf, CELTH_CHAR* pdata, CELTH_INT datalength);  
CELTH_INT celth_rb_free (Celth_ringbuf *pbuf);  
CELTH_INT celth_rb_read (Celth_ringbuf *pbuf, CELTH_CHAR* prdata,CELTH_INT maxlength);  
  
CELTH_INT celth_rb_data_size (Celth_ringbuf *pbuf);  
CELTH_Status_t celth_rb_clear (Celth_ringbuf *pbuf);  








#ifdef __cplusplus

}

#endif






#endif





