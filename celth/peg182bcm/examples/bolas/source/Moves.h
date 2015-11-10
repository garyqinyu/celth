

struct ELEMENT
	{
	ELEMENT *pNext;
	};

struct	QUEUE
	{
	ELEMENT	*pHead;
	ELEMENT	*pTail;
	short	wSize;
	};	

ELEMENT	*RemoveQueueElement( QUEUE *pQueue );
void	InsertQueueElement( QUEUE *pQueue, ELEMENT *pNewElement );
void	InitQueue( QUEUE *pQueue );

