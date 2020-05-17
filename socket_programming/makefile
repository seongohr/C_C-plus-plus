all:
	g++ -o serverA serverA.cpp
	g++ -o serverB serverB.cpp
	g++ -std=c++11 serverC.cpp -o serverC
	g++ -std=c++11 aws.cpp -o aws
	g++ -std=c++11 client.cpp -o client
	

.PHONY:serverA serverB serverC aws

serverA:
	./serverA

serverB:
	./serverB

serverC:
	./serverC

aws: 
	./aws