/**
Name : Seongoh Ryoo

Comment : 
I started my code for TCP Server-Client implementation from Beej's guide and UDP Server implementation
from internet source-geeksforgeeks- below.
https://www.geeksforgeeks.org/explicitly-assigning-port-number-client-socket/
Other code I refered are commented below.

**/

#include <stdio.h> //C Standard Input and Output Library //http://www.cplusplus.com/reference/cstdio/
#include <stdlib.h> //C Standard General Utilities Library http://www.cplusplus.com/reference/cstdlib/
#include <unistd.h> // Header file that provides access to the POSIX operating system API
#include <errno.h> // Holds the error code for the last system call
#include <string.h> // C Strings
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> //in_port_t, in_addr_t
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>

using namespace std;

#define PORT_AWS_TCP "34214"
#define PORT_AWS_UDP 33214
#define PORT_SERVER_A 30214
#define PORT_SERVER_B 31214
#define PORT_SERVER_C 32214
#define BACKLOG 10 // how many pending connections queue will hold
#define MAXDATASIZE 1024
#define LOCALHOST "127.0.0.1"

void sigchld_handler(int s)
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;
    while(waitpid(-1, NULL, WNOHANG) > 0);
    errno = saved_errno;
}
// get sockaddr, IPv4:
void *get_in_addr(struct sockaddr *sa){
    return &(((struct sockaddr_in*)sa)->sin_addr);
}
    
//print array - for debugging
void printArray(vector<string> arr) {
    cout << "print array" <<endl;
    for (int i = 0; i < arr.size(); i++) {
        cout << arr[i] << endl;
    }
}

// * split string by delimiter(char) and put it into a vector
// * reference (https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c)
vector<string> split (const string &inputs, char delim) {
    vector<string> result;
    stringstream tmp (inputs);
    string token;

    while (getline (tmp, token, delim)) {
        result.push_back(token);
    }

    return result;
}

//print booting message
void printMSG_bootup() {
    cout << "The AWS is up and running.\n" << endl;
}
//print message after receiving data from client
void printMSG_recv_client(vector<string> information) {
    cout << "The AWS has received map ID " << information[0]
         << ", start vertex " << information[1]
         << ", destination vertex " << information[2]
         << " and file size " << information[3]
         << " from the client using TCP over port " << PORT_AWS_TCP
         << endl
         << endl;
}

//print message after sending server A/server B using UDP
void printMSG_sent_mapId(char server) {
    cout << "The AWS has sent mapID to server "
         <<  server
         << " using UDP over port "
         << PORT_AWS_UDP
         << endl
         << endl;
}

//print message after receiving map information from server A/server B using UDP
void printMSG_recv_mapInfo(char server) {
    cout << "The AWS has received map information from server "
         << server
         << endl
         << endl;
}

//print message when node(vertice)s are found in the map
void printMSG_hasNodes() {
    cout << "The source and desetination vertex are in the graph\n" << endl;
}

//print message when a node is not foundin the map
void printMSG_noNodes(string node) {
    cout << node
         << " vertex not found in the graph, sending error to client using TCP over port "
         << PORT_AWS_TCP
         << endl
         << endl;
}

// print message when no map found
void printMSG_noMap(string mapId) {
    cout << "Map "
         << mapId
         << " was not found, sending error to client using TCP over port "
         << PORT_AWS_TCP
         << endl
         << endl;
}

//print message after sending data to server C
void printMSG_sent_C() {
    cout << "The AWS has sent map, source ID, destination ID, propagation speed"
         << " and transmission speed to server C using UDP over port "
         << PORT_AWS_UDP
         << endl
         << endl;
}

// print message after receiving data from server C
void printMSG_rcv_C(vector<string> result) {
    float p_delay = std::stof(result[2]);
    float t_delay = std::stof(result[3]);

    cout << "The AWS has received results from server C:\n"
         << " * Shortest path: "
         << result[0] << endl
         << " * Shortest distance: "
         << fixed << setprecision(2) << result[1] << " km\n"
         << " * Transmission delay: "
         << fixed << setprecision(2) << t_delay << " s\n"
         << " * Propagation delay: "
         << fixed << setprecision(2) << p_delay << " s\n"
         << endl;
}

