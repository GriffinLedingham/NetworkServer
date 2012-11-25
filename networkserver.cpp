#include <iostream>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <sys/types.h>


using namespace std;

//SOCK_STREAM

int printTime()
{
    time_t timer;
    
    char buffer[25];
    struct tm* tm_info;
    time(&timer);
    
    tm_info = localtime(&timer);
    strftime(buffer, 25, "%Y %b %d %H:%M:%S", tm_info);
    cout << buffer;
}

std::string connection(char* root, int portNum)
{
    DIR *myDir;
    struct dirent *dirp;
    FILE *f;
    char* buffer;
    unsigned long fLen;
    struct stat st;
    
    struct sockaddr_in ip4addr;
    
	ip4addr.sin_family = AF_INET;
	ip4addr.sin_port = htons(portNum);
	ip4addr.sin_addr.s_addr = INADDR_ANY;
	
	int sockfd = socket(PF_INET, SOCK_STREAM, 0);
	bind(sockfd, (sockaddr*)&ip4addr, sizeof(sockaddr_in));
    
    
    cout << "sws is running on TCP port " << portNum << " and serving " << root << "\n";
    cout << "press ‘q’ to quit ...\n";
    
	void *buf = calloc(5000, sizeof(char));
	listen(sockfd, SOMAXCONN);
	socklen_t len;
	int sockTemp = accept(sockfd, (sockaddr*)&ip4addr, &len);
	recv(sockTemp, buf, 5000, 0);
    
    
    
    std::string fileNameIn;
    fileNameIn= strtok((char*)buf,"\r\n");
    
    
    //fileNameIn = strtok(fileNameIn.c_str(),"GET ");
    std::string fullInput = fileNameIn;
    
    //THIS IS WHATS STRIPPING SPACES AND DOTS
    
    //char* tempFileIn = strtok((char*)fileNameIn.c_str(),"GET ");
    //tempFileIn = strtok(tempFileIn,"HTTP/1.0");
    //fileNameIn = (std::string)tempFileIn;
    
    if(!fileNameIn.empty())
    {
        myDir = opendir(root);
    }
    
    std::string fileName = "";
    std::string requestedFile = "";
    while ((dirp = readdir(myDir)) != NULL)
    {
        fileName = (char*)dirp->d_name;
        if(fileName == fileNameIn)
        {
            requestedFile = fileName;
        }
    }
    requestedFile.insert(0,"/");
    requestedFile.insert(0,root);
    f = fopen(requestedFile.c_str(),"rb");
    if(f==NULL)
    {
        cout<<"File = null pointer\n";
    }
    
    stat(requestedFile.c_str(),&st);
    fLen = st.st_size;
    
	buffer=(char*)malloc(fLen+1);
    
    if (!buffer)
	{
		fprintf(stderr, "Memory error!");
        fclose(f);
		exit(1);
	}
    
    fread(buffer, fLen, 1, f);
    cout<<"Buffer has been written with length " << fLen << " \n";
	fclose(f);
    
    struct sockaddr_in hostAddr;
    getpeername(sockTemp, (sockaddr *)&hostAddr, &len);
    
    cout << "Peer Name: " << inet_ntoa(hostAddr.sin_addr) << "\n";
    
    send(sockTemp,(char*)buffer,fLen+1,0);
    
    std::string returnAddr;
    //cout << hostAddr.sin_port;
    //printf("%u", hostAddr.sin_port);
    //returnAddr.insert(0, portString);
    char* portOut;
    sprintf(portOut,"%d",ntohs(hostAddr.sin_port));
    returnAddr.insert(0, fullInput);
    returnAddr.insert(0, " ");
    returnAddr.insert(0, portOut);
    returnAddr.insert(0, ":");
    returnAddr.insert(0, inet_ntoa(hostAddr.sin_addr));
    
    return returnAddr;
}

int main (int numArgs, char** args)
{
	int portNum;
    char* root;
    int seq = 0;

	if(numArgs == 3)
	{
		portNum = atoi(args[1]);
		root = (char*)malloc(sizeof(char)*sizeof(strlen(args[2]+1)));
        root = args[2];
	}
	else
	{
		cout << "Argument Error, arguments need to be in the form of <port> <directory>.\n";
		return 0;
	}
    
    std::string peerAddr = connection(root, portNum);
    if(!peerAddr.empty())
    {
        seq++;
        cout << seq << " ";
        printTime();
        cout << " " << peerAddr << "\n";
    }	
}


