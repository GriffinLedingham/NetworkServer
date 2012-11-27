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
    
    void *buf = calloc(5000, sizeof(char));

    
	int sockTemp = accept(sockfd, (sockaddr*)&ip4addr, &len);
	recv(sockTemp, buf, 5000, 0);
    
    struct sockaddr_in hostAddr;
    getpeername(sockTemp, (sockaddr *)&hostAddr, &len);
    
    std::string fileNameIn;
    
    fileNameIn= strtok((char*)buf,"\r\n");
    cout << (char*)fileNameIn;
    cout << endl;
    std::string fullInput = fileNameIn;
    
    std::string temp;
    temp = (std::string)strtok((char*)fileNameIn.c_str()," ");
    bool emptyFlag= false;
    
    
    for(int i =0;i<2;i++)
    {
        if(temp.empty())
        {
            emptyFlag = true;
            break;
        }
        
        parsedInput[i] = temp;
        
        temp = strtok(NULL, " ");
        /*if(temp.find("/"))
        {
            temp = temp.substr(1,temp.length()-1);
        }*/
        cout << parsedInput[i] << "\n";
        //cout << parsedInput[i] << "\n";
        
    }
    if(!emptyFlag)
    {
        parsedInput[2] = temp;
        cout << parsedInput[2] << "\n";
    }
    //cout << parsedInput[2] << "\n";
    //temp
    fileNameIn = parsedInput[1];
    
    if(parsedInput[0] != "GET" || parsedInput[2] != "HTTP/1.0")
    {
        statusMessage = "HTTP/1.0 400 Bad Request; ";
        cout << seq << " ";
        
        printTime();
        
        cout << " " << inet_ntoa(hostAddr.sin_addr) << ":" << ntohs(hostAddr.sin_port) << " " << fullInput << "; "<< statusMessage << "\n";
        close(sockTemp);
        return;
        
    }
    
    
    
    //fileNameIn = strtok(fileNameIn.c_str(),"GET ");
    //std::string fullInput = fileNameIn;
    
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
            fnf = false;
        }
    }
    requestedFile.insert(0,"/");
    requestedFile.insert(0,root);
    f = fopen(requestedFile.c_str(),"rb");
    
    

    if(fnf)
    {
        //cout<<"File = null pointer\n";
        statusMessage = "HTTP/1.0 404 Not Found; ";
        cout << seq << " ";
        
        printTime();
        
        cout << " " << inet_ntoa(hostAddr.sin_addr) << ":" << ntohs(hostAddr.sin_port) << " " << fullInput << "; "<< statusMessage << "\n";
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
    //cout<<"Buffer has been written with length " << fLen << " \n";
	fclose(f);
    
    
    //cout << "Peer Name: " << inet_ntoa(hostAddr.sin_addr) << "\n";
    //cout << "TEST TEST";
    cout << seq << " ";

    printTime();
    
    cout << " " << inet_ntoa(hostAddr.sin_addr) << ":" << ntohs(hostAddr.sin_port) << " " << parsedInput[0] << " " << parsedInput[1] << " " << parsedInput[2] << ";"<< statusMessage << " "<< root << parsedInput[1];
    
    cout << endl;
    
    send(sockTemp,(char*)buffer,fLen+1,0);
    
    
    //std::string returnAddr;
    //cout << hostAddr.sin_port;
    //printf("%u", hostAddr.sin_port);
    //returnAddr.insert(0, portString);
    //char* portOut;
    //sprintf(portOut,"%d",ntohs(hostAddr.sin_port));
    //returnAddr.insert(0, parsedInput[2]);
    //returnAddr.insert(0, " ");
    //returnAddr.insert(0, portOut);
    //returnAddr.insert(0, ":");
    //returnAddr.insert(0, inet_ntoa(hostAddr.sin_addr));
    
    close(sockTemp);
    
    //return returnAddr;
}

std::string connection(char* root, int portNum)
{
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
    
    
    while(true)
    {
        listen(sockfd, SOMAXCONN);
    
        reqWait(sockfd, ip4addr, root, seq);
        seq++;
        
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
    
    //std::string peerAddr = connection(root, portNum);
    
    connection(root,portNum);
    
       // cout << seq << " ";
        
       //cout << " " << peerAddr << "\n";
    //}
}


