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
#define NUM_OPERANDS 5

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
bool isCurrentVersion (char[]);
bool isContinuePacket(char[]);
void structureBadVersionPacket(char[]);
bool isInvalidOperator(char[]);
void structureBadOperatorPacket(char[]);
int calculateResult(int, char, int);
int getOperand(char[], char[]);
char getField(char[], char[]);
void structureResponse(char[], int, bool, bool);

int main(int argc, char **argv)
{
  int socketfd, connSocket, result, calculation = 0;
  bool bIsLastPacket = false;
  char szGetRequest[MAX_SIZE], szResponse[MAX_SIZE];
  char operand1, operand2, operator;

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

  connSocket = accept(socketfd, &sConnAddr, &addrLen);

  if ( -1 == connSocket )
  {
    perror("Accept:");
    close(socketfd);
    return EXIT_FAILURE;
  }

  while(!bIsLastPacket)
  {

    receiveMathPacket(connSocket, szGetRequest);
    printf("Incoming\n%s", szGetRequest);

    if (isCurrentVersion(szGetRequest))
    {
      if (isInvalidOperator(szGetRequest))
      {
        bIsLastPacket = true;
        structureBadOperatorPacket(szResponse);
        send(connSocket, szResponse, strlen(szResponse), 0);
      }
      else 
      {
        bIsLastPacket = isLastPacket(szGetRequest);
        if (!isContinuePacket(szGetRequest))
        {
          operand1 = getOperand(szGetRequest, "Operand1");
          operator = getField(szGetRequest, "Operator");
          operand2 = getOperand(szGetRequest, "Operand2");
          calculation = calculateResult(operand1, operator, operand2);
        }
        else 
        {
          operator = getField(szGetRequest, "Operator");
          operand2 = getOperand(szGetRequest, "Operand2");
          calculation = calculateResult(calculation, operator, operand2);
        }
        structureResponse(szResponse, calculation, bIsLastPacket, false);
        printf("Outgoing\n%s", szResponse);
        send(connSocket, szResponse, strlen(szResponse), 0);
      } 
    }
    else
    {
      bIsLastPacket = true;
      structureBadVersionPacket(szResponse);
      send(connSocket, szResponse, strlen(szResponse), 0);
    }
  }

  close(connSocket);

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
  char *pIsFound;
  bool bIsLastPacket;

  pIsFound = strstr(szGetRequest, "Close");

  if (NULL != pIsFound)
  {
    bIsLastPacket = true;
  }
  else 
  {
    bIsLastPacket = false;
  }

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
/****************************************************************************
 Function:		  isCurrentVersion
 
 Description:	  checks if the received packet is from the current server 1.1
                version
 
 Parameters:	  szGetRequest - an array of chars containing the request packet
 
 Returned:		  true if the packet contains version 1.1, else false
****************************************************************************/
bool isCurrentVersion (char szGetRequest[])
{
  bool bCurr;
  char* pStr;
  pStr = strstr(szGetRequest, "MATH/1.1");
  if (NULL != pStr)
  {
    bCurr = true;
  }
  else 
  {
    bCurr = false;
  }
  return (bCurr);
}
/****************************************************************************
 Function:		  structureBadVersionPacket
 
 Description:	  structures the error response for a bad version packet
 
 Parameters:	  response - an array of chars that is filled with the error 
                           response
 
 Returned:		  none
****************************************************************************/
void structureBadVersionPacket(char response[])
{
  memset(response, '\0', MAX_SIZE);
  strncat(response, 
         "MATH/1.1 500 OlderCodeDoesNotCheckout\nConnection: Close\n\n", 
         (MAX_SIZE - strlen(response)) - 1 );
}
/****************************************************************************
 Function:		  isInvalidOperator
 
 Description:	  checks if the request packet contains one of the valid operators
 
 Parameters:	  request - an array of chars containing the request
 
 Returned:		  true if the packet has an invalid operator, else false
****************************************************************************/
bool isInvalidOperator(char request[])
{
  const char OPERANDS[NUM_OPERANDS] = "x+-/%";
  char* pStr;
  bool bError = false;

  pStr = strstr(request, "Operator");
  
  if (NULL != pStr)
  {
    while (!isspace(*pStr))
    {
      pStr++;
    }

    pStr++;

    for (int i = 0; i < NUM_OPERANDS; i++)
    {
      if (*pStr == OPERANDS[i])
      {
        bError = true;
      }
    }
  }

return !bError;
}
/****************************************************************************
 Function:		  structureBadOperatorPacket
 
 Description:	  strctures the error response for a bad operator
 
 Parameters:	  response - an array of chars that is filled with the error 
                           response
 
 Returned:		  none
****************************************************************************/
void structureBadOperatorPacket(char response[])
{
  memset(response, '\0', MAX_SIZE);
  strncat(response, 
         "MATH/1.1 200 BAD_OPERATOR\nConnection: Close\n\n", 
         (MAX_SIZE - strlen(response)) - 1 );
}
/****************************************************************************
 Function:		  
 
 Description:	  
 
 Parameters:	 
 
 Returned:		 
****************************************************************************/
int calculateResult(int operand1, char operator, int operand2)
{
  int result = -1;

  if ('+' == operator)
  {
    result = operand1 + operand2;
  }
  else if ('-' == operator)
  {
    result = operand1 - operand2;
  }
  else if ('%' == operator)
  {
    result = operand1 % operand2;
  }
  else if ('x' == operator)
  {
    result = operand1 * operand2;
  }
  else if ('/' == operator)
  {
    result = operand1 / operand2;
  }
  return result;
}
/****************************************************************************
 Function:		  
 
 Description:	  
 
 Parameters:	 
 
 Returned:		 
****************************************************************************/
char getField(char request[], char field[])
{
  char* pStr;

  pStr = strstr(request, field);

  while (!isspace(*pStr))
  {
    pStr++;
  }
  pStr++;

  return *pStr;
}
/****************************************************************************
 Function:		  
 
 Description:	  
 
 Parameters:	  szGetRequest - an array of chars that holds the request
                operand1     - the first operand in the equation
                operator     - the operator in the equation
                operand2     - the second operand in the equation 
 
 Returned:		  
****************************************************************************/
void structureResponse(char szResponse[], int result, bool bRounded, bool 
                             bConnClose)
{
  char integerString[32];
  memset(integerString, '\0', 32);

  sprintf(integerString, "%d", result);

  memset(szResponse, '\0', MAX_SIZE);
  strncat(szResponse, "MATH/1.1 100 OK\nResult: ", 
          (MAX_SIZE - strlen(szResponse)) - 1 );
  strncat(szResponse, integerString, (MAX_SIZE - strlen(szResponse)) - 1 );
  strncat(szResponse, "\nRounding: ", (MAX_SIZE - strlen(szResponse)) - 1 );
  if(bRounded)
  {
    strncat(szResponse, "True\n", (MAX_SIZE - strlen(szResponse)) - 1 );
  }
  else
  {
    strncat(szResponse, "False\n", (MAX_SIZE - strlen(szResponse)) - 1 );
  }
  strncat(szResponse, "Overflow: False\nX-Server-Version: 1.1.0\nConnection: ",
         (MAX_SIZE - strlen(szResponse)) - 1 );
  if(bConnClose)
  {
    strncat(szResponse, "Close\n\n", (MAX_SIZE - strlen(szResponse)) - 1 );
  }
  else
  {
    strncat(szResponse, "Keep-Alive\n\n", (MAX_SIZE - strlen(szResponse)) - 1 );
  }
}

int getOperand(char request[], char field[])
{
  {
  int temp;
  char* pStr, *pEnd;

  pStr = strstr(request, field);

  while (!isspace(*pStr))
  {
    pStr++;
  }
  pStr++;

  pEnd = pStr;
  while('\n' != *pEnd)
  {
    pEnd++;
  }

  *pEnd = '\0';
  temp = atoi(pStr);
  *pEnd = '\n';

  return temp;
}
}