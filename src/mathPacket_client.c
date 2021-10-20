//***************************************************************************
// File name:		httpget_large
// Author:			Cayden Wagner
// Date:				October 5 2021
// Class:				CS 360
// Assignment:	Lab HTTP2
// Purpose:			Practice writing a Large HTTP Request
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

bool isEndOf(char[]);

/****************************************************************************
 Function:		main
 
 Description:	make several HATTP requests on a single connection
 
 Parameters:	int argc: number of command line arguments
							char **argv: the command line arguments
 
 Returned:		EXIT_SUCCESS
****************************************************************************/
int main(int argc, char **argv)
{
  const int MAX_SIZE = 1024, HTTP_PORT = 80;

  char szGetRequest[MAX_SIZE];
  char receiveBuffer[MAX_SIZE];
  char szGetResponse[MAX_SIZE];
  
  int connSocket;

  bool bIsFound = false;

  struct sockaddr_in sConnAddr;

  szGetRequest[0] = '\0';
  szGetResponse[0] = '\0';
  receiveBuffer[0] = '\0';

  connSocket = socket(AF_INET, SOCK_STREAM, 0);
  sConnAddr.sin_family = AF_INET;
  sConnAddr.sin_port = htons(HTTP_PORT);
  inet_aton(argv[1], &sConnAddr.sin_addr);

  if (-1 == connSocket)
  {
     perror("socket failed!\n");
     return -1;
  }
  
  connect(connSocket, (struct sockaddr *) &sConnAddr, sizeof(sConnAddr));

  strncat(szGetRequest, "CALCULATE MATH/1.0\nOperand 1: ", (MAX_SIZE - strlen(szGetRequest)) - 1 );
  strncat(szGetRequest, argv[3], (MAX_SIZE - strlen(szGetRequest)) - 1 );
  strncat(szGetRequest, "\nOperator: ", (MAX_SIZE - strlen(szGetRequest)) - 1 );
  strncat(szGetRequest, argv[4], (MAX_SIZE - strlen(szGetRequest)) - 1 );
  strncat(szGetRequest, "\nOperand2: ", (MAX_SIZE - strlen(szGetRequest)) - 1 );
  strncat(szGetRequest, argv[5], (MAX_SIZE - strlen(szGetRequest)) - 1 );
  strncat(szGetRequest, "\nConnection: Close\n\n", (MAX_SIZE - strlen(szGetRequest)) - 1 );
  
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
  
  close(connSocket);

  return EXIT_SUCCESS; // aspirational
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

  return (NULL == pStr);
}
