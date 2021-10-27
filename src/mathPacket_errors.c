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

const char NEW_LINE = '\n';
const int MAX_SIZE = 1024;

void structureRequest(char[], const char*, const char*, const char*);
void receiveMathPacket(int, char[]);
void printSeperator();
bool isEndOf(char[]);
void errorTestResponse(char[]);
void structureBadRequest(char[], const char*, const char*, const char*);
void structureTimeoutRequest(char[], const char*, const char*, const char*);

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
  char szGetResponse[MAX_SIZE];
  
  int connSocket;

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
  printf("Expected Code: 200\n\n");

  memset(szGetResponse, '\0', MAX_SIZE);
  memset(szGetRequest, '\0', MAX_SIZE);

  structureRequest(szGetRequest, &GOOD_OPERAND, &BAD_OPERATOR, &GOOD_OPERAND);
  printf("%s\n", szGetRequest);

  send(connSocket, szGetRequest, strlen(szGetRequest), 0);

  receiveMathPacket(connSocket, szGetResponse);
  errorTestResponse(szGetResponse);
  
  printf("%s", szGetResponse);

  close(connSocket);

  printSeperator();
  printf("\n");

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
  printf("Expected Code: 201\n\n");

  memset(szGetResponse, '\0', MAX_SIZE);
  memset(szGetRequest, '\0', MAX_SIZE);

  structureRequest(szGetRequest, &BAD_OPERAND, &GOOD_OPERATOR, &GOOD_OPERAND);
  printf("%s\n", szGetRequest);

  send(connSocket, szGetRequest, strlen(szGetRequest), 0);

  receiveMathPacket(connSocket, szGetResponse);
  errorTestResponse(szGetResponse);
  
  printf("%s", szGetResponse);

  close(connSocket);

  printSeperator();
  printf("\n");

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
  printf("Expected Code: 202\n\n");

  memset(szGetResponse, '\0', MAX_SIZE);
  memset(szGetRequest, '\0', MAX_SIZE);

  structureRequest(szGetRequest, &GOOD_OPERAND, &GOOD_OPERATOR, &BAD_OPERAND);
  printf("%s\n", szGetRequest);

  send(connSocket, szGetRequest, strlen(szGetRequest), 0);

  receiveMathPacket(connSocket, szGetResponse);
  errorTestResponse(szGetResponse);
  
  printf("%s", szGetResponse);

  close(connSocket);

  printSeperator();
  printf("\n");

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
  printf("Expected Code: 300\n\n");

  memset(szGetResponse, '\0', MAX_SIZE);
  memset(szGetRequest, '\0', MAX_SIZE);

  structureBadRequest(szGetRequest, &GOOD_OPERAND, &GOOD_OPERATOR, 
                      &GOOD_OPERAND);
  printf("%s\n", szGetRequest);

  send(connSocket, szGetRequest, strlen(szGetRequest), 0); 

  receiveMathPacket(connSocket, szGetResponse);
  errorTestResponse(szGetResponse);
  
  printf("%s", szGetResponse);

  close(connSocket);

  printSeperator();
  printf("\n");

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
  printf("Expected Code: 400\n\n");

  memset(szGetResponse, '\0', MAX_SIZE);
  memset(szGetRequest, '\0', MAX_SIZE);

  structureTimeoutRequest(szGetRequest, &GOOD_OPERAND, &GOOD_OPERATOR, 
                          &GOOD_OPERAND);
  printf("%s\n", szGetRequest);

  send(connSocket, szGetRequest, strlen(szGetRequest), 0); 

  receiveMathPacket(connSocket, szGetResponse);
  errorTestResponse(szGetResponse);
  
  printf("%s", szGetResponse);

  close(connSocket);

  printSeperator();

  return EXIT_SUCCESS;
}
/****************************************************************************
 Function:		  structureRequest
 
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

  strncat(szGetRequest, "CALCULATE MATH/1.0\nOperand1: ", 
         (MAX_SIZE - strlen(szGetRequest)) - 1 );
  szGetRequest[strlen(szGetRequest)] = *operand1;
  strncat(szGetRequest, "\nOperator: ", (MAX_SIZE - strlen(szGetRequest)) - 1 );
  szGetRequest[strlen(szGetRequest)] = *operator;
  strncat(szGetRequest, "\nOperand2: ", (MAX_SIZE - strlen(szGetRequest)) - 1 );
  szGetRequest[strlen(szGetRequest)] = *operand2;
  strncat(szGetRequest, "\nConnection: Close\n\n", 
         (MAX_SIZE - strlen(szGetRequest)) - 1);
  szGetRequest[strlen(szGetRequest)] = '\0';
}
/****************************************************************************
 Function:		  printSeperator
 
 Description:	  prints 4 = signs and a newline to seperate test cases
 
 Parameters:	  none
 
 Returned:		  none
****************************************************************************/
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
/****************************************************************************
 Function:		  receiveMathPacket
 
 Description:	  receives a  response from the server
 
 Parameters:	  connSocket - the socket that is configured to the server
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
 Function:		  errorTestResponse
 
 Description:	  Verifies the response's error code, if it is not 100 OK, then
                the response is formatted into the appropriate error coe
 
 Parameters:	  respose - an array of chars that is verified and reformatted
                          if necessary
 
 Returned:		  none
****************************************************************************/
void errorTestResponse(char response[])
{
  const int OK_CODE = 100;
  char* pStr = NULL;
  char* pEnd = NULL;
  char newResponse[MAX_SIZE];

  memset(newResponse, '\0', MAX_SIZE);

  pStr = strstr(response, "MATH");

  while(!isspace(*pStr))
  {
    pStr++;
  }

  pEnd = pStr;
  pEnd++;

  while(!isspace(*pEnd))
  {
    pEnd++;
  }

  *pEnd = '\0';

  if (OK_CODE != atoi(pStr))
  {
    strncat(newResponse, "Response Code:", 
           (MAX_SIZE - strlen(newResponse)) - 1 );
    strncat(newResponse, pStr, (MAX_SIZE - strlen(newResponse)) - 1 );
    strncat(newResponse, "\nResponse Message:", 
           (MAX_SIZE - strlen(newResponse)) - 1 );

    *pEnd = ' ';

    pStr = pEnd;

    while('\n' != *pEnd)
    {
      pEnd++;
    }

    *pEnd = '\0';
    strncat(newResponse, pStr, (MAX_SIZE - strlen(newResponse)) - 1 );
    newResponse[strlen(newResponse)] = '\n';

    memset(response, '\0', MAX_SIZE);
    memcpy(response, newResponse, strlen(newResponse));
    response[strlen(response)] = '\0';
  }
}
/****************************************************************************
 Function:		  structureTimeoutRequest
 
 Description:	  Structures a server request in the incorrect format
 
 Parameters:	  szGetRequest - an array of chars that holds the request
                operand1     - the first operand in the equation
                operator     - the operator in the equation
                operand2     - the second operand in the equation
 
 Returned:		  none
****************************************************************************/
void structureTimeoutRequest(char szGetRequest[], const char* operand1, 
                      const char* operator, const char* operand2)
{
  strncat(szGetRequest, "CALCULATE MATH/1.0\nOperand1: ", 
         (MAX_SIZE - strlen(szGetRequest)) - 1 );
  szGetRequest[strlen(szGetRequest)] = *operand1;
  strncat(szGetRequest, "\nOperator: ", (MAX_SIZE - strlen(szGetRequest)) - 1 );
  szGetRequest[strlen(szGetRequest)] = *operator;
  strncat(szGetRequest, "\nOperand2: ", (MAX_SIZE - strlen(szGetRequest)) - 1 );
  szGetRequest[strlen(szGetRequest)] = *operand2;
  strncat(szGetRequest, "\nConnection: Close\n", 
         (MAX_SIZE - strlen(szGetRequest)) - 1);
}
/****************************************************************************
 Function:		  structureBadRequest
 
 Description:	  Structures a server request in the incorrect format
 
 Parameters:	  szGetRequest - an array of chars that holds the request
                operand1     - the first operand in the equation
                operator     - the operator in the equation
                operand2     - the second operand in the equation
 
 Returned:		  none
****************************************************************************/
void structureBadRequest(char szGetRequest[], const char* operand1, 
                      const char* operator, const char* operand2)
{
  strncat(szGetRequest, "CALCULATE MATH/1.0\nOperand1: ", 
         (MAX_SIZE - strlen(szGetRequest)) - 1 );
  szGetRequest[strlen(szGetRequest)] = *operand1;
  strncat(szGetRequest, "\nOperator: ", (MAX_SIZE - strlen(szGetRequest)) - 1 );
  szGetRequest[strlen(szGetRequest)] = *operator;
  strncat(szGetRequest, "\nOpera: ", (MAX_SIZE - strlen(szGetRequest)) - 1 );
  szGetRequest[strlen(szGetRequest)] = *operand2;
  strncat(szGetRequest, "\nConnection: Close\n\n", 
         (MAX_SIZE - strlen(szGetRequest)) - 1);
}