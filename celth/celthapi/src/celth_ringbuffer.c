#include <stdlib.h>   
#include <stdio.h>   



#include "celth_stddef.h"

#include "celth_mutex.h"


#include "celth_ringbuffer.h"





CELTH_Status_t celth_rb_init (Celth_ringbuf **ppbuf, CELTH_INT allocsize)
{

	Celth_ringbuf *ring;  

	if(ppbuf==NULL || allocsize < 1024)  
	{  
		return CELTH_ERROR_BAD_PARAMETER;  
	}  



	ring = malloc(sizeof (Celth_ringbuf));  
   if(ring == NULL)  
   {  
	   return CELTH_ERROR_NO_MEMORY;	
   }

        ring->rw_lock = CELTH_SemCreate( "ringbufferrw", CELTHOSL_FifoNoTimeOut, 1 );

        if(ring->rw_lock==NULL)
        {
                return CELTH_ERROR_FAIL;
        }


  

	ring->size = 1;  
    while(ring->size < allocsize)  
        ring->size <<= 1;  /*size = 1, 2, 4, 8, 16...*/

	
	/*alloc ring->buffer, size = ring->size*/  
		ring->buffer=malloc(sizeof(CELTH_CHAR)*(ring->size));  
	   
		if(ring->buffer == NULL)  
		{  
			free(ring);  
			return CELTH_ERROR_NO_MEMORY;  
		}  
		 
		*ppbuf = ring; 


return CELTH_NO_ERROR;


}
CELTH_VOID celth_rb_destroy (Celth_ringbuf * pbuf)
{
	if(pbuf==NULL)
	{
	return;
	}
	CELTH_SemWait(pbuf->rw_lock,CELTHOSL_WaitForEver);
	free(pbuf->buffer);
	free(pbuf);
	CELTH_SemPost(pbuf->rw_lock);
	CELTH_SemDestroy(pbuf->rw_lock);
		
}
CELTH_INT celth_rb_write (Celth_ringbuf *pbuf, CELTH_CHAR* pdata, CELTH_INT datalength)
{

	int total;	
   	int i;  
 
   /*total = rb space left*/	

   if(pbuf==NULL)
   	{
   		return 0;
   	}
   total = celth_rb_free(pbuf);  
 
   /* total = len =min(space, len) */  
   if(datalength > total)	
   	{
   		printf("\nclear the ringbuf\n");
	   celth_rb_clear(pbuf); 
   	
/* now the total is the total buffer length*/
total=celth_rb_free(pbuf);		
if(datalength>total)
{
 datalength=total;
}						
	}	
 
   i = pbuf->wr_pointer;	


CELTH_SemWait(pbuf->rw_lock,CELTHOSL_WaitForEver);

/* if the data length exceed the ringbuffer left space*/

if(i + datalength > pbuf->size)  
    {  
    	/* write the data to the top of the ringbuffer  from the ringbuffer write pointer(ringbuffer->wr_pointer , the write length is the ringbuffer left space length*/
        memcpy(pbuf->buffer + i, pdata, pbuf->size - i);  
        pdata += pbuf->size - i;  
        datalength -= pbuf->size - i;   
        pbuf->data_size += pbuf->size - i;  
        i = 0;  
    }  

/* write the left data the to ringbuffer bottom , if the data length exceed the left space*/
/* or if the data length did not exceed the left space , wirte the data to the ringbuffer write pointer directly*/
memcpy(pbuf->buffer + i, pdata, datalength);  
pbuf->wr_pointer = i + datalength;  
pbuf->data_size += datalength;  

CELTH_SemPost(pbuf->rw_lock);

return datalength;  


   



}

CELTH_INT celth_rb_free (Celth_ringbuf *pbuf)
{
	if(pbuf)
	{
	return (pbuf->size - pbuf->data_size);
	}
	


}

CELTH_INT celth_rb_read (Celth_ringbuf *pbuf, CELTH_CHAR* prdata,CELTH_INT maxlength)
{


int total;	
int i;	
/* total = max = min(used, len) */	

if(pbuf==NULL)
 {
	 return 0;
 }
CELTH_SemWait(pbuf->rw_lock,CELTHOSL_WaitForEver);


total = celth_rb_data_size(pbuf);  

if(maxlength > total)  
	maxlength = total;  
else  
	total = maxlength;  

i = pbuf->rd_pointer;  
if(i + maxlength > pbuf->size)	
{  
	/*read the left data in the top of the ringbuf,( the data left in the ringbuffer->buffer) */
	memcpy(prdata, pbuf->buffer + i, pbuf->size - i);	
	prdata += pbuf->size - i;  
	maxlength -= pbuf->size - i;  
	pbuf->data_size -= pbuf->size - i;	
	i = 0;	
}  
/* after read the left data in the ringbuffer top , read the data from the bottom of the ringbuffer*/ 
/* or read the data from the ringbuffer directly, when the data did not ring in the buffer*/
memcpy(prdata, pbuf->buffer + i, maxlength);  
pbuf->rd_pointer = i + maxlength;  
pbuf->data_size -= maxlength;  

CELTH_SemPost(pbuf->rw_lock);

/*return total;*/

return pbuf->data_size;  




}
  
CELTH_INT celth_rb_data_size (Celth_ringbuf *pbuf)
{
	if(pbuf)
	{
	return pbuf->data_size;
	}
}
CELTH_Status_t celth_rb_clear (Celth_ringbuf *pbuf)
{

	if(pbuf==NULL)
		{
		return 1;
		}


		CELTH_SemWait(pbuf->rw_lock,CELTHOSL_WaitForEver);

       	memset(pbuf->buffer,0,pbuf->size);	
	    pbuf->rd_pointer=0;  
        pbuf->wr_pointer=0;  
        pbuf->data_size=0;  

		
		CELTH_SemPost(pbuf->rw_lock);

		
        return 0;  
}









