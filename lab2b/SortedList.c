#include "SortedList.h"
/**
 * SortedList_insert ... insert an element into a sorted list
 *
 *	The specified element will be inserted in to
 *	the specified list, which will be kept sorted
 *	in ascending order based on associated keys
 *
 * @param SortedList_t *list ... header for the list
 * @param SortedListElement_t *element ... element to be added to the list
 *
 * Note: if (opt_yield & INSERT_YIELD)
 *		call pthread_yield in middle of critical section
 */
void SortedList_insert(SortedList_t *list, SortedListElement_t *element)
{
  SortedListElement_t *current = list->next;
  SortedListElement_t *temp;
  
  while(current != list)
  {
     if (strcmp(element->key,current->key)<=0)
     break;
     else
      current=current->next;
  }
   if (opt_yield & INSERT_YIELD)
   {
   pthread_yield();
  // printf("insert\n");
   }
   temp=current->prev;
   element->prev=temp;
   element->next=current;
    temp->next =element;
   current->prev=element;
}
/**
 * SortedList_delete ... remove an element from a sorted list
 *
 *	The specified element will be removed from whatever
 *	list it is currently in.
 *
 *	Before doing the deletion, we check to make sure that
 *	next->prev and prev->next both point to this node
 *
 * @param SortedListElement_t *element ... element to be removed
 *
 * @return 0: element deleted successfully, 1: corrtuped prev/next pointers
 *
 * Note: if (opt_yield & DELETE_YIELD)
 *		call pthread_yield in middle of critical section
 */
int SortedList_delete( SortedListElement_t *element)
{
   SortedListElement_t *ptemp = element->prev;
   SortedListElement_t *ntemp = element->next; 
   
   if (opt_yield & DELETE_YIELD)
   {
     pthread_yield();
   // printf("delete\n");
    }  
   if(ptemp->next!=element |ntemp->prev !=element)
     return 1;
   else
   {
      ntemp->prev=ptemp;
      ptemp->next=ntemp;
      element->prev=NULL;
      element->next=NULL;
      return 0;
   }   

}
/**
 * SortedList_lookup ... search sorted list for a key
 *
 *	The specified list will be searched for an
 *	element with the specified key.
 *
 * @param SortedList_t *list ... header for the list
 * @param const char * key ... the desired key
 *
 * @return pointer to matching element, or NULL if none is found
 *
 * Note: if (opt_yield & SEARCH_YIELD)
 *		call pthread_yield in middle of critical section
 */
SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key)
{
  SortedListElement_t *current = list->next;
  
  if (opt_yield & SEARCH_YIELD)
    {
    pthread_yield();
   // printf("look\n;");
    }
  while(current != list)
    {
      if(strcmp(key, current->key)==0)
	      return current;
      else
       current = current->next;
    }
    
  return NULL;
}
/**
 * SortedList_length ... count elements in a sorted list
 *	While enumeratign list, it checks all prev/next pointers
 *
 * @param SortedList_t *list ... header for the list
 *
 * @return int number of elements in list (excluding head)
 *	   -1 if the list is corrupted
 *
 * Note: if (opt_yield & SEARCH_YIELD)
 *		call pthread_yield in middle of critical section
 */
int SortedList_length(SortedList_t *list)
{
  int num=0;
  SortedListElement_t *current = list->next;
  if (opt_yield & SEARCH_YIELD)
    pthread_yield();
  while(current != list)
    {
      num++;
      current = current->next;
    }
  return num;
}




