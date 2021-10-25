//***************************************************************************
// File name:		mathPacket_errors.c
// Author:			Cayden Wagner
// Date:				October 19 2021
// Class:				CS 360
// Assignment:	mathPacket
// Purpose:			To interact with the mathPacket server by sending invalid 
//              requests and printing the error responses
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

void structureRequest(char[], const char*, const char*, const char*);
void printSeperator();
bool isEndOf(char[]);

/****************************************************************************
 Function:		main
 
 Description:	Sends invalid requests to the math packet server and prints the 
              error responses
 
 Parameters:	none
 
 Returned:		EXIT_SUCCESS
****************************************************************************/
int main(int argc, char **argv)
{
  const char GOOD_OPERAND = '1', BAD_OPERAND = 'p';
  const char GOOD_OPERATOR = '+', BAD_OPERATOR = '$';
  
  char szGetRequest[MAX_SIZE];
  char receiveBuffer[MAX_SIZE];
  char szGetResponse[MAX_SIZE];
  
  int connSocket;

  bool bIsFound = false;

  struct sockaddr_in sConnAddr;

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

  printSeperator();

  memset(szGetRequest, '\0', MAX_SIZE);
  memset(szGetResponse, '\0', MAX_SIZE);
  memset(receiveBuffer, '\0', MAX_SIZE);
  bIsFound = false;

  structureRequest(szGetRequest, &GOOD_OPERAND, &BAD_OPERATOR, &GOOD_OPERAND);
  
  printf("%s\n", szGetRequest);

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

  printSeperator();

  return EXIT_SUCCESS;
}
/****************************************************************************
 Function:		  structureResponse
 
 Description:	  Structures a server request in the correct format
 
 Parameters:	  szGetRequest - an array of chars that holds the request
                operand1     - the first operand in the equation
                operator     - the operator in the equation
                operand2     - the second operand in the equation
 
 Returned:		  none
****************************************************************************/
void structureRequest(char szGetRequest[], const char* operand1, 
                      const char* operator, const char* operand2)
{
  strncat(szGetRequest, "CALCULATE MATH/1.0\nOperand1: ", (MAX_SIZE - strlen(szGetRequest)) - 1 );
  strncat(szGetRequest, operand1, (MAX_SIZE - strlen(szGetRequest)) - 1 );
  strncat(szGetRequest, "\nOperator: ", (MAX_SIZE - strlen(szGetRequest)) - 1 );
  strncat(szGetRequest, operator, (MAX_SIZE - strlen(szGetRequest)) - 1 );
  strncat(szGetRequest, "\nOperand2: ", (MAX_SIZE - strlen(szGetRequest)) - 1 );
  strncat(szGetRequest, operand2, (MAX_SIZE - strlen(szGetRequest)) - 1 );
  strncat(szGetRequest, "\nConnection: Close\n\n", (MAX_SIZE - strlen(szGetRequest)) - 1);
}

void printSeperator() 
{
  printf("====\n");
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