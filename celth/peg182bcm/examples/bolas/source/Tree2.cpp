#include "moves.h"
#include "tree.h"


void	AddNode( NODE& Tree, NODE *pNode )
{
	pNode->Parent = &Tree;
	pNode->Player = Tree.Player;
	pNode->VirtualPlayer = 1 - Tree.VirtualPlayer;
	pNode->Alpha = 0;
	pNode->Beta = 0;
	pNode->Depth = Tree.Depth + 1;
   //pNode->Value = -100;
	InitQueue( &pNode->Children );
	InsertQueueElement( &Tree.Children, (ELEMENT *)pNode );
}

void	RemoveNode( NODE *Node )
{					  	  
	if ( !Node )
		return;

	while( Node->Children.wSize )
	{
		NODE *pNode = (NODE *)RemoveQueueElement( &Node->Children );
		if ( pNode )
		{
			RemoveNode( pNode );

		}				 
	}
	if ( Node->Parent )
		delete Node;
}

void	BuildTree()
{
	NODE	Tree;
	Tree.Parent = 0;
	Tree.Player = 0;
	Tree.Alpha = 0;
	Tree.Beta = 0;
	InitQueue( &Tree.Children );
	for ( int i = 0; i < 1000; i++ )
	{
		NODE *pNewNode = new NODE;
		AddNode( Tree,pNewNode );
		for ( int j = 0; j < 2; j++ )
		{
			NODE *pNewNode2 = new NODE;
			AddNode( *pNewNode,pNewNode2 );
		}			 
	}				  
	
	while( Tree.Children.wSize )
	{
		NODE *pNode = (NODE *)RemoveQueueElement( &Tree.Children );
		RemoveNode( pNode );
	}											  
}

