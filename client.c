// Name of Author(s): Jun Shen
// Course Number and Name: CSE 434,Computer Networks
// Semester: Fall 2015
//Project Part: 1
// Time Spent: 50 minutes

#include <stdio.h>      
#include <stdlib.h>     
#include <string.h>     
#include <unistd.h>     

#include <sys/socket.h> 
#include <arpa/inet.h>  



void myquit(char *errorMessage);  

int main(int argc, char *argv[])
{
    int clntSock;                        
    struct sockaddr_in servAddr; 
    unsigned short servPort;     
    char *servIP;                    
    char *echoString;                
    char sendMsg[200];     
    unsigned int sendMsgLen;   
    int clntNum;   
    int bytesRcvd;   
                                     

    if((argc != 4))
    {
       printf("Usage: %s <hostname> <client number> <server port>\n", argv[0]);
       return -1;
    }

    servIP = argv[1];  
    clntNum = atoi(argv[2]); 
    servPort = atoi(argv[3]);

	clntSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(clntSock < 0)
        myquit("unable to create client socket\n");

    
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(servIP); 
    servAddr.sin_port = htons(servPort); 

    if (connect(clntSock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)
        myquit("unable to connect server\n");
	
	//client number duplication check
	sprintf(sendMsg, "client[%d]", clntNum);
	sendMsgLen = strlen(sendMsg);         
	if(send(clntSock, sendMsg, sendMsgLen, 0) != sendMsgLen)
		myquit("unable to send msg\n");	
	sendMsgLen = recv(clntSock, sendMsg, 200, 0);
	printf("recv from server:%s\n", sendMsg);
	if(sendMsgLen == 0)
	  myquit("duplicate client number...rejected by server\n");
	  
	//main commication loop  
	while(1)
	{
		printf("Please input message:");
		fgets (sendMsg, 200, stdin);
		printf("\n");  
		if ((strlen(sendMsg)>0) && (sendMsg[strlen (sendMsg) - 1] == '\n'))
			sendMsg[strlen (sendMsg) - 1] = '\0';		
		sendMsgLen = strlen(sendMsg);         
		
		if(send(clntSock, sendMsg, sendMsgLen, 0) != sendMsgLen)
			myquit("unable to send msg\n");

		if(sendMsgLen == 0)
		  break;
		
		sendMsgLen = recv(clntSock, sendMsg, 200, 0);
		printf("recv from server:%s\n", sendMsg);
		memset(sendMsg, 0, sizeof(sendMsg));
	}
    close(clntSock);
    return 0;
}

void myquit(char *msg)
{
    printf("%s",msg);
    exit(1);
}

