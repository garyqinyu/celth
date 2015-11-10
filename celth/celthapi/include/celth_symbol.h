#ifndef	CELTH_SYMBOL_H_
#define	CELTH_SYMBOL_H_


#ifdef __cplusplus 
extern "C" {
#endif

CELTH_BOOL  CELTH_Symbol_Init( CELTH_VOID* context );
CELTH_VOID	CELTH_Symbol_Register( CELTH_CONST CELTH_CHAR *symbol, CELTH_VOID *value );
CELTH_BOOL 	CELTH_Symbol_Enquire_Symbol( CELTH_CONST CELTH_CHAR **symbol, CELTH_VOID *value );
CELTH_BOOL  CELTH_Symbol_Enquire_Value_Inexact( CELTH_CONST CELTH_CHAR *symbol, CELTH_VOID **value );
CELTH_BOOL 	CELTH_Symbol_Enquire_Value( CELTH_CONST CELTH_CHAR *symbol, CELTH_VOID **value );

#ifdef __cplusplus
}
#endif

#endif /*CELTH_SYMBOL_H_*/



