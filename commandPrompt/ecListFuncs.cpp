 /*  See ecListFuncs.h for specification of each function.
 *
 *  NOTE: remove the print statements below as you implement each function
 *  or you will receive no credit for that function.
 *
 */

#include <iostream>
#include <vector>

#include "ecListFuncs.h"

using namespace std;

int linkedListSize(ListType list);


ListType vectorToList(const vector<int> & nums) {
   ListType list = NULL;
   ListType current = NULL;
   
   if( nums.size() <= 0 ) { return NULL; }
   
   list = current = new Node(nums[0]);   
   for(unsigned int i = 1; i < nums.size(); i++) {
      current = current->next = new Node(nums[i]);
   }
   return list;
}

int countRuns(ListType list) {
   //empty list or 1 element
   if (list == NULL || list->next == NULL) { return 0; }
   
   //list has more than one element
   int count = 0;
   bool consecutiveNums = false;
   int previous = list->data;
   int current;
   ListType p = list->next;
   
   //search from the second element
   while(p != NULL) {
      current = p->data;
      if(previous == current) {
         if(!consecutiveNums) { 
            ++count; 
            consecutiveNums = true;
         }
      }
      else { consecutiveNums = false; }      
      previous = current;
      p = p->next;
   }
   return count;
}



ListType reverse(ListType list) {
   //empty list
   if (list == NULL) { return list; }
   //list is not empty
   ListType listNew = NULL;
   ListType p = list;
   
   while(p != NULL) {
      insertFront(listNew, p->data);
      p = p->next;
   }
   return listNew;
}



void removeMiddle(ListType &list) {
   //empty list
   if(list == NULL) { return; }
   
   //not empty list
   ListType p = list;
   ListType tmp = NULL;
   int size = linkedListSize(list);

   if (size == 1) { 
      tmp = list; 
      list = NULL;
   }
   else if (size == 2) {
      tmp = list;
      list = list->next;
   }
   else {
      int midIndex = (size-1)/2;
      for(int i = 0; i < midIndex-1; i++) {
         p = p->next;
      }
      tmp = p->next;
      p->next = p->next->next;
   }
   delete tmp;
}


void split(ListType &list, int splitVal, ListType &a, ListType &b) {
   ListType prev = NULL;
   ListType tmp = list;
   
   a = list;
   list = NULL;

   while(tmp != NULL) {      
      if(tmp->data == splitVal) {
         //first element ==  splitVal
         if(prev != NULL) {
            prev->next = NULL;
         } 
         else {
            a = NULL;
         }
         b = tmp->next;
         delete tmp;
         return;
      }
      prev = tmp;
      tmp = tmp->next;      
   }
}


//count linked list size
//return the length of the linked list
int linkedListSize(ListType list) {
   int length = 0;
   while(list != NULL){
      list = list->next;
      length++;
   }
   return length;
}