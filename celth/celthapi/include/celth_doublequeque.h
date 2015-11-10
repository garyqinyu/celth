#ifndef CELTHLST_DOUBLEQUEUE_H
#define CELTHLST_DOUBLEQUEUE_H   

/*================== Module Overview =====================================
This modules defines macros to control doubly linked queue.
All operations are typesafe (doesn't required typecasting) and constant time.

This list allow:
  o Insert a new entry at the head of the list
  o Insert a new entry after/before any element in the list
  o O(1) removal of any entry in the list
  o Forward traversal through the list
  o Each element requires two pointers
  o Code size and execution time is about 2.5 that for singly-linked list
  
========================================================================*/

 
/***************************************************************************
Summary:
    Creates new data type for the list head
	
Description:
    Creates new data type for the list head, this type used to create variable for the lists head.
    User should create new the list head data type for every different element datatype.
	
Input:
	name - name for the new data type (structure)
    type - existing user data type used for element of the list

Example:    
     CELTHLST_Q_HEAD(block_head, block);
     struct block_head  head;

Returns:
    <none>
****************************************************************************/
#define CELTHLST_Q_HEAD(name, type) struct name { struct type *q_first, *q_last; }

/***************************************************************************
Summary:
    Defines links entry
	
Description:
    Defines entrys for the list inside the user structure.for the element.
	
Input:
    type - the datatype for element

Example:
     struct block {
        CELTHLST_Q_ENTRY(block) link;
        char string[256];
     };  

Returns:
    <none>
****************************************************************************/
#define CELTHLST_Q_ENTRY(type)  struct { struct type *q_next, *q_prev; }

/***************************************************************************
Summary:
    Initializes lists head
	
Description:
    Initializes the head of the list. The head shall be initialized before list can be used.
    This macro used for dynamic initialization.
	
Input:
    head - pointer to the list head

See also:
    CELTHLST_Q_INITIALIZER

Example:
    CELTHLST_Q_INIT(&head);

Returns:
    <none>
****************************************************************************/
#define CELTHLST_Q_INIT(head) ((head)->q_last = (head)->q_first=NULL)

/***************************************************************************
Summary:
    Initializes lists head
	
Description:
    Initializes the head of the list. The head shall be initialized before list can be used.
    This macro used for static initialization.
	
Input:
    head - pointer to the list head

See also:
    CELTHLST_Q_INIT

Example:
    static struct block_head  head = CELTHLST_Q_INITIALIZER(&head);

Returns:
    <none>
****************************************************************************/
#define CELTHLST_Q_INITIALIZER(head) {NULL, NULL}

/***************************************************************************
Summary:
    Tests if list is empty
	
Description:
    Tests if list is empty.

Input:
    head - pointer to the list head

Returns:
    true - list empty
    false - list has elements

Example:
    if (CELTHLST_Q_EMPTY(&head) { return ; }

****************************************************************************/
#define CELTHLST_Q_EMPTY(head) ((head)->q_first == NULL)

/***************************************************************************
Summary:
    Returns the lists first element
	
Description:
    Returns pointer to the first element from the list
	
Input:
    head - pointer to the list head

Returns:
    pointer to the first element from the list.

Example:
    struct block *first=CELTHLST_Q_FIRST(&head);
****************************************************************************/
#define CELTHLST_Q_FIRST(head) ((head)->q_first)

/***************************************************************************
Summary:
    Returns the lists first element
	
Description:
    Returns pointer to the last element from the list

Input:
    head - pointer to the list head

Returns:
    pointer to the last element from the list.

Example:
    struct block *first=CELTHLST_Q_FIRST(&head);
****************************************************************************/
#define CELTHLST_Q_LAST(head) ((head)->q_last)

/***************************************************************************
Summary:
    Returns next element from the lists
	
Description:
    Returns pointer to the next element from the list
	
Input:
    elm - pointer to the list element
    field - name of the elements link field

Returns:
    pointer to the next element from the list

Example:
    struct block *second=CELTHLST_Q_NEXT(first);
****************************************************************************/
#define CELTHLST_Q_NEXT(elm, field) ((elm)->field.q_next)


/***************************************************************************
Summary:
    Returns next element from the lists
	
Description:
    Returns pointer to the previous element from the list
	
Input:
    elm - pointer to the list element
    field - name of the elements link field

Returns:
    pointer to the previous element from the list

Example:
    struct block *first=CELTHLST_Q_PREV(second);
****************************************************************************/
#define CELTHLST_Q_PREV(elm, field) ((elm)->field.q_prev)

