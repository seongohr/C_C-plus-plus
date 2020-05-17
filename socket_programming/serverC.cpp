/**
Name : Seongoh Ryoo

Comment : 
I started my code for UDP Server-Client implementation from internet source-geeksforgeeks- below.
https://www.geeksforgeeks.org/udp-server-client-implementation-c/
Other code I refered are commented below.

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
#include <bits/stdc++.h> 
#include <tuple>
#include <math.h>
#include <stack>
#include <iomanip>

using namespace std;

#define PORT_AWS_UDP 33214
#define MY_PORT 32214
#define LOCALHOST "127.0.0.1"
#define MAXBUFLEN 1024

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

// * split string by delimiter(string) and put it into a vector
// * reference (https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c)
vector<string> split (string s, string delim) {
    string token;
    vector<string> result;
    size_t start = 0, end, len_delim = delim.length();

    while ((end = s.find (delim, start)) != string::npos) {
        token = s.substr (start, end - start);
        start = end + len_delim;
        result.push_back (token);
    }

    result.push_back (s.substr (start));
    return result;
}

// * make a vertices map
map<string, map<string, float> >makeVerticesMap(vector<string> information, map<string, map<string, float> > vertices_map) {
    vector<string> info;

    for (int i = 6; i < information.size(); i++) {
        info = split(strcpy(new char[MAXBUFLEN], information[i].c_str()), ' ' );
        string v1 = info[0];
        string v2 = info[1];
        float distance = stof(info[2]);

        vertices_map.insert(make_pair(v1, map<string, float>())); 
        vertices_map.insert(make_pair(v2, map<string, float>()));
        vertices_map[v1].insert(make_pair(v2, distance));
        vertices_map[v2].insert(make_pair(v1, distance));
    }
    return vertices_map;
}

// * find the vertice with min distance in tentative map
string findNextVertice(map<string, tuple<string, float> > m) {
    string minV;
    float minD = INFINITY; 
    string key;
    float d;

    for (auto element : m)
    {
        key = element.first;
        tuple <string, float> t = element.second;
        d = get<1>(t);
        if (minD > d) {
            minV = key;
            minD = d;
        }
    }
    return minV;
}

// * make a tentative vertice map
map<string, tuple<string, float> > makeTentativeMap(map<string, map<string, float> > v_map
                                                    , map<string, tuple<string, float> > tentative
                                                    , map<string, tuple<string, float> > permanent
                                                    , string curr_v, float pre_d) 
{
    map<string, float>::iterator i_itr; //inner iterator
    string v;
    float d; // distance from previous node to current node
    float distance; // new distance through current node
    float old_d; // previous distance value to current node

    for (i_itr = v_map[curr_v].begin(); i_itr != v_map[curr_v].end(); i_itr++) {
        v = i_itr->first; //vertice
        d = i_itr->second; //distance to v
        distance = pre_d + d;
        if (permanent.count(v) == 0) { // the node doesn't exists in a permanent tree 
            if (tentative.count(v) > 0) { // node is in tentative 
                old_d = get<1>(tentative[v]);
                if (distance < old_d) { //update the distance if current distance is shorter than the old one
                    tentative[v] = make_tuple(curr_v, distance);
                }
            }
            else { // add that node and distance to tentative when not exists
                distance = pre_d + d;
                tentative[v] = make_tuple(curr_v, distance);
            }
        }
    }
    return tentative;
}

// * find path from src to des searching from destination to source
string findPath(string src, string des, map<string, tuple<string, float> > p_map) {
    string delimiter = " -- ";
    stack<string> path;
    tuple<string, float> tmp; //(previous vertice, shortest distance)
    string pre_v = des; //previous vertice, initialize with destination(starting point)
    string s_path = "";

    path.push(des); // initialize the path with the destination : find a path from des to src

    while (pre_v != src) {
        tmp = p_map[pre_v];//(previous vertice, shortest distance)
        pre_v = get<0>(tmp);//update pre_v with next previous vertice
        path.push(pre_v);//push the previous vertice to path stack
    }

    s_path += path.top();
    path.pop();
    while (!path.empty()) {
        s_path += delimiter + path.top();
        path.pop();
    }
    return s_path;
}

// * search shortest path from source to destination
// * string s: source, string d: destination
tuple<string, float> shortestPath(string src, string des, map<string, map<string, float> > v_map) {
    map<string, tuple<string, float> > permanent;
    map<string, tuple<string, float> > tentative;
    
    string curr_v = src; //current vertice (use it for making tentative map)
    float pre_d = 0; //distance to the previous vertice
    string next_v;//next vertice to be included in a permanent tree
    tuple<string, float> tmp;
    float s_distance;
    
    //initialize permanent vertices with source vertice
    permanent[src] = make_tuple(src, 0);

    while(permanent.size() < v_map.size() && permanent.count(des) == 0) {   
        //make tentative map
        tentative = makeTentativeMap(v_map, tentative, permanent, curr_v, pre_d);
        //get next vertice to be included in permanent map
        next_v = findNextVertice(tentative);
        tmp = tentative[next_v];
        //add to permanent vertices
        permanent[next_v] = make_tuple(get<0>(tmp), get<1>(tmp));
        //remove vertice from the tentative
        tentative.erase(next_v);
        //update vertices and distance with shortest distance to current vertice
        curr_v = next_v;
        pre_d = get<1>(tmp);
    }
    string s_path(findPath(src, des, permanent));
    s_distance = get<1>(permanent[des]);

    return make_tuple(s_path, s_distance);
}

// * print received data
void printMSG_received(float p_speed, float t_speed, string mapId, string src, string des) {
    cout << "The Server C has received data for calculation:\n"
         << " * Propagation speed: " << setprecision(2) << fixed << p_speed << " km/s\n"
         << " * Transmission speed: " << int(t_speed) << " KB/s\n"
         << " * map ID: " << mapId << endl
         << " * Source ID: " << src << endl
         << " * Destination ID: " << des << endl
         << endl;
}

// * print calculated data
void printMSG_caculated(string path, float distance, float t_delay, float p_delay) {
    cout << "The Server C has finished the calculation:\n"
         << " * Shortest path: " << path << endl
         << " * Shortest distance: " << distance << " km\n"
         << " * Transmission delay: " << fixed << setprecision(2) << t_delay << " s\n"
         << " * Propagation delay: " << fixed << setprecision(2) << p_delay << " s\n"
         << endl;
}

// * print message after sending data to aws
void printMSG_sent() {
    cout << "The Server C has finished sending the output to AWS."
         << endl
         << endl;
}

// * print message when boot up
void printMSG_bootup() {
    cout << "The Server C is up and running using UDP on port "
         << MY_PORT
         << endl << endl;
}

// * calculating delays
vector<float> calculateDelays(float shortest_d, float p_speed, int file_size, int t_speed) {
    vector<float> delays;
    float propagation_delay = shortest_d / p_speed;
    float transmission_delay = float(file_size) / t_speed;
    float overall_delay = propagation_delay + transmission_delay;

    delays.push_back(propagation_delay);
    delays.push_back(transmission_delay);
    delays.push_back(overall_delay);

    return delays;
}

// * convert to string
// * reference : https://stackoverflow.com/questions/2125880/convert-float-to-stdstring-in-c 
template < typename Type > std::string to_str (const Type & t)
{
  std::ostringstream os;
  os << t;
  return os.str ();
}

// * calculate Dijkstra
string calculateDijkstra(char input[MAXBUFLEN]) { 
    map<string, map<string, float> > vertices_map;//vertices map
    vector<string> information = split(input, '&');//replace tmp with input
    
    // make a vertices_map
    vertices_map = makeVerticesMap(information, vertices_map);

    vector<float> delays;
    string source = information[0];
    string destination = information[1];
    int file_size = stoi(information[2]);
    string mapId = information[3];
    float propagation_speed = stof(information[4]);
    float transmission_speed = stof(information[5]);
    string result = "";
    
    //print received data
    printMSG_received(propagation_speed, transmission_speed, mapId, source, destination);
    
    //get shortest path
    tuple<string, float> shortest = shortestPath(source, destination, vertices_map);
 
    delays = calculateDelays(get<1>(shortest), propagation_speed, file_size, transmission_speed);
    printMSG_caculated(get<0>(shortest), get<1>(shortest), delays[1], delays[0]);
    result += get<0>(shortest) + "&" + to_str(get<1>(shortest)) + "&" 
                + to_str(delays[0]) + "&" + to_str(delays[1]) + "&" + to_str(delays[2]);
    return result;
}

int main(void)
{
    // udp preparation
    int sockfd;
    char buf[MAXBUFLEN];
    struct sockaddr_in servaddr, cliaddr;

    //creating socket file descriptor
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed\n");
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
        if ((n = recvfrom(sockfd, (char *) buf, MAXBUFLEN - 1, MSG_WAITALL, (struct sockaddr *) &cliaddr, &len)) == -1){
            perror("failed to receive\n");
            exit(EXIT_FAILURE);
        }
        buf[n] = '\0';

        // data processing (calculation)
        string tmp = calculateDijkstra(buf);
        // place data to the buffer for sending 
        strcpy(buf, tmp.c_str());
        
        // send the results to AWS
        if ((sendto(sockfd, (const char *)buf, strlen(buf), 0, (const struct sockaddr *) &cliaddr, len)) == -1) {
            perror("failed to sendto\n");
            exit(EXIT_FAILURE);
        }
        printMSG_sent();
    }
    return 0;
}
