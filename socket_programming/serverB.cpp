/**
Name : Seongoh Ryoo

Comment : 
I started my code for UDP Server-Client implementation from internet source-geeksforgeeks- below.
https://www.geeksforgeeks.org/udp-server-client-implementation-c/
**/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <iostream>
#include <fstream>
#include <map>

using namespace std;

#define PORT_AWS_UDP 33214
#define MY_PORT 31214
#define LOCALHOST "127.0.0.1"
#define MAXBUFLEN 1024
#define MAP_NAME "map2.txt"

//print booting message
void printMSG_bootup() {
    cout << "The Server B is up and running using UDP on port "
         << MY_PORT
         << endl
         << endl;
}

//print message after receiving data from AWS
void printMSG_received(char mapId) {
    cout << "The Server B has received input for finding graph of map "
         << mapId
         << endl
         << endl;
}

//print meessage when no map id found in the map file
void printMSG_noMap(string mapId) {
    cout << "The Server B does not have the required graph id "
         << mapId
         << endl
         << endl;
}

// print messsage after sending the result to aws
void printMSG_sent(bool hasMap) {
    if (hasMap == false) {
        cout << "The Server B has sent \"Graph not Found\" to AWS\n";
    }
    else {
        cout << "The Server B has sent Graph to AWS\n";
    }
    cout << endl;
}

// read map file and store data in the map(key:value) created
map<string, string> read_map() {
    string line;
    ifstream myfile (MAP_NAME);
    map<string, string> map;
    string key;
    if (myfile.is_open())
    {
        string info;
        while ( getline (myfile,line) )
        {   
            if (line.size() == 1 && isalpha(line.c_str()[0])) {
                key = line;
                info = "";
            }
            else {
                info += "&" + line;
            }
            map[key] = info;
        }
        myfile.close();
    }
    else {
        cout << "Unable to open file";
    }
    return map;
}

// place the found data into buf, if not found then place "False" to the buf and call printMSG_noMap
string get_map_info(map<string, string> map, char id, bool* hasMap) {
    string mapId(1, id);
    string result;

    if (map.find(mapId) == map.end()) {
        result = "False&";
        *hasMap = false;
        printMSG_noMap(mapId);
    }
    else {
        result = mapId + map[mapId];
        *hasMap = true;
    }
    return result;
}

int main(void)
{
    bool hasMap; // true if mapID exists5 in the map file
    map<string, string> map; // where the read file will be stored
    char mapId; // mapId received from aws

    //read map file
    map = read_map();

    // udp preparation
    int sockfd;
    char buf[MAXBUFLEN];
    struct sockaddr_in servaddr, cliaddr;

    //creating socket file descriptor
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    //filling server information
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET; //IPv4
    servaddr.sin_addr.s_addr = inet_addr(LOCALHOST);
    servaddr.sin_port = htons(MY_PORT);

    memset(&cliaddr, 0, sizeof(cliaddr));

    // bind the socket with the server address
    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind failed\n");
        exit(EXIT_FAILURE);
    }
    else {
        // print booting message
        printMSG_bootup();
    }

    while (1) {
        unsigned int len, n;
        len = sizeof(cliaddr);

        //receive map ID from AWS
        if ((n = recvfrom(sockfd, (char *) buf, MAXBUFLEN - 1, MSG_WAITALL, (struct sockaddr *) &cliaddr, &len)) == -1) {
            perror("failed to receive\n");
            exit(EXIT_FAILURE);
        }

        buf[n] = '\0';

        mapId = buf[0];
        printMSG_received(mapId);
        
        // get the found result, true: map found and placed the result in the buf, false: no map found  
        string result = get_map_info(map, mapId, &hasMap);
        // place the data for sending to aws
        strcpy(buf, result.c_str());
        // send the results to AWS
        if ((sendto(sockfd, (const char *)buf, strlen(buf), 0, (const struct sockaddr *) &cliaddr, len)) == -1) {
            perror("failed to send\n");
            exit(EXIT_FAILURE);
        }
        // print a message after sending the data to aws
        printMSG_sent(hasMap);
    }
    return 0;
}
