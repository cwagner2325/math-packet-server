//***************************************************************************
// File name:		
// Author:			Cayden Wagner
// Date:				October 19 2021
// Class:				CS 360
// Assignment:	
// Purpose:			
//***************************************************************************

#define _GNU_SOURCE

#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

const int MAX_SIZE = 1024;

bool isEndOf(char[]);
void structureRequest(char[], char*, char*, char*);

/****************************************************************************
 Function:		main
 
 Description:	
 
 Parameters:	int argc: number of command line arguments
							char **argv: the command line arguments
 
 Returned:		EXIT_SUCCESS
****************************************************************************/
int main(int argc, char **argv)
{
  const int MAX_SIZE = 1024;

  char szGetRequest[MAX_SIZE];
  char receiveBuffer[MAX_SIZE];
  char szGetResponse[MAX_SIZE];
  
  int connSocket;

  bool bIsFound = false;

  struct sockaddr_in sConnAddr;

  szGetRequest[0] = '\0';
  szGetResponse[0] = '\0';
  receiveBuffer[0] = '\0';

  structureRequest(szGetRequest, argv[3], argv[4], argv[5]);

  connSocket = socket(AF_INET, SOCK_STREAM, 0);
  sConnAddr.sin_family = AF_INET;
  sConnAddr.sin_port = htons(atoi(argv[2]));
  inet_aton(argv[1], &sConnAddr.sin_addr);

  if (-1 == connSocket)
  {
     perror("socket failed!\n");
     return -1;
  }
  
  connect(connSocket, (struct sockaddr *) &sConnAddr, sizeof(sConnAddr));
  
  printf(">|%s<|\n\n", szGetRequest);

  send(connSocket, szGetRequest, strlen(szGetRequest), 0);

  if (recv(connSocket, &receiveBuffer, sizeof(szGetResponse), 0) <= 0)
  {
    perror("recieve failed!\n");
    return -1;
  }

  strncat(szGetResponse, receiveBuffer, (MAX_SIZE - strlen(szGetRequest)) - 1 );

  bIsFound = isEndOf(szGetResponse);

  while (!bIsFound)
  {
    memset(receiveBuffer, '\0', sizeof(szGetResponse));
    recv(connSocket, &receiveBuffer, sizeof(szGetResponse), 0);

    strncat(szGetResponse, receiveBuffer, (MAX_SIZE - strlen(szGetRequest)) - 1 );

    bIsFound = isEndOf(szGetResponse);
  }

  printf("%s", szGetResponse);
  
  close(connSocket);

  return EXIT_SUCCESS; 
}
/****************************************************************************
 Function:		  
 
 Description:	  
 
 Parameters:	  
 
 Returned:		  
****************************************************************************/
bool isEndOf(char response[])
{
  char* pStr = NULL;
  pStr = strstr(response, "\n\n");

  return (NULL != pStr);
}
/****************************************************************************
 Function:		  
 
 Description:	  
 
 Parameters:	  
 
 Returned:		  
****************************************************************************/
void structureRequest(char szGetRequest[], char* operand1, char* operator, char* operand2)
{
  strncat(szGetRequest, "CALCULATE MATH/1.0\nOperand1: ", (MAX_SIZE - strlen(szGetRequest)) - 1 );
  strncat(szGetRequest, operand1, (MAX_SIZE - strlen(szGetRequest)) - 1 );
  strncat(szGetRequest, "\nOperator: ", (MAX_SIZE - strlen(szGetRequest)) - 1 );
  strncat(szGetRequest, operator, (MAX_SIZE - strlen(szGetRequest)) - 1 );
  strncat(szGetRequest, "\nOperand2: ", (MAX_SIZE - strlen(szGetRequest)) - 1 );
  strncat(szGetRequest, operand2, (MAX_SIZE - strlen(szGetRequest)) - 1 );
  strncat(szGetRequest, "\nConnection: Close\n\n", (MAX_SIZE - strlen(szGetRequest)) - 1);
}