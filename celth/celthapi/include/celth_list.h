

#ifndef CELTH_SLIST_H
#define CELTH_SLIST_H

/*================== Module Overview =====================================
This modules defines macros to control singly-linked list.
All operations are typesafe (doesn't required typecasting) and constant time.

This list allow:
  o Insert a new entry at the head of the list
  o Insert a new entry after any element in the list
  o O(1) removal of an entry from the list head
  o O(n) removal of any entry from the list
  o Forward traversal through the list
  o Each element requires one pointers
  o Code size and execution time is the smallest and fastest compared to other lists

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
     CELTHLST_S_HEAD(block_head, block);
     struct block_head  head;

Returns:
    <none>
****************************************************************************/
#define CELTHLST_S_HEAD(name, type) struct name { struct type *sl_first;}

/***************************************************************************
Summary:
    Defines links entry

Description:
    Defines entrys for the list inside the user structure.for the element.

Input:
    type - the datatype for element

Example:
     struct block {
        CELTHLST_S_ENTRY(block) link;
        char string[256];
     };

Returns:
    <none>
****************************************************************************/
#define CELTHLST_S_ENTRY(type) struct { struct type *sl_next; }

/***************************************************************************
Summary:
    Initializes lists head

Description:
    Initializes the head of the list. The head shall be initialized before list can be used.
    This macro used for dynamic initialization.

Input:
    phead - pointer to the list head

See also:
    CELTHLST_S_INITIALIZER

Example:
    CELTHLST_S_INIT(&head);

Returns:
    <none>
****************************************************************************/
#define CELTHLST_S_INIT(phead)    (phead)->sl_first = NULL

/***************************************************************************
Summary:
    Initializes lists head

Description:
    Initializes the head of the list. The head shall be initialized before list can be used.
    This macro used for static initialization.

Input:
    head - pointer to the list head

See also:
    CELTHLST_S_INIT

Example:
    static struct block_head  head = CELTHLST_S_INITIALIZER(head);

Returns:
    <none>
****************************************************************************/
#define CELTHLST_S_INITIALIZER(head) { NULL }


/***************************************************************************
Summary:
    Tests if list is empty

Description:
    Tests if list is empty.

Input:
    phead - pointer to the list head

Returns:
    true - list empty
    false - list has elements

Example:
    if (CELTHLST_S_EMPTY(&head) { return ; }

****************************************************************************/
#define CELTHLST_S_EMPTY(phead) ((phead)->sl_first==NULL)

/***************************************************************************
Summary:
    Returns the lists first element

Description:
    Returns pointer to the first element from the list

Input:
    phead - pointer to the list head

Returns:
    pointer to the first element from the list.

Example:
    struct block *first=CELTHLST_S_FIRST(&head);
****************************************************************************/
#define CELTHLST_S_FIRST(phead) (phead)->sl_first

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
    struct block *second=CELTHLST_S_NEXT(first, link);
****************************************************************************/
#define CELTHLST_S_NEXT(pitem, field) (pitem->field.sl_next)

#define CELTHLST_INIT_NEXT(pitem,field) ((pitem->field).sl_next=NULL)

/***************************************************************************
Summary:
    Inserts element into the list

Description:
    Inserts new element into the head of the list.

Input:
    phead - pointer to the list head
    pitem - pointer to the new element
    field - name of the elements link field

Returns:
    <none>

Example:
    CELTHLST_S_INSERT_HEAD(&head, new_block, link);
****************************************************************************/
#define CELTHLST_S_INSERT_HEAD(phead, pitem, field) do { \
                (pitem)->field.sl_next = (phead)->sl_first; \
                (phead)->sl_first = (pitem); \
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
    CELTHLST_S_INSERT_AFTER(&head, first, second, link);
****************************************************************************/
#define CELTHLST_S_INSERT_AFTER(head, elm, new_elm, field) do { \
        (new_elm)->field.sl_next = (elm)->field.sl_next; \
        (elm)->field.sl_next = new_elm; \
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
    CELTHLST_S_REMOVE

Returns:
    <none>

Example:
    CELTHLST_S_REMOVE_HEAD(&head, link);
****************************************************************************/
#define CELTHLST_S_REMOVE_HEAD(phead, field) (phead)->sl_first = (phead)->sl_first->field.sl_next


/***************************************************************************
Summary:
    Removes element from the list

Description:
    Removes element from the of the list. This implementation is O(n),
    where n it's position of the element in the list

Input:
    head - pointer to the list head
    elm - pointer to the list element
    type - datatype for an element of the list
    field - name of the elements link field

See also:
    CELTHLST_S_REMOVE_HEAD

Returns:
    <none>

Example:
    CELTHLST_S_REMOVE(&head, first, block, link);
****************************************************************************/
#define CELTHLST_S_REMOVE(phead, elm, type, field) do { \
        if ((phead)->sl_first == (elm)) { \
             CELTHLST_S_REMOVE_HEAD(phead, field);\
        } else { \
            struct type *blst_s_remove_cur; \
                                    \
            for (blst_s_remove_cur=(phead)->sl_first; blst_s_remove_cur->field.sl_next != (elm); blst_s_remove_cur = blst_s_remove_cur->field.sl_next) {} \
            blst_s_remove_cur->field.sl_next=blst_s_remove_cur->field.sl_next->field.sl_next; \
        } \
    } while(0)

