//***************************************************************************
// File name:		mathPacket_client.c
// Author:			Cayden Wagner
// Date:				October 19 2021
// Class:				CS 360
// Assignment:	mathPacket
// Purpose:			To interact with the mathPacket server by sending a request 
//              and printing the response
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
const char NEW_LINE = '\n';

bool isEndOf(char[]);
int checkErrorCode(char[]);
bool checkHeader(char[], char[]);

void formatXstrings(char[]);
void formatStrings(char[]);
void receiveMathPacket(int, char[]);
void errorTestResponse(char[]);

void structureVersion1Packet(char[], char*, char* , char*);
void structureFirstRequest(char[], char*, char*, char*);
void structureContinuePacket(char[], char*, char*, bool);

/****************************************************************************
 Function:		main
 
 Description:	Sends a request to the math packet server and prints the 
              response
 
 Parameters:	int argc: number of command line arguments
							char **argv: the command line arguments
 
 Returned:		EXIT_SUCCESS
****************************************************************************/
int main(int argc, char **argv)
{
  const int MAX_SIZE = 1024, OK_CODE = 100;
  const int  VERSION_1_SIZE[2] = { 6, 7 };

  char szGetRequest[MAX_SIZE];
  char szGetResponse[MAX_SIZE];
  
  int connSocket, errorCode;
  bool bIsDisplay = false, bIsLastPacket;

  struct sockaddr_in sConnAddr;

  memset(szGetRequest, '\0', MAX_SIZE);
  memset(szGetResponse, '\0', MAX_SIZE);

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

  if (VERSION_1_SIZE[0] == argc || VERSION_1_SIZE[1] == argc)
  {
    structureVersion1Packet(szGetRequest, argv[3], argv[4], argv[5]);
  }
  else
  {
    structureFirstRequest(szGetRequest, argv[3], argv[4], argv[5]);
  }
  
  if (strcmp(argv[argc - 1], "-d") == 0)
  {
    printf("%s\n", szGetRequest);
    bIsDisplay = true;
  }
  
  send(connSocket, szGetRequest, strlen(szGetRequest), 0);
  receiveMathPacket(connSocket, szGetResponse);
  errorCode = checkErrorCode(szGetResponse);

  for (int i = 6; i < argc - 1 && OK_CODE == errorCode; i += 2)
  {
    memset(szGetRequest, '\0', sizeof(szGetRequest));
    bIsLastPacket = (i < argc -3);

    structureContinuePacket(szGetRequest, argv[i], argv[i+1], bIsLastPacket);

    if (bIsDisplay) 
    {
      printf("%s\n", szGetRequest);
    }

    send(connSocket, szGetRequest, strlen(szGetRequest), 0);
    receiveMathPacket(connSocket, szGetResponse);
    errorCode = checkErrorCode(szGetResponse);
  }

  formatXstrings(szGetResponse);
  formatStrings(szGetResponse);
  errorTestResponse(szGetResponse);

  printf("%s", szGetResponse);
  
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
 Function:		  structurerResponse
 
 Description:	  Structures a server request in the correct format
 
 Parameters:	  szGetRequest - an array of chars that holds the request
                operand1     - the first operand in the equation
                operator     - the operator in the equation
                operand2     - the second operand in the equation
 
 Returned:		  none
****************************************************************************/
void structureFirstRequest(char szGetRequest[], char* operand1, char* operator,
                      char* operand2)
{

  strncat(szGetRequest, "CALCULATE MATH/1.1\nOperand1: ", 
         (MAX_SIZE - strlen(szGetRequest)) - 1 );
  strncat(szGetRequest, operand1, (MAX_SIZE - strlen(szGetRequest)) - 1 );
  strncat(szGetRequest, "\nOperator: ", (MAX_SIZE - strlen(szGetRequest)) - 1 );
  strncat(szGetRequest, operator, (MAX_SIZE - strlen(szGetRequest)) - 1 );
  strncat(szGetRequest, "\nOperand2: ", (MAX_SIZE - strlen(szGetRequest)) - 1 );
  strncat(szGetRequest, operand2, (MAX_SIZE - strlen(szGetRequest)) - 1 );
  strncat(szGetRequest, "\nConnection: Keep-Alive\n\n", 
         (MAX_SIZE - strlen(szGetRequest)) - 1);
}
/****************************************************************************
 Function:		  formatXstrings
 
 Description:	  Takes every line in a server response and puts the lines that
                start with an X at the end of the response
 
 Parameters:	  response - the server response that is formatted
 
 Returned:		  none
****************************************************************************/
void formatXstrings(char response[])
{
  char* pStr = NULL, *pLine = NULL;
  char tempX[MAX_SIZE];
  char temp[MAX_SIZE];

  memset(temp, '\0', MAX_SIZE);
  memset(tempX, '\0', MAX_SIZE);

  pLine = strtok(response, "\n");

  while(NULL != pLine)
  {  
    pStr = strstr(pLine, "X");

    if(NULL != pStr)
    {
      strncat(tempX, pLine, (MAX_SIZE - strlen(tempX)) - 1 );
      tempX[strlen(tempX)] = NEW_LINE;
    }
    else
    {
      strncat(temp, pLine, (MAX_SIZE - strlen(temp)) - 1 );
      temp[strlen(temp)] = NEW_LINE;
    }
    pLine = strtok(NULL, "\n");
  } 

  strncat(temp, tempX, (MAX_SIZE - strlen(temp)) - 1 );

  memset(response, '\0', MAX_SIZE);
  memcpy(response, temp, MAX_SIZE);
}
/****************************************************************************
 Function:		  checkHeader
 
 Description:	  checks if the contents of a header is true or false
 
 Parameters:	  response - the response that is verified
                header   - the header that is evaluated as true or false
 
 Returned:		  none
****************************************************************************/
bool checkHeader(char response[], char header[])
{
  char* pStr, *pEnd, *pIsFound = NULL;

  pStr = strstr(response, header);
  pEnd = pStr;
  bool bStatus;

  if (NULL != pStr)
  { 
    while(NEW_LINE != *pEnd)
    {
      pEnd++;
    }

    *pEnd = '\0';

    pIsFound = strstr(pStr, "True");
  
    if (NULL != pIsFound)
    {
      bStatus = true;
    }
    else 
    {
      pIsFound = strstr(pStr, "False");
      
      if (NULL != pIsFound)
      {
      bStatus = false;
      }
    }

    *pEnd = NEW_LINE;
  }

  return bStatus;
}
/****************************************************************************
 Function:		  formatStrings
 
 Description:	  Formtas the Rounding and Overflow fields into the correct 
                formats
 
 Parameters:	  response - the server response that is formatted
 
 Returned:		  none
****************************************************************************/
void formatStrings(char response[])
{
  char newResponse[MAX_SIZE];
  char* pStr, *pEnd , tempChar;

  memset(newResponse, '\0', MAX_SIZE);

  pStr = &response[0];
  pEnd = strstr(response, "Rounding");

  if (NULL != pEnd)
  {
    tempChar = *pEnd;
    *pEnd = '\0';
    strncat(newResponse, pStr, (MAX_SIZE - strlen(newResponse)) - 1 );
    *pEnd = tempChar;

    while('\n' != *pEnd)
    {
      pEnd++;
    }

    while('\n' != *pEnd)
    {
      pEnd++;
    }
    pEnd++;
    
    if (checkHeader(response, "Rounding"))
    {
      strncat(newResponse, "Rounded!\n", (MAX_SIZE - strlen(newResponse)) - 1 );
    }
    else
    {
      strncat(newResponse, "No Rounding\n", (MAX_SIZE - strlen(newResponse))
              - 1 );
    }

    if (checkHeader(response, "Overflow"))
    {
      strncat(newResponse, "Overflow!\n", (MAX_SIZE - strlen(newResponse)) 
              - 1 );
    }
    else
    {
      strncat(newResponse, "No Overflow\n", (MAX_SIZE - strlen(newResponse)) 
              - 1 );
    }

    strncat(newResponse, pEnd, (MAX_SIZE - strlen(newResponse)) - 1 );
  }
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
    strncat(newResponse, "Response Code:", (MAX_SIZE - strlen(newResponse)) 
            - 1 );
    strncat(newResponse, pStr, (MAX_SIZE - strlen(newResponse)) - 1 );
    strncat(newResponse, "\nResponse Message:", (MAX_SIZE - strlen(newResponse)) 
            - 1 );

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
  else
  {
    *pEnd = ' ';
  }
}
/****************************************************************************
 Function:		  checkErrorCode
 
 Description:	  
 
 Parameters:	  respose - 
 
 Returned:		  none
****************************************************************************/
int checkErrorCode(char response[])
{
  char* pStr = NULL;
  char* pEnd = NULL;
  char tempChar;
  int errorCode;

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

  tempChar = *pEnd;
  *pEnd = '\0';
  errorCode = atoi(pStr);
  *pEnd = tempChar;

  return errorCode;
}
/****************************************************************************
 Function:		  structureContinuePacket
 
 Description:	  
 
 Parameters:	  respose - 
 
 Returned:		  none
****************************************************************************/
void structureVersion1Packet(char szGetRequest[], char* operand1, 
                             char* operator, char* operand2)
{
  strncat(szGetRequest, "CALCULATE MATH/1.0\n", 
         (MAX_SIZE - strlen(szGetRequest)) - 1 );
  strncat(szGetRequest, "Operand1: ", (MAX_SIZE - strlen(szGetRequest)) - 1 );
  strncat(szGetRequest, operand1, (MAX_SIZE - strlen(szGetRequest)) - 1 );
  strncat(szGetRequest, "\nOperator: ", (MAX_SIZE - strlen(szGetRequest)) - 1 );
  strncat(szGetRequest, operator, (MAX_SIZE - strlen(szGetRequest)) - 1 );
  strncat(szGetRequest, "\nOperand2: ", (MAX_SIZE - strlen(szGetRequest)) - 1 );
  strncat(szGetRequest, operand2, (MAX_SIZE - strlen(szGetRequest)) - 1 );
  strncat(szGetRequest, "\nConnection: Close\n\n", 
         (MAX_SIZE - strlen(szGetRequest)) - 1);
}
/****************************************************************************
 Function:		  structureContinuePacket
 
 Description:	  
 
 Parameters:	  respose - 
 
 Returned:		  none
****************************************************************************/
void structureContinuePacket(char szGetRequest[], char* operator, 
                             char* operand2, bool bIsContinue)
{
  strncat(szGetRequest, "CONTINUE MATH/1.1\n", 
         (MAX_SIZE - strlen(szGetRequest)) - 1 );
  strncat(szGetRequest, "Operator: ", (MAX_SIZE - strlen(szGetRequest)) - 1 );
  strncat(szGetRequest, operator, (MAX_SIZE - strlen(szGetRequest)) - 1 );
  strncat(szGetRequest, "\nOperand2: ", (MAX_SIZE - strlen(szGetRequest)) - 1 );
  strncat(szGetRequest, operand2, (MAX_SIZE - strlen(szGetRequest)) - 1 );
  
  if(bIsContinue)
  {
    strncat(szGetRequest, "\nConnection: Keep-Alive\n\n", 
           (MAX_SIZE - strlen(szGetRequest)) - 1);
  }
  else 
  {
  strncat(szGetRequest, "\nConnection: Close\n\n", 
         (MAX_SIZE - strlen(szGetRequest)) - 1);
  }
}