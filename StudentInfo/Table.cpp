// Table.cpp  Table class implementation


#include "Table.h"

#include <iostream>
#include <string>
#include <cassert>

using namespace std;


// listFuncs.h has the definition of Node and its methods.  -- when
// you complete it it will also have the function prototypes for your
// list functions.  With this #include, you can use Node type (and
// Node*, and ListType), and call those list functions from inside
// your Table methods, below.

#include "listFuncs.h"


//*************************************************************************


Table::Table() {
   hashSize = HASH_SIZE;
   data = new ListType[hashSize];
   numOfEntry = 0;
   nonEmptyBucket = 0;
   initialization();
   numOfChains = new int[hashSize];
   intArrInitialization();
}
 

Table::Table(unsigned int hSize) {
   hashSize = hSize;
   data = new ListType[hashSize];
   numOfEntry = 0;
   nonEmptyBucket = 0;
   initialization();
   numOfChains = new int[hashSize];
   intArrInitialization();
}


int * Table::lookup(const string &key) {
   if(numOfEntry != 0) {
      ListType n = data[hashCode(key)];
      if (n != NULL) { return containsKey(n, key); }
   }
   return NULL;
}

bool Table::remove(const string &key) {
   int index = hashCode(key);
   bool result = false;
   
   if(numOfEntry != 0) {
      ListType n = data[index];
      if(n != 0) {//not null: the list is not null
         result = listRemove(n, key);
         if (result == true){
            data[index] = n;
            numOfEntry--;
            numOfChains[index]--;
            if(numOfChains[index] == 0) { nonEmptyBucket--; }
         }
      }
      
   }
   return result;
}

bool Table::insert(const string &key, int value) {
   int index = hashCode(key);
   ListType n = data[index];
   bool result = false;
   
   if (n == NULL) {
      n = new Node(key, value);
      data[index] = n;
      numOfChains[index]++;
      nonEmptyBucket++;
      numOfEntry++;
      result = true;
   }
   else {
      if (containsKey(n, key) != NULL) { return result; } // if contains: not change
      result = listPut(n, key, value);
      data[index] = n;
      numOfChains[index]++;
      numOfEntry++;
   }
   return result;
}

int Table::numEntries() const {
   return numOfEntry;      
}

void Table::printAll() const {
   for(int i = 0; i < hashSize; i++) {
      ListType n = data[i];
      if(n != NULL) {
         printList(n); 
      }   
   }
}


void Table::hashStats(ostream &out) const {
   out << "number of buckets: " << hashSize << endl;  
   out << "number of entries: " << numOfEntry << endl;
   out << "number of non-empty buckets: " << nonEmptyBucket << endl;
   out << "longest chain: " << getLongestChain() << endl;
}


// add definitions for your private methods here
void Table::initialization() {
   for(int i = 0; i < hashSize; i++) {
      data[i] = NULL;
   }
}

void Table::intArrInitialization() {
   for(int i = 0; i < hashSize; i++) {
      numOfChains[i] = 0;
   }
}
int Table::getLongestChain() const {
   int longest = numOfChains[0];
   for(int i = 1; i < hashSize; i++) {
      if (longest < numOfChains[i] ) {
         longest = numOfChains[i];
      }
   }
   return longest;
}