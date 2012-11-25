#include <iostream>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>

using namespace std;

//SOCK_STREAM

int main (int numArgs, char** args)
{
	int portNum;
	//int PF_INET, SOCK_STREAM;
	char* root;
	//cout << SOCK_STREAM;

	if(numArgs == 3)
	{
		portNum = atoi(args[1]);
		root = (char*)malloc(sizeof(char)*sizeof(strlen(args[2]+1)));
	}
	else
	{
		cout << "Argument Error, program ending.\n";
		return 0;
	}

	struct sockaddr_in ip4addr;

	ip4addr.sin_family = AF_INET;
	ip4addr.sin_port = htons(portNum);
	ip4addr.sin_addr.s_addr = INADDR_ANY;
	
	int sockfd = socket(PF_INET, SOCK_STREAM, 0);
	bind(sockfd, (sockaddr*)&ip4addr, sizeof(sockaddr_in));


	void *buf = calloc(5000, sizeof(char));
	listen(sockfd, SOMAXCONN);
	socklen_t len;
	int sockTemp = accept(sockfd, (sockaddr*)&ip4addr, &len);
	
	recv(sockTemp, buf, 5000, 0);

	cout << (char*)buf <<endl;
    
	
/*  NEED TO CAST THE BUFFER TO A STRING SO THAT WE CAN PASS IT TO LOCATE THE FILE TO SEND
	string s(buf);

cout<< s;
*/

		
}
