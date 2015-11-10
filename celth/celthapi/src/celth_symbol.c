
#include "celth_stddef.h"

#include "celth_symbol.h"

#include "celth_mutex.h"


#include "stdlib.h"
#include "string.h"





typedef struct nvasymbol_s	nvasymbol_t;
struct nvasymbol_s
{
	nvasymbol_t		*next;
	CELTH_VOID		*value;
	CELTH_CHAR		symbol[2];
};

CELTH_STATIC nvasymbol_t		*root = NULL;
/*CELTH_STATIC CELTHOSL_Sem_t	symbol;*/
CELTH_STATIC CELTHOSL_SemId_t	symbol_lock;


/* =====================================================================*/
/*****Local find functions*/
/*===================================================================== */

CELTH_STATIC nvasymbol_t *find_value( CELTH_VOID *value )
{
	nvasymbol_t   *current;

    CELTH_SemWait( symbol_lock, CELTHOSL_WaitForEver );
    for( current = root; current != NULL; current = current->next )
    {
		if( current->value == value ) 
			break;
    }
    CELTH_SemPost( symbol_lock );

    return current;
}

/* --- */

CELTH_STATIC nvasymbol_t *find_exact( CELTH_CONST CELTH_CHAR *symbol )
{
	nvasymbol_t   *current;

    CELTH_SemWait( symbol_lock, CELTHOSL_WaitForEver );
    for( current = root; current != NULL; current = current->next )
    {
		if( strcmp(current->symbol, symbol) == 0 ) 
			break;
    }
    CELTH_SemPost( symbol_lock );

    return current;
}

/* --- */

CELTH_STATIC nvasymbol_t *find_inexact( CELTH_CONST CELTH_CHAR *symbol )
{
	nvasymbol_t   *current;

	 CELTH_SemWait( symbol_lock, CELTHOSL_WaitForEver );
    for( current = root; current != NULL; current = current->next )
    {
		if( strncmp(current->symbol, symbol, strlen(symbol)) == 0 ) 
			break;
    }
    CELTH_SemPost( symbol_lock );

    return current;
}




CELTH_BOOL  CELTH_Symbol_Init( CELTH_VOID* context )
{
    symbol_lock	= CELTH_SemCreate( "CelthSymbol", CELTHOSL_FifoNoTimeOut, 1 );

	if(symbol_lock!=NULL)
		{
    return FALSE;
		}
	return TRUE;

}
CELTH_VOID	CELTH_Symbol_Register( CELTH_CONST CELTH_CHAR *symbol, CELTH_VOID *value )
{

	
		nvasymbol_t  *symbol_structure;
	
	/* --- */
	
		symbol_structure = find_exact( symbol );
		if( symbol_structure != NULL )
		{
			symbol_structure->value = value;
		}
		else
		{
			symbol_structure = (nvasymbol_t *)CELTHOSL_MemoryAlloc( sizeof(nvasymbol_t)+strlen(symbol) );
	
			if( symbol_structure == NULL )
			{
					printf("error alloc symbol\n");
				return;
			}
	
			symbol_structure->next	= root;
			root					= symbol_structure;
			symbol_structure->value = value;
			strcpy( symbol_structure->symbol, symbol );
		}
}
CELTH_BOOL 	CELTH_Symbol_Enquire_Symbol( CELTH_CONST CELTH_CHAR **symbol, CELTH_VOID *value )
{
	nvasymbol_t  *symbol_structure;

    symbol_structure = find_value( value );
    if( symbol_structure == NULL )
    {
		*symbol = NULL;
		return TRUE;
    }
    else
    {
		*symbol = symbol_structure->symbol;
		return FALSE;
    }

}
CELTH_BOOL  CELTH_Symbol_Enquire_Value_Inexact( CELTH_CONST CELTH_CHAR *symbol, CELTH_VOID **value )
{
	nvasymbol_t  *symbol_structure;

    symbol_structure = find_inexact( symbol );
    if( symbol_structure == NULL )
    {
		*value = NULL;
		return TRUE;
    }
    else
    {
		*value = symbol_structure->value;
		return FALSE;
    }


}
CELTH_BOOL 	CELTH_Symbol_Enquire_Value( CELTH_CONST CELTH_CHAR *symbol, CELTH_VOID **value )
{
		nvasymbol_t  *symbol_structure;
	
	
		symbol_structure = find_exact( symbol );
		if( symbol_structure == NULL )
		{
			*value = NULL;
			return TRUE;
		}
		else
		{
			*value = symbol_structure->value;
			return FALSE;
		}

}







