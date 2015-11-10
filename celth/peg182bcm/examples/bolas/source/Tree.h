
struct	NODE {
	void	*pNext;

	QUEUE	Children;
	struct NODE *Parent;
	unsigned int	Player: 2;
	unsigned int	AnchorX : 3;
	unsigned int	AnchorY : 3;

	unsigned int	VirtualPlayer: 2;
	unsigned int	NextX : 3;
	unsigned int	NextY : 3;


	char	Alpha;
	char	Beta;
	unsigned char	Depth : 4;
	unsigned char	Finished: 1;
	unsigned char	Reserved: 3;
    int	Value;
	unsigned Board[7];

};


void	AddNode( NODE& Tree, NODE *pNode );
void	RemoveNode( NODE *Node ) ;

