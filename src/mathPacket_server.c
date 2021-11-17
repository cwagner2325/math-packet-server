//***************************************************************************
// File name:		mathPacket_server.c
// Author:			Cayden Wagner
// Date:				November 162021
// Class:				CS 360
// Assignment:	mathPacket_server
// Purpose:			To provide a server that handles mathPacket requests from the
//              client and responds appropriately
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
void receiveMathPacket(int, char[]);
bool isLastPacket(char[]);
bool isContinuePacket(char[]);

int main(int argc, char **argv)
{
  int socketfd, connSocket, result;

  bool bIsLastPacket = false;

  char szGetRequest[MAX_SIZE], szResponse[MAX_SIZE];

  struct sockaddr_in sAddr, sConnAddr;
  socklen_t addrLen = sizeof(struct sockaddr_in); 

  memset(szGetRequest, '\0', MAX_SIZE);
  memset(szResponse, '\0', MAX_SIZE);

  
  socketfd = socket(AF_INET, SOCK_STREAM, 0);

  if ( -1 == socketfd ) 
  {
    fprintf(stderr, "No socket!\n\n");
    return EXIT_FAILURE;
  }

  sAddr.sin_family = AF_INET;
  sAddr.sin_port = htons(atoi(argv[1]));
  sAddr.sin_addr.s_addr = htonl(INADDR_ANY);

  result = bind(socketfd, &sAddr, sizeof(struct sockaddr_in));

  while ( -1 == result )
  {
    perror("Bind:");
    close (socketfd);
    sleep(30);
    result = bind(socketfd, &sAddr, sizeof(struct sockaddr_in));
  }

  result = listen(socketfd, 1);

  if ( -1 == result ) 
  {
    perror("Listen:");
    close (socketfd);
    return EXIT_FAILURE;
  }

  while(!bIsLastPacket)
  {
    connSocket = accept(socketfd, &sConnAddr, &addrLen);

    if ( -1 == connSocket )
    {
      perror("Accept:");
      close(socketfd);
      return EXIT_FAILURE;
    }

    receiveMathPacket(connSocket, szGetRequest);
    printf("Incoming\n%s", szGetRequest);

    bIsLastPacket = isLastPacket(szGetRequest);

    if (isContinuePacket(szGetRequest))
    {
      printf("Continue\n");
    }
    else
    {
      printf("Calculate\n");
    }

    close(connSocket);
  }

  return EXIT_SUCCESS;
}

/****************************************************************************
 Function:		  isEndOf
 
 Description:	  determines if the string passed in contains \n\n
 
 Parameters:	  response - the response that is parsed for \n\n characters
 
 Returned:		  true if \n\n is found, else false
****************************************************************************/
bool isEndOf(char response[])
{
  char* pStr = NULL;
  pStr = strstr(response, "\n\n");

  return (NULL != pStr);
}
/****************************************************************************
 Function:		  receiveMathPacket
 
 Description:	  receives a  response from the server
 
 Parameters:	  connSocket    - the socket that is configured to the server
                szGetResponse - a char array that contains the received
                                response
 
 Returned:		  none
****************************************************************************/
void receiveMathPacket(int connSocket, char szGetResponse[])
{
  char receiveBuffer[MAX_SIZE], newResponse[MAX_SIZE]; 

  memset(receiveBuffer, '\0', MAX_SIZE);
  memset(newResponse, '\0', MAX_SIZE);
  memset(szGetResponse, '\0', strlen(szGetResponse));

  if (recv(connSocket, &receiveBuffer, MAX_SIZE, 0) <= 0)
  {
    perror("recieve failed!\n");
    return;
  }

  strncat(newResponse, receiveBuffer, (MAX_SIZE - strlen(newResponse) - 1 ));

  while (!isEndOf(newResponse))
  {
    memset(receiveBuffer, '\0', MAX_SIZE);
    recv(connSocket, &receiveBuffer, MAX_SIZE, 0);

    strncat(newResponse, receiveBuffer, (MAX_SIZE - strlen(newResponse)) - 1 );
  }
  newResponse[strlen(newResponse)] = '\0';
  memcpy(szGetResponse, newResponse, strlen(newResponse));
}
/****************************************************************************
 Function:		  isLastPacket
 
 Description:	  determines if the request has specifies a connection close or
                not
 
 Parameters:	  szGetResponse - a char array that contains the received
                                response
 
 Returned:		  true if the packet specifies connection close, else false
****************************************************************************/
bool isLastPacket(char szGetRequest[])
{
  char* pStr, *pEnd, *pIsFound;
  bool bIsLastPacket;
  pStr = strstr(szGetRequest, "Connection");

  while (!isspace(*pStr))
  {
    pStr++;
  }
  pEnd = pStr;
  pEnd++;

  while ('\n' != *pEnd)
  {
    pEnd++;
  }

  *pEnd = '\0';

  pIsFound = strstr(pStr, "Close");

  if (NULL != pIsFound)
  {
    bIsLastPacket = true;
  }
  else 
  {
    bIsLastPacket = false;
  }
  
  *pEnd = '\n';

  return bIsLastPacket;
}
/****************************************************************************
 Function:		  isContinuePacket
 
 Description:	  determines if the request is a CALCULATE protocol
 
 Parameters:	  szGetResponse - a char array that contains the received
                                response
 
 Returned:		  
****************************************************************************/
bool isContinuePacket(char szGetRequest[])
{
  char* pStr;
  pStr = strstr(szGetRequest, "CONTINUE");

  return (NULL != pStr);
}