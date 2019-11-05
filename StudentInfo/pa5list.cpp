// pa5list.cpp
// a program to test the linked list code necessary for a hash table chain

// You are not required to submit this program for pa5.

// We gave you this starter file for it so you don't have to figure
// out the #include stuff.  The code that's being tested will be in
// listFuncs.cpp, which uses the header file listFuncs.h

// The pa5 Makefile includes a rule that compiles these two modules
// into one executable.

#include <iostream>
#include <string>
#include <cassert>

using namespace std;

#include "listFuncs.h"
#include "Table.h"


int main() {
   //listFuncs test
   cout << "Construction" << endl;
   Node *myList = new Node("John", 30);
   printList(myList);
   
   cout << "containsKey(John): " << * (containsKey(myList, "John")) << endl;
   printList(myList);
   
   
   cout << "listPut(Jack, 80): " << listPut(myList, "Jack", 80) << endl;
   printList(myList);
   
   cout << "listPut(John, 50): " << listPut(myList, "John", 50) << endl;
   printList(myList);

   cout << "listRemove(Susan): " << listRemove(myList, "Susan") << endl;
   printList(myList);
   
   cout << "listRemove(John): " << listRemove(myList, "John") << endl;
   printList(myList);
   
   cout << "listRemove(Jack): " << listRemove(myList, "Jack") << endl;
   printList(myList);
   
   //Table test
   cout << "------Table test------" << endl;
   cerr << "Hash stats: " << endl;
   cout << endl;
   
   Table *test1 = new Table();
   cout << "construction: " << endl;
   test1->hashStats(cerr);
   cout << endl;
   
   Table *test3 = new Table(5);
   cout << "construction(5): " << endl;
   test3->hashStats(cerr);
   cout << endl;
   
   cout << "test1 lookup(John): " << test1->lookup("John") << endl << endl;
   
   cout << "test1 insert(John, 80): " << test1->insert("John", 80) << endl;
   test1->hashStats(cerr);
   test1->printAll();
   cout << endl;
   
   cout << "test3 insert(John, 80): " << test3->insert("John", 80) << endl;
   test3->hashStats(cerr);
   test3->printAll();
   cout << endl;
   
   cout << "test3 insert(Susan, 80): " << test3->insert("Susan", 80) << endl;
   test3->hashStats(cerr);
   test3->printAll();
   cout << endl;
   
   cout << "test3 lookup(Susan, 80): " << *(test3->lookup("Susan")) << endl;
   test3->hashStats(cerr);
   test3->printAll();
   cout << endl;
   
   cout << "test3 remove(Susan)" << test3->remove("Susan") << endl;
   test3->hashStats(cerr);
   test3->printAll();
   cout << endl;
   
   cout << "test3 remove(John)" << test3->remove("John") << endl;
   test3->hashStats(cerr);
   test3->printAll();
   cout << endl;
   
   cout << "test3 remove(Jack)" << test3->remove("Jack") << endl;
   test3->hashStats(cerr);
   test3->printAll();
   cout << endl;
   
   return 0;
}