// print message after sending a result to client
void printMSG_sent_client() {
    cout << "The AWS has sent calculated results to client using TCP over port "
         << PORT_AWS_TCP
         << endl
         << endl;
}

// find vertices in the array
bool hasVertice(string src, string des, vector<string> res, string* noVertice) {
    string token;
    char delim = ' ';
    int idx;
    bool has_src = false;
    bool has_des = false;
 
    for (int i = 3; i < res.size(); i++) {
        stringstream tmp (res[i]);
        idx = 0;
        while (getline (tmp, token, delim)) {
            if (idx < 2) { // idx for only checking edges not the distance
                if ( has_src == false && src == token) {
                    has_src = true;
                }
                if ( has_des == false && des == token) {
                    has_des = true;
                }
            }
            if (has_src == true && has_des == true) {
                return true;
            }
            ++idx;   
        }
    }
    // store vertice info when not exists
    if (has_src == false) {
        *noVertice += src;
    }

    if (has_des == false) {
        if ((*noVertice).size() > 0) {
            *noVertice += " / ";
        }
        *noVertice += des;
    }
    
    return false;

}

//return true when nodes are exists
bool existVertices(string src, string des, vector<string> input , string* noVertices) {
    bool has_v;
    has_v = hasVertice(src, des, input, noVertices); // true when both vertice exists

    if (has_v == false) {
        printMSG_noNodes(*noVertices);
        
    }
    else {
        printMSG_hasNodes();
    }
    
    return has_v; //return true when both vertice exists

}

// UDP communication between aws and serverA/B/C and return received data with string format
string communicate_UDP(string message, struct sockaddr_in serv_addr, int sockfd, char server ) {
    char buf_send[MAXDATASIZE], buf_receive[MAXDATASIZE];
    unsigned int len, n;
    
    strcpy(buf_send, message.c_str());
    //send
    if(sendto(sockfd, buf_send, sizeof(buf_send), 0, (struct sockaddr *) &serv_addr
            , sizeof(serv_addr)) == -1) //failure
    {
        perror("failed to send\n");
        exit(EXIT_FAILURE);
    }
    else {//success
        if (server == 'C') {//print message after sending information to server C
            printMSG_sent_C();
        }
        else {//print message after sending information to server A/B
            printMSG_sent_mapId(server);
        }
    }

    len = sizeof(serv_addr);
    //receive
    if((n = recvfrom(sockfd, (char*)buf_receive, MAXDATASIZE - 1, MSG_WAITALL 
            , (struct sockaddr *)  &serv_addr, &len)) == -1 ){
        perror("failed to receive\n");
        exit(EXIT_FAILURE);
    }
    buf_receive[n] == '\0';
    
    // store received data
    string recv_msg(buf_receive);
    return recv_msg;
}

