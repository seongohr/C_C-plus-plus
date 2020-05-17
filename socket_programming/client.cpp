/**
Name : Seongoh Ryoo

Comment : 
I started my code for TCP Server-Client implementation from Beej's guide.
Other code I refered are commented below.

**/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>

using namespace std;

#define SERVER_AWS_PORT "34214"
#define LOCALHOST "127.0.0.1"
#define MAXBUFLEN 1024
#define NUM_ARG 5

// get sockaddr, IPv4:
void *get_in_addr(struct sockaddr *sa)
{
    return &(((struct sockaddr_in*)sa)->sin_addr);
}

// * split string by delimiter(char) and put it into a vector
// * reference (https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c)
vector<string> split(const string &inputs, char delim) {
    vector<string> result;
    stringstream tmp (inputs);
    string token;

    while (getline (tmp, token, delim)) {
        result.push_back(token);
    }

    return result;
}

// * print received result 
void printMSG_result(string input, string mapId, string src, string des) {
    vector<string> res = split(input, '&');
    
    if (res[0] == "NoMap") {
        cout << "No map id " 
             << mapId 
             << " found" << endl << endl;
    }
    else if (res[0] == "NoVertex") {
        cout << "No vertex id " 
             << res[1] 
             << " found" 
             << endl << endl;
    }
    else {
        float p_delay = stof(res[2]);
        float t_delay = stof(res[3]);
        float o_delay = stof(res[4]);
        float m_length = stof(res[1]);
        //print message received from aws
        cout << "The client has received results from AWS:" << endl
             << "---------------------------------------------------------------" << endl
             << "Source\tDestination\tMin Length\tTt\tTp\tDelay" << endl
             << "---------------------------------------------------------------" << endl
             << "   " << src << "\t\t" 
             << des << "\t" 
             << fixed << setprecision(2) << m_length << "\t\t" 
             << fixed << setprecision(2) << t_delay << "\t" 
             << fixed << setprecision(2) << p_delay << "\t" 
             << fixed << setprecision(2) << o_delay <<endl
             << "---------------------------------------------------------------" << endl
             << "Shortest path: " << res[0] << endl << endl;
    }
}

// * booting message
void printMSG_bootup() {
    cout << "The client is up and running\n"
         << endl;
}

// * pring message after sending the received data from user to aws
void printMSG_sent_aws(string mapId, string src, string des, string file_size) {
    cout << "The client has sent query to AWS using TCP: start vertex "
         << src // source vertex index
         << ", destination vertex "
         << des // destination vertex index
         << ", map "
         << mapId // Map ID
         << ", file size "
         << file_size // file size
         << endl << endl;
}

int main(int argc, char *argv[]) {
    // prepare for TCP
    int sockfd, numbytes;
    char buf_send[MAXBUFLEN],  buf_recv[MAXBUFLEN];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET_ADDRSTRLEN];

    // print booting message
    printMSG_bootup();

    /**
    argv[1] : Map ID
    argv[2] : source vertex index
    argv[3] : destination vertex index
    argv[4] : file size
    **/
    if (argc != NUM_ARG) {
        fprintf(stderr,"usage: client <Map ID> <Source Vertex Index> <Destination Vertex Index> <File Size> \n");
        exit(1);
    }

    //filling server information to connect to
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    if ((rv = getaddrinfo(LOCALHOST, SERVER_AWS_PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
    
    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("client: socket\n");
            continue;
        }
        
        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect\n");
            continue;
        }
        break;
    }
    
    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }
    
    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
    
    freeaddrinfo(servinfo); // all done with this structure

    string mapId(argv[1]);
    string source(argv[2]);
    string destination(argv[3]);
    string file_size(argv[4]);
    
    /**put received data to buf for sending aws
    argv[1] : Map ID
    argv[2] : source vertex index
    argv[3] : destination vertex index
    argv[4] : file size
    **/
    for (int i = 1; i < NUM_ARG; i++) {
        if (i == 1) {
            strcpy(buf_send, argv[i]);
        }
        else {
            strcat(buf_send, argv[i]);
        }
        if (i != NUM_ARG - 1) {
            strcat(buf_send, "&");
        }
    }
    
    //send to aws
    if (send(sockfd, buf_send, strlen(buf_send), 0) == -1) {
        perror("client: send\n");
        exit(EXIT_FAILURE);
    }

    //print message after sending to aws
    printMSG_sent_aws(mapId, source, destination, file_size);
    
    if ((numbytes = recv(sockfd, (char *)buf_recv, MAXBUFLEN-1, 0)) == -1) {
        perror("recv\n");
        exit(EXIT_FAILURE);
    }
    buf_recv[numbytes] = '\0';

    // store received data
    string result(buf_recv);
    // extract data received from aws
    printMSG_result(result, mapId, source ,destination);
    // close socket
    close(sockfd);

    return 0;
}
