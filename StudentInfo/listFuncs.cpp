#include <iostream>

#include <cassert>

#include "listFuncs.h"


using namespace std;

Node::Node(const string &theKey, int theValue) {
   key = theKey;
   value = theValue;
   next = NULL;
}

Node::Node(const string &theKey, int theValue, Node *n) {
   key = theKey;
   value = theValue;
   next = n;
}

bool listRemove(ListType & list, string target) {
   bool removed = false;
   ListType p = list;
   if ( containsKey(list, target) == NULL ) { return removed; } //not contains key
   else {//contains Key
      if(p->key == target) { // first element is the target
         if(p->next == NULL) { //first element is the target in one length list
            delete list;
            list = NULL;
         }
         else {
            ListType removeTarget = p;
            p = p->next;
            list = p;
            delete removeTarget;
         }
         removed = true;
      }
      else {//target is not first element
         ListType p = lookUpBeforeTarget(list, target);
         ListType removeTarget = p->next;
         p->next = p->next->next;
         delete removeTarget;
         removed = true;
      }
      return removed;
   }
}

bool listPut(ListType & list, string putKey, int putValue) {
   bool addOrChange = false;
   if ( containsKey(list, putKey) == NULL) { //not contains the key: insert
      list = new Node(putKey, putValue, list);
      addOrChange = true;
   }
   else { //contains the key: update
      ListType p = lookUpBeforeTarget(list, putKey);
      p = p->next;
      p->value = putValue;
      addOrChange = true;
   }
   return addOrChange;
}

int * containsKey(ListType & list, string target) {
   ListType p = list;
   do {
      if(p->key == target) { return &(p->value); }
      p = p->next;
   } while( p != NULL );
   return NULL;
}
       
ListType lookUpBeforeTarget(ListType & list, string target) {
   ListType p = list;
   while(p != NULL) {
      if(p->next->key == target) { break;}
      p = p->next;
   }
   return p;
}

void printList (ListType & list) {
   ListType p = list;
   //if the ListType is empty
   if (p == NULL) { 
      return;
   }
   while(p != NULL) {
      cout << p->key << " " << p->value << endl;
      p = p->next;
   }
}



   