int main()
{
    /** TCP setting**/
    int sockfd_tcp, new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char s[INET_ADDRSTRLEN];
    int rv;
    char buf[MAXDATASIZE];
    int numbytes;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; //
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; 
    if ((rv = getaddrinfo(NULL, PORT_AWS_TCP, &hints, &servinfo)) != 0) { //suceeds : 0
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd_tcp = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("AWS: TCP socket");
            continue;
        }

        if (setsockopt(sockfd_tcp, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }
        if (bind(sockfd_tcp, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd_tcp);
            perror("AWS: bind");
            continue;
        }
        break;
    }
    
    freeaddrinfo(servinfo); // all done with this structure
    
    if (p == NULL)  {
        fprintf(stderr, "AWS: failed to bind\n");
        exit(1);
    }
    if (listen(sockfd_tcp, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    //print the message when booting up
    printMSG_bootup();

    // reap all dead processes
    sa.sa_handler = sigchld_handler; 
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }
        
    /** UDP setting**/
    struct sockaddr_in my_addr, servA_addr, servB_addr, servC_addr;
    int sockfd_udp;
    
    // create socket
    if ((sockfd_udp = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        perror("UDP socket creation failed\n");
        exit(EXIT_FAILURE);
    }

    //filling information of server A
    memset(&servA_addr, 0, sizeof(servA_addr));
    servA_addr.sin_family = AF_INET;
    servA_addr.sin_addr.s_addr = inet_addr(LOCALHOST);
    servA_addr.sin_port = htons(PORT_SERVER_A);
    
    //filling information of server B
    memset(&servB_addr, 0, sizeof(servB_addr));
    servB_addr.sin_family = AF_INET;
    servB_addr.sin_addr.s_addr = inet_addr(LOCALHOST);
    servB_addr.sin_port = htons(PORT_SERVER_B);

    //filling information of server C
    memset(&servC_addr, 0, sizeof(servC_addr));
    servC_addr.sin_family = AF_INET;
    servC_addr.sin_addr.s_addr = inet_addr(LOCALHOST);
    servC_addr.sin_port = htons(PORT_SERVER_C); 

    //explicitly assigning port number by binding udp client(aws itself) with the port(PORT_AWS_UDP)
    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = inet_addr(LOCALHOST);
    my_addr.sin_port = htons(PORT_AWS_UDP);

    if (bind(sockfd_udp, (struct sockaddr *) &my_addr, sizeof(my_addr)) < 0) {
        perror("bind failed\n");
        exit(EXIT_FAILURE);
    }

    while(1) {
        // ** TCP **/
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd_tcp, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept failed\n");
            continue;
        }
        
        inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
        
        if (!fork()) { // this is the child process
            close(sockfd_tcp); // child doesn't need the listener

            //receive from a client by TCP
            if ((numbytes = recv(new_fd, buf, MAXDATASIZE - 1, 0)) == -1 ) {
                perror("recv failed\n");
                exit(EXIT_FAILURE);
            }
            buf[numbytes] = '\0';

            string mapInfotoC; // storage of a message sending to c
            string client_inputs(buf); // store received data from client
            
            //extract the information from the input
            vector<string> information = split(client_inputs, '&');
            string mapId = information[0];
            string source = information[1];
            string destination = information[2];
            string file_size = information[3];
            
            //print the input from the client
            printMSG_recv_client(information);

            // * UDP communication with serverA/B by sending map Id
            string res_A = communicate_UDP(mapId, servA_addr, sockfd_udp, 'A'); 
            printMSG_recv_mapInfo('A'); // print a message after receiving data from server A
            string res_B = communicate_UDP(mapId, servB_addr, sockfd_udp, 'B');
            printMSG_recv_mapInfo('B'); // print a message after receiving data from server B
            string result_client;// storage of the result for client

            // extract data from server A and server B
            vector<string> resultA = split(res_A, '&');
            vector<string> resultB = split(res_B, '&');

            bool has_V; // true when both src and des vertices exist
            bool no_map = false; // true when no map found
            string noVertices; // store which vertice doesn't exist

            // prepare a message for sending server C
            if (resultA[0] == "False" && resultB[0] == "False") {// no map
                no_map = true;
                printMSG_noMap(mapId); // no map : no need to send a message for C
            }
            else if ( resultA[0] != "False" ) { // server A has map -> vertice check
                has_V = existVertices(source, destination, resultA, &noVertices);
                if (has_V == true) {
                    mapInfotoC = res_A; // prepare a message for C only if both vertices exist
                }
            }
            else if ( resultB[0] != "False") { // server B has map -> vertice check
                has_V = existVertices(source, destination, resultB, &noVertices);
                if (has_V == true) {
                    mapInfotoC = res_B; // prepare a message for C only if both vertices exist
                }
            }

            // prepare a message for client 
            if ( no_map == true ) { // No map error message
                result_client = "NoMap&";
            }
            else if ( no_map == false && has_V == false ) { // No vertice error message with vertice info
                result_client = "NoVertex&" + noVertices + "&";
            }
            else {
                // send information to server C, receive the result and put it in the storage for client 
                string msg_c = source + "&" + destination + "&" + file_size + "&" + mapInfotoC;
                string res_C = communicate_UDP(msg_c, servC_addr, sockfd_udp, 'C');
                vector<string> resultC = split(res_C, '&');
                printMSG_rcv_C(resultC);
                result_client = res_C;
            }
            
            //send the result to client
            char buf_client[MAXDATASIZE];
            strcpy(buf_client, result_client.c_str());
            if (send(new_fd, buf_client, strlen(buf_client), 0) == -1) {
                    perror("failed to send to the client\n");
            }
            // print a message after sending a calculated result received from server C to the client
            if (no_map == false && has_V == true) {
                printMSG_sent_client();
            }

            close(new_fd); // close child(TCP)
            exit(0);
        }
        close(new_fd);
    }
    return 0;
}
