/*++

Module Name:
	client.c.

Abstract:
	This module is simple TCP client.

Revision History:
	Date    : Sep 26 2012.

	Author  : Unmesh Joshi (S-2515965).
		: Koustubha Bhat (S-2516144).

	VUnetID : uji300
		: kbt350

	Desc    : Created.
	
--*/

//===================================================================
//	Requirements
//===================================================================
//1. Connect to server
//2. Request for counter
//3. Print the counter (received as reply from the server)


//===================================================================
//	Test Cases
//===================================================================
//


/////////////////////////////////////////////////////////////////////
//	H E A D E R S.
/////////////////////////////////////////////////////////////////////

#include "common.h"

#include <sys/socket.h>
#include <arpa/inet.h>	// htons
#include <netinet/in.h>	// struct sockaddr_in
#include <netdb.h>


/////////////////////////////////////////////////////////////////////
//	M A C R O S.
/////////////////////////////////////////////////////////////////////

#define	SERVER_PORT	9999

#define	LOG_FILE	"./log.client.txt"


/////////////////////////////////////////////////////////////////////
//	S T R U C T U R E S
/////////////////////////////////////////////////////////////////////


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
  int icbRead;
  int iFdSock;
  int iCounter;
  char szIPAddress[30];
  struct sockaddr_in serverAddress;


  if (2 != argc)
  {
    printf("Usage: ./client <IP Address>\n");
    exit(0);
  }

  InitLogging(LOG_FILE);

  iFdSock = socket(AF_INET, SOCK_STREAM, 0);
  if (0 > iFdSock)
  {
    Log("socket failed", "main", LOGLEVEL_FATAL_ERROR, NULL);
  }

  memset(&serverAddress, 0, sizeof(serverAddress));
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons(SERVER_PORT);

  iRet = GetIPAddressFromHostName(argv[1], szIPAddress);
  if (-1 == iRet)
  {
    Log("Invalid host name", "main", LOGLEVEL_FATAL_ERROR, NULL);
  }

  iRet = inet_pton(AF_INET, szIPAddress, &serverAddress.sin_addr);
  if (0 >= iRet)
  {
    close(iFdSock);
    Log("inet_pton failed", "main", LOGLEVEL_FATAL_ERROR, NULL);
  }

  iRet = connect(iFdSock, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
  if (0 > iRet)
  {
    close(iFdSock);
    Log("connect failed", "main", LOGLEVEL_FATAL_ERROR, NULL);
  }
  //
  //	TODO: connection errors
  //

  icbRead = 0;
  do
  {
    icbRead = read(iFdSock, &iCounter, sizeof(iCounter));
    if (0 > icbRead)
    {
      break;
    }

  } while(icbRead != sizeof(iCounter));

  printf("%d", iCounter);

  if (0 > icbRead)
  {
    close(iFdSock);
    Log("read failed", "main",LOGLEVEL_FATAL_ERROR, NULL);
  }

  close(iFdSock);
  DeInitLogging();
  return 0; 

}
