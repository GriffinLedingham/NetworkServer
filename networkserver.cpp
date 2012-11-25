#include <iostream>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

using namespace std;

//SOCK_STREAM

int main (int numArgs, char** args)
{
	int portNum;
	//int PF_INET, SOCK_STREAM;
    char* root;
	//cout << SOCK_STREAM;
    DIR *myDir;
    struct dirent *dirp;
    FILE *f;
    char* buffer;
    unsigned long fLen;
    struct stat st;


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
    
    std::string fileNameIn = strtok((char*)buf,"\r\n");
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
            //cout << fileName << "\n";
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
    //fseek(f, 0L, SEEK_END);
	//fLen=ftell(f);
	//fseek(f, 0L, SEEK_SET);
    
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

    //cout << fileNameIn << "a\n\n";
    
    //cout << fileName << "\n\n";*/
    
/*  NEED TO CAST THE BUFFER TO A STRING SO THAT WE CAN PASS IT TO LOCATE THE FILE TO SEND
	string s(buf);

cout<< s;
*/

		
}
