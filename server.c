// Name of Author(s): Jun Shen
// Course Number and Name: CSE 434,Computer Networks
// Semester: Fall 2015
//Project Part: 1
// Time Spent: 50 minutes

#include <stdio.h>      
#include <stdlib.h>     
#include <string.h>     
#include <unistd.h>   

#include "sys/socket.h" 
#include "arpa/inet.h"        
#include <semaphore.h>

#include <map>

using namespace std;
  


map<int, char> commClients;


void myquit(char *errorMessage);  
void clientHandler(int clntSocket, int clntNum);
int clientNumCheck(int inClntSkt);


int main(int argc, char *argv[])
{
    int servSkt;
    int clntSkt;
    struct sockaddr_in servAddr; 
    struct sockaddr_in clntAddr; 
    int servPort;     
    int clntAddrLen;       
	int isChildProc;
	int isvalidClnt;
		
    if (argc != 2)     
    {
        printf("\n Usage:  server <Server Port>\n");
        return -1;
    }

    servPort = atoi(argv[1]);  

    servSkt = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (servSkt < 0)
        myquit("unable to create socket!");
      
    memset(&servAddr, 0, sizeof(servAddr));  

    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(servPort);
    
    
    if(bind(servSkt, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)
      myquit("unable to bind...\n");    

    if(listen(servSkt, 10) < 0)
      myquit("unable to listen...\n");    
		
    while(1)
    {
        clntAddrLen = sizeof(clntAddr);
		clntSkt = accept(servSkt, (struct sockaddr *) &clntAddr, (socklen_t *)&clntAddrLen);
        if(clntSkt < 0)
          myquit("accept() failed");

        printf("Handling incoming request %s\n", inet_ntoa(clntAddr.sin_addr));
        
        //centralized client id check
        isvalidClnt = clientNumCheck(clntSkt);
        if(isvalidClnt == -1)
          continue;
          
        isChildProc = fork();
        if(isChildProc == 0)
			clientHandler(clntSkt, isvalidClnt);
    }
}

int clientNumCheck(int inClntSkt)
{
    char recvMsg[200];       
    int recvMsgSize; 	
	int clientNum;
	int isValid = -1;			
	
	recvMsgSize = recv(inClntSkt, recvMsg, 200, 0);
	if(recvMsgSize <= 0)
	{	printf("unable to check recv msg\n");
	}
	else
	{		
		sscanf(recvMsg,"client[%d]", &clientNum);
		
		if(commClients.find(clientNum) == commClients.end())
		{
			 commClients[clientNum] = 1;
			 isValid = clientNum;
		} 
	}
	
	printf("recv client:%d", isValid);
	if(isValid == -1)
	  send(inClntSkt, "", 0, 0);
	else
	  send(inClntSkt, "pass client number check", 25, 0);
	  
	return isValid;  
}

void clientHandler(int inClntSkt, int clntNum)
{
    char recvMsg[200];       
    int recvMsgSize;                   
	
	while(1)
	{
		recvMsgSize = recv(inClntSkt, recvMsg, 200, 0);
		if(recvMsgSize < 0)
			myquit("unable to recv msg\n");
        else if(recvMsgSize == 0)
			break;
		
		printf("received msg[%d]: %s\n", clntNum, recvMsg);  

		if(send(inClntSkt, "msg received", recvMsgSize, 0) != recvMsgSize)
			myquit("send() failed");
	}
	
    close(inClntSkt);
    myquit("close client socket\n");
}

void myquit(char *msg)
{
    printf("%s",msg);
    exit(1);
}