/***************************************************************************
Summary:
    Inserts element into the list

Description:
    Inserts new element into the head of the list.

Input:
    head - pointer to the list head
    elm - pointer to the new element
    field - name of the elements link field

Returns:
    <none>

Example:
    CELTHLST_Q_INSERT_HEAD(&head, new_block, link);
****************************************************************************/
#define CELTHLST_Q_INSERT_HEAD(head, elm, field) do { \
    if ( ((elm)->field.q_next = (head)->q_first) != NULL ) (head)->q_first->field.q_prev = (elm); \
    if ( (head)->q_last == NULL) (head)->q_last = (elm); \
    (head)->q_first = (elm); (elm)->field.q_prev = NULL;}  while(0)

/***************************************************************************
Summary:
    Inserts element into the list
	
Description:
Description:
    Inserts new element into the tail list.
	
Input:
    head - pointer to the list head
    elm - pointer to the new element
    field - name of the elements link field

Returns:
    <none>

Example:
    CELTHLST_Q_INSERT_TAIL(&head, elm, link);
****************************************************************************/
#define CELTHLST_Q_INSERT_TAIL(phead, pitem, field) do {				\
                (pitem)->field.q_next = NULL;						\
                (pitem)->field.q_prev = (phead)->q_last;			\
                if ((phead)->q_last) {								\
                        (phead)->q_last->field.q_next = (pitem);	\
                } else {											\
                        (phead)->q_first = (pitem);					\
                }													\
                (phead)->q_last = (pitem);							\
               } while(0)
    

/***************************************************************************
Summary:
    Inserts element into the list
	
Description:
    Inserts new element after existing element.
	
Input:
    head - pointer to the list head
    elm - pointer to the element from the list
    new_elm - pointer to the new element
    field - name of the elements link field

Returns:
    <none>

Example:
    CELTHLST_Q_INSERT_AFTER(&head, first, second, link);
****************************************************************************/
#define CELTHLST_Q_INSERT_AFTER(head, elm, new_elm, field) do { \
        (new_elm)->field.q_prev = (elm); \
        if (((new_elm)->field.q_next = (elm)->field.q_next)!=NULL)  (elm)->field.q_next->field.q_prev = (new_elm); \
        if ((elm)->field.q_next == NULL)  (head)->q_last = (new_elm);\
        (elm)->field.q_next = (new_elm); } while(0)

/***************************************************************************
Summary:
    Inserts element into the list
	
Description:
    Inserts new element after existing element.
	
Input:
    head - pointer to the list head
    elm - pointer to the element from the list
    new_elm - pointer to the new element
    field - name of the elements link field

Returns:
    <none>

Example:
    CELTHLST_Q_INSERT_BEFORE(&head, second, first, link);
****************************************************************************/
#define CELTHLST_Q_INSERT_BEFORE(head, elm, new_elm, field) do { \
        (new_elm)->field.q_next = (elm); \
        if (((new_elm)->field.q_prev = (elm)->field.q_prev)!=NULL) elm->field.q_prev->field.q_next = new_elm; \
        else (head)->q_first = (new_elm); \
        (elm)->field.q_prev = (new_elm); } while(0)



/***************************************************************************
Summary:
    Removes element from the list

Description:
    Removes element from the list.

Input:
    head - pointer to the list head
    elm - pointer to the list element
    field - name of the elements link field

See also:
    CELTHLST_Q_REMOVE_HEAD

Returns:
    <none>

Example:
    CELTHLST_Q_REMOVE(&head, first, link);
****************************************************************************/
#define CELTHLST_Q_REMOVE(head, elm, field) do { \
    if ((elm)->field.q_next) (elm)->field.q_next->field.q_prev = (elm)->field.q_prev;  else (head)->q_last = (elm)->field.q_prev; \
    if ((elm)->field.q_prev) (elm)->field.q_prev->field.q_next = (elm)->field.q_next; else (head)->q_first = (elm)->field.q_next; \
   } while(0)

/***************************************************************************
Summary:
    Removes element from the list

Description:
    Removes element from the head of the list.

Input:
    head - pointer to the list head
    field - name of the elements link field

See also:
    CELTHLST_Q_REMOVE

Returns:
    <none>

Example:
    CELTHLST_Q_REMOVE_HEAD(&head, first, link);
****************************************************************************/
#define CELTHLST_Q_REMOVE_HEAD(head, field) do { \
    (head)->q_first = (head)->q_first->field.q_next; \
    if ((head)->q_first) { (head)->q_first->field.q_prev = NULL;} else (head)->q_last=NULL; \
   } while(0)


#endif /* CELTHLST_DOUBLEQUEUE_H  */