/***************************************************************************
Summary:
    Removes element from the list

Description:
    Removes next element from the list.

Input:
    head - pointer to the list head
    elm - pointer to the element from the list
    field - name of the elements link field

See also:
    CELTHLST_S_REMOVE

Returns:
    <none>

Example:
    CELTHLST_S_REMOVE_NEXT(elm, link);
****************************************************************************/
#define CELTHLST_S_REMOVE_NEXT(head, elm, field) \
           elm->field.sl_next=elm->field.sl_next->field.sl_next;

/***************************************************************************
Summary:
    Adds new entry to the dictionary

Description:
    Adds new element to the dictionary implemented as sordted single linked list

Input:
    head - pointer to the list head
    elm - pointer to the element from the list
    type - datatype for an element of the list
    key - name of the key element
    link - name of the elements link field
    duplicate - label that would be used (gotoed) if duplicate element is found

See also:
    CELTHLST_S_DICT_FIND
    CELTHLST_S_DICT_REMOVE

Returns:
    <none>

Example:
    CELTHLST_S_DICT_ADD(head, elm, block, key, link, err_duplicate);
****************************************************************************/
#define CELTHLST_S_DICT_ADD(head, elm, type, key, link, duplicate) do { \
    struct type *blst_s_dict_add_i, *blst_s_dict_add_prev; \
    for(blst_s_dict_add_prev=NULL,blst_s_dict_add_i=CELTHLST_S_FIRST((head));blst_s_dict_add_i!=NULL;blst_s_dict_add_prev=blst_s_dict_add_i,blst_s_dict_add_i=CELTHLST_S_NEXT(blst_s_dict_add_i, link)) { \
        if((elm)->key > blst_s_dict_add_i->key) {  break;} \
        if((elm)->key == blst_s_dict_add_i->key) { goto duplicate; } \
    } \
    if(blst_s_dict_add_prev) { CELTHLST_S_INSERT_AFTER((head), blst_s_dict_add_prev, (elm), link);} \
    else { CELTHLST_S_INSERT_HEAD(head, (elm), link);}\
    } while(0)

/***************************************************************************
Summary:
    Finds element in the dictionary

Description:
    Finds element in the dictinary with matching key

Input:
    head - pointer to the list head
    elm - pointer to the element from the list
    key - value of key to find
    field - name of the key element
    link - name of the elements link field

See also:
    CELTHLST_S_DICT_ADD
    CELTHLST_S_DICT_REMOVE

Returns:
    elm = NULL, if element not found in the list
    elm - pointer to the found element

Example:
    CELTHLST_S_DICT_FIND(head, elm, 123, key, link);
****************************************************************************/
#define CELTHLST_S_DICT_FIND(head, elm, key, field, link) do { \
    for((elm) = CELTHLST_S_FIRST(head); (elm)!=NULL ; (elm) = CELTHLST_S_NEXT((elm), link)) { \
        if(key == (elm)->field )  { break; } \
        else if( key > (elm)->field ) { (elm) = NULL;break; } } } while(0)


/***************************************************************************
Summary:
    Removed element in the dictionary

Description:
    Removes element in the dictinary with matching key

Input:
    head - pointer to the list head
    elm - pointer to the element from the list
    type - datatype for an element of the list
    key - value of key to find
    field - name of the key element
    link - name of the elements link field

See also:
    CELTHLST_S_DICT_ADD
    CELTHLST_S_DICT_FIND

Returns:
    elm = NULL, if element not found in the list
    elm - pointer to the found element

Example:
    CELTHLST_S_DICT_REMOVE(head, elm, 123, block, key, link);
****************************************************************************/
#define CELTHLST_S_DICT_REMOVE(head, elm_, key, type, field, link) do { \
    struct type *blst_s_dict_remove_prev, *blst_s_dict_remove_elm; \
    for(blst_s_dict_remove_prev=NULL,(blst_s_dict_remove_elm)=CELTHLST_S_FIRST((head));(blst_s_dict_remove_elm)!=NULL;blst_s_dict_remove_prev=blst_s_dict_remove_elm, blst_s_dict_remove_elm=CELTHLST_S_NEXT((blst_s_dict_remove_elm), link)) { \
        if( (key) == (blst_s_dict_remove_elm)->field ) { \
            if(blst_s_dict_remove_prev) { CELTHLST_S_REMOVE_NEXT((head), blst_s_dict_remove_prev, link); } \
            else { CELTHLST_S_REMOVE_HEAD((head), link);} break; \
        } else if( key > blst_s_dict_remove_elm->field ) { blst_s_dict_remove_elm = NULL;break; } \
    } (elm_)=blst_s_dict_remove_elm;} while(0)




#endif /* CELTH_SLIST_H */







