/*++

Module Name:
	hotelgwclient.c.

Abstract:
	This module implements hotel gateway client.

Revision History:
	Date    : Oct 07 2012.

	Author  : Unmesh Joshi (S-2515965).
		: Koustubha Bhat (S-2516144).

	VUnetID : uji300
		: kbt350

	Desc    : Created.
	
--*/

//===================================================================
//	Requirements
//===================================================================
//


//===================================================================
//	Test Cases
//===================================================================
//


/////////////////////////////////////////////////////////////////////
//	H E A D E R S.
/////////////////////////////////////////////////////////////////////

#include "hotelgwclient.h"

#include "../common.h"

#include <fcntl.h>
#include <sys/io.h>
#include <string.h>

#include <sys/socket.h>
#include <arpa/inet.h>  // htons
#include <netinet/in.h> // struct sockaddr_in
#include <netdb.h>

/////////////////////////////////////////////////////////////////////
//	M A C R O S.
/////////////////////////////////////////////////////////////////////

#define CLIENT_LOG_FILE "./clientgw.log"
#define SERVER_PORT 9999


/////////////////////////////////////////////////////////////////////
//	G L O B A L S
/////////////////////////////////////////////////////////////////////

COMMAND Commands[] = {
                      {COMMAND_LIST_ID, COMMAND_LIST_STR, 0},
                      {COMMAND_BOOK_ID, COMMAND_BOOK_STR, 2},
                      {COMMAND_GUESTS_ID, COMMAND_GUESTS_STR, 1},
                    };


/////////////////////////////////////////////////////////////////////
//	F U N C T I O N  D E F I N T I O N S
/////////////////////////////////////////////////////////////////////
 

int GetIPAddressFromHostName(const char *pszHostName, char *pszIPAddress)
{
  int iRet;
  struct sockaddr_in *pSockAddrIn;

  char szTempIP[30];

  struct addrinfo AddrInfoHints;
  struct addrinfo *pAddrInfoServers;
  struct addrinfo *pTemp;

  memset(&AddrInfoHints, 0, sizeof(AddrInfoHints));
  AddrInfoHints.ai_family = AF_INET;
  AddrInfoHints.ai_socktype = SOCK_STREAM;

  iRet = getaddrinfo(pszHostName, NULL, &AddrInfoHints, &pAddrInfoServers);
  if (0 > iRet)
  {
    return -1;
  }

  for (pTemp = pAddrInfoServers; pTemp != NULL; pTemp = pTemp->ai_next)
  {
    pSockAddrIn = (struct sockaddr_in *) pTemp->ai_addr;
    strcpy(szTempIP, inet_ntoa( pSockAddrIn->sin_addr ));

    if (0 != strcmp("0.0.0.0", szTempIP))
    {
      strcpy(pszIPAddress, szTempIP);
      return 0;
    }
  }

  return -1;
}


int main(int argc, char *argv[], char *envp[])
{
  int iRet;
  int iLoop;
  int icbRead;
  int iLength;
  int iFdSock;
  int iCounter;
  char szIPAddress[30];
  struct sockaddr_in serverAddress;

  char szResult[80];
  char szMessage[25];
  char szCommandLine[MAX_BUF];

  if (1 == argc)
  {
    printf("Usage: ./hotelgwclient <IP Address> <options>\n");
    exit(0);
  }

  iFdSock = socket(AF_INET, SOCK_STREAM, 0);
  if (0 > iFdSock)
  {
    Log(CLIENT_LOG_FILE, "socket failed", "main", LOGLEVEL_FATAL_ERROR, NULL);
  }

  memset(&serverAddress, 0, sizeof(serverAddress));
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons(SERVER_PORT);

  iRet = GetIPAddressFromHostName(argv[1], szIPAddress);
  if (-1 == iRet)
  {
    Log(CLIENT_LOG_FILE,"Invalid host name", "main", LOGLEVEL_FATAL_ERROR, NULL);
  }

  iRet = inet_pton(AF_INET, szIPAddress, &serverAddress.sin_addr);
  if (0 >= iRet)
  {
    close(iFdSock);
    Log(CLIENT_LOG_FILE,"inet_pton failed", "main", LOGLEVEL_FATAL_ERROR, NULL);
  }

  iRet = connect(iFdSock, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
  if (0 > iRet)
  {
    close(iFdSock);
    Log(CLIENT_LOG_FILE,"connect failed", "main", LOGLEVEL_FATAL_ERROR, NULL);
  }


  //
  // We have to write whole command line to socket
  //
  strcpy(szCommandLine, "");
  iLength = strlen(szCommandLine);

  for (iLoop = 2; iLoop < argc; ++iLoop)
  {
    //argv[iLoop]
    if ((iLength + strlen(argv[iLoop]) >= MAX_BUF))
    {
      break;
    }

    strcat(szCommandLine, argv[iLoop]);
    strcat(szCommandLine, " "); 
    iLength = strlen(szCommandLine);
  }

  iRet = write(iFdSock, szCommandLine, strlen(szCommandLine));
  if (0 > iRet)
  {
    printf("Write Error.\n");
    Log(CLIENT_LOG_FILE,"Error in writing to the socket", "main", LOGLEVEL_ERROR, NULL);
  }

  shutdown(iFdSock, SHUT_WR);// Flushes the write stream. IMP!

  while (1)
  {
    iRet = read(iFdSock, szResult, ARRAY_SIZE(szResult));
    if (-1 == iRet || 0 == iRet)
    {
      sprintf(szMessage, "\niRet = %d read\n", iRet);
      Log(CLIENT_LOG_FILE, szMessage, "main", LOGLEVEL_ERROR, NULL);
      break;
    }
    else
    {
      printf("%s",szResult);
      //Log(CLIENT_LOG_FILE, szMessage, "main", LOGLEVEL_INFO, NULL);
    }
  }

  close(iFdSock);
  return 0; 

}
