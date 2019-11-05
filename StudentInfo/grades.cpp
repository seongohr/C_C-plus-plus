/*
 * grades.cpp
 * A program to test the Table class.
 * How to run it:
 *      grades [hashSize]
 * 
 * the optional argument hashSize is the size of hash table to use.
 * if it's not given, the program uses default size (Table::HASH_SIZE)
 *
 */

#include "Table.h"

// cstdlib needed for call to atoi
#include <cstdlib>


using namespace std;

void insertOrChange(string &command);
void lookupOrRemove(string &command);
void proceed(Table * & table);
void help();

int main(int argc, char * argv[]) {

   // gets the hash table size from the command line

   int hashSize = Table::HASH_SIZE;

   Table * grades;  // Table is dynamically allocated below, so we can call
   // different constructors depending on input from the user.

   if (argc > 1) {
      hashSize = atoi(argv[1]);  // atoi converts c-string to int

      if (hashSize < 1) {
         cout << "Command line argument (hashSize) must be a positive number" 
              << endl;
         return 1;
      }

      grades = new Table(hashSize);

   }
   else {   // no command line args given -- use default table size
      grades = new Table();
   }


   grades->hashStats(cout);

   // add more code here
   // Reminder: use -> when calling Table methods, since grades is type Table*
   
   proceed(grades);
            
   return 0;
}

//process the command when a user's command input is insert or change
//insert : insert the name and the score when the name doesn't exists in the Table
//         doesn't make a change in the Table and print the message when the name exists
//change : change the score when the name exists
//         doesn't make a change in the Table and print the meessage when the name doesn't exists
void insertOrChange(string &command, Table * & table) {
   string name; 
   int score;
   cin >> name >> score;
   if(command == "insert" ) { 
      bool result = table->insert(name, score);
      if (! result) { cout << name << " is already present." << endl; }
   }
   else { //in case of change
      int *val = table->lookup(name);
      if (val == NULL) { //doesn't exists the name : print the error message
         cout << name << " is not present in the table." << endl;
      }
      else { //when the name exists : change the score
         *val = score;
      }
   }
}

//process when a user's command input is lookup or remove
//lookup : print the score of the name or the error message when the name doesn't exists.
//remove : remove the the name and the score or 
//         print the error message when the name doesn't exists.
void lookupOrRemove(string &command, Table * & table) {
   string name;         
   cin >> name;
   if (command == "lookup") {
      int * val = table->lookup(name);
      //the received name doesn't exists            
      if (val == NULL) {
         cout << name << " is not present in the table." << endl;
      }
      //print the score when the name exists
      else {
         cout << * val << endl;
      }
   }
   //in case of remove
   else {
      bool result = table->remove(name);
      //print the message when the name wasn't found in the table
      if(! result) { cout << name << " is not present in the table." << endl; }
   }
}

//get a user's input and process it case by case
void proceed(Table * & table) {
   while(true){
      string command;
      cout << "cmd>";
      cin >> command;
      //stop the program when the command is quit
      if (command == "quit" ) { break; }
      if (command == "insert" || command == "change") { insertOrChange(command, table); }
      else if (command == "lookup" || command == "remove") { lookupOrRemove(command, table); }
      //print all the entries in the Table
      else if (command == "print") { table->printAll(); }
      //print the number of entries in the Table
      else if (command == "size") { cout << table->numEntries() << endl; }
      //print the status of Table
      else if (command == "stats") { table->hashStats(cout); }
      //print the help message
      else if (command == "help") { help(); }
      //print the error message and help message when the wrong command is received
      else { 
         cout << "ERROR: invalid command"<< endl;    
         help();
      }   
   }
}

//print the help message 
void help() {
   cout << "--------------------------command summary--------------------------" << endl;
   cout << "insert name score : Insert this name and score in the grade table." << endl;
   cout << "change name score: Change the score for name." << endl;
   cout << "remove name: Remove this student" << endl;
   cout << "print: Prints out all name and scores in the table" << endl;
   cout << "size: Prints out the number of entries in the table" << endl;
   cout << "stats: Prints out statistics about the hash table at this point" << endl;
   cout << "help: Prints out a brief command summary." << endl;
   cout << "quit: Exits the program" << endl;
}

             
             
             



