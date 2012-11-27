#include <iostream>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <stdio.h>

using namespace std;

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

void reqWait(int sockfd, sockaddr_in ip4addr, char* root, int seq)
{
    DIR *myDir;
    struct dirent *dirp;
    FILE *f;
    char* buffer;
    unsigned long fLen;
    struct stat st;
    std::string parsedInput[3];
    socklen_t len;
std:string statusMessage = "HTTP/1.0 200 OK; ";
    bool fnf = true;
    bool weTookOutASlash = false;
    bool baseCase = false;
    int badFlag= 0;
    int count = 0;
    
    void *bufIn = calloc(5000, sizeof(char));
    
    
	int sockTemp = accept(sockfd, (sockaddr*)&ip4addr, &len);
	recv(sockTemp, bufIn, 5000, 0);
    
    struct sockaddr_in hostAddr;
    getpeername(sockTemp, (sockaddr *)&hostAddr, &len);
    
    std::string fileNameIn;
    char* buf = (char*)bufIn;
    std::string slash (" / ");
    
    if(buf[3] == ' ' && buf[4] == '/' && buf[5] == ' ')
    {
        baseCase = true;
        buf[4] = '/';
        weTookOutASlash= false;
    }
    else if(buf[4] == '/')
    {
        buf[4] = ' ';
        weTookOutASlash = true;
        
    }
    
    fileNameIn= strtok((char*)buf,"\r\n");
    
    if(fileNameIn.find("..") != string::npos)
    {
        statusMessage = "HTTP/1.0 400 Bad Request; ";
        cout << seq << " ";
        
        printTime();
        
        cout << " " << inet_ntoa(hostAddr.sin_addr) << ":" << ntohs(hostAddr.sin_port) <<"; "<< statusMessage << "\n";
        close(sockTemp);
        return;
    }
    
    std::string fullInput = fileNameIn;
    
    char* temp;
    temp =strtok((char*)fileNameIn.c_str()," ");
    
    while(temp != NULL && count<3)
    {
        parsedInput[count] = temp;
        temp = strtok(NULL, " ");
        count++;
    }
    if(temp !=NULL)
    {
        statusMessage = "HTTP/1.0 400 Bad Request; ";
        cout << seq << " ";
        
        printTime();
        
        cout << " " << inet_ntoa(hostAddr.sin_addr) << ":" << ntohs(hostAddr.sin_port) <<"; "<< statusMessage << "\n";
        close(sockTemp);
        return;
    }
    
    fileNameIn = parsedInput[1];
	char* get = (char*)parsedInput[0].c_str();
	char* http = (char*)parsedInput[2].c_str();
    
    if(strcasecmp(get,"get")!=0 || strcasecmp(http,"http/1.0")!=0)
    {
        statusMessage = "HTTP/1.0 400 Bad Request; ";
        cout << seq << " ";
        
        printTime();
        
        cout << " " << inet_ntoa(hostAddr.sin_addr) << ":" << ntohs(hostAddr.sin_port) <<"; "<< statusMessage << "\n";
        close(sockTemp);
        return;
        
    }
    
    if(!fileNameIn.empty())
    {
        myDir = opendir(root);
    }
    
    std::string fileName = "";
    std::string requestedFile = "";
    
    if(baseCase)
        fileNameIn = "index.html";
    
    while ((dirp = readdir(myDir)) != NULL)
    {
        fileName = (char*)dirp->d_name;
        if(fileName == fileNameIn)
        {
            requestedFile = fileName;
            fnf = false;
        }
    }
    
    
    requestedFile.insert(0,"/");
    
    
    requestedFile.insert(0,root);
    
    fileNameIn.insert(0,requestedFile);
    
    cout << requestedFile<< " filenameIN "<< fileNameIn<< endl;
    
    f = fopen(requestedFile.c_str(),"rb");
    
    
    
    if(fnf)
    {
        
        statusMessage = "HTTP/1.0 404 Not Found; ";
        cout << seq << " ";
        
        printTime();
        
        cout << " " << inet_ntoa(hostAddr.sin_addr) << ":" << ntohs(hostAddr.sin_port) << " " <<parsedInput[0]<<" ";
        
        if(weTookOutASlash)
            cout << "/";
        
        cout<<parsedInput[1] << " " << parsedInput[2] << "; "<< statusMessage <<fileNameIn<<"\n";
        
        close(sockTemp);
        return;
        
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
	fclose(f);
    cout << seq << " ";
    printTime();
    
    cout << " " << inet_ntoa(hostAddr.sin_addr) << ":" << ntohs(hostAddr.sin_port) << " " << parsedInput[0]<< " ";
    
    if(weTookOutASlash)
        cout << "/";
    
    cout<<parsedInput[1] << " " << parsedInput[2] << "; "<< statusMessage <<requestedFile<< "\n";
    
    send(sockTemp,(char*)buffer,fLen+1,0);
    
    close(sockTemp);
    
}

std::string connection(char* root, int portNum)
{
    fd_set listenfds;
    char buffer[1000];
    int seq = 0;
    int opt = 1;
    struct sockaddr_in ip4addr;
	ip4addr.sin_family = AF_INET;
	ip4addr.sin_port = htons(portNum);
	ip4addr.sin_addr.s_addr = INADDR_ANY;
    
	int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    setsockopt(sockfd, IPPROTO_TCP, SO_REUSEADDR, (char*)&opt,sizeof(opt));
	bind(sockfd, (sockaddr*)&ip4addr, sizeof(sockaddr_in));
    
    cout << "sws is running on TCP port " << portNum << " and serving " << root << "\n";
    cout << "press ‘q’ to quit ...\n";
    listen(sockfd, SOMAXCONN);
    
    while(true)
    {
        FD_ZERO(&listenfds);
        FD_SET(sockfd,&listenfds);
        FD_SET(0,&listenfds);
        
        select(sockfd+1, &listenfds, 0, 0, 0);
        
        if(FD_ISSET(0, &listenfds))
        {
            fgets(buffer,1000,stdin);
            if(buffer[0] =='q')
            {
                exit(0);
            }
        }
        
        if (FD_ISSET(sockfd, &listenfds)) {
            reqWait(sockfd, ip4addr, root, seq);
            seq++;
        }
    }
}

int main (int numArgs, char** args)
{
	int portNum;
    char* root;
    
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
    
    connection(root,portNum);
    
}