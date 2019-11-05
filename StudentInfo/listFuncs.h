//*************************************************************************
// Node class definition 
// and declarations for functions on ListType

// Note: we don't need Node in Table.h
// because it's used by the Table class; not by any Table client code.

// Note2: it's good practice to not put "using" statement in header files.  Thus
// here, things from std libary appear as, for example, std::string

#ifndef LIST_FUNCS_H
#define LIST_FUNCS_H
//#include <string>
  
using namespace std;

struct Node {
   std::string key;
   int value;

   Node *next;

   Node(const std::string &theKey, int theValue);

   Node(const std::string &theKey, int theValue, Node *n);
   
   
};


typedef Node * ListType;

//*************************************************************************
//add function headers (aka, function prototypes) for your functions
//that operate on a list here (i.e., each includes a parameter of type
//ListType or ListType&).  No function definitions go in this file.

//put the set of a key and a value
//insert the key and a value when the key doesn't exist and returns true
//change the value when the key already exists and returns true
//returns false when the key is not exist
//PRE : ListType(list) passed is not NULL
bool listPut(ListType & list, std::string putKey, int putValue);

//remove the set of a target key and a value when the target key exists
//returns true when removed and returns false when the target key doesn't exist
//PRE : ListType(list) passed is not NULL
bool listRemove(ListType & list, std::string target);

//check if the target key exists
//returns the location of the set of the key and the value
//returns NULL when the target key doesn't exist
//PRE : ListType(list) passed is not NULL
int * containsKey(ListType & list, std::string target);

//lookup the Listtype one before the target key and returns it
//PRE : ListType(list) passed is not NULL
//PRE : target exists in the list
//PRE : the length of ListType(list)  > 1
ListType lookUpBeforeTarget(ListType & list, std::string target);

//print all the elements in the ListType linkedlist
void printList(ListType & list);










// keep the following line at the end of the file
#endif
