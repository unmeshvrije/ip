/*++

Module Name:
	serv1.c.

Abstract:
	This module implements the server side of the TCP connec
	-tion required for the "Content preserving server"  .

Revision History:
	Date    : Sep 26 2012.

	Author  : Unmesh Joshi (S-2515965).
		: Koustubha Bhat (S-2516144).

	VUnetID : uji300
		: kbt350

	Desc    : Created.
	Owner	: Koustubha Bhat
--*/


//===================================================================
//	Requirements
//===================================================================
//1. Server maintains a counter and this is a persistent variable.
//2. Server listens to client requests
//3. Each client request is processed by incrementing the common pers
//   -istent counter.
//4. Server sends the incremented counter value back to the client.
//5. The counter value 0 is the initial one and is never given to an
//   actual client. Essentially counter value cannot be negative.


//===================================================================
//	Test Cases
//===================================================================
//


/////////////////////////////////////////////////////////////////////
//	H E A D E R S.
/////////////////////////////////////////////////////////////////////
#include <arpa/inet.h>
#include <netinet/in.h>

#include "common.h"


/////////////////////////////////////////////////////////////////////
//	M A C R O S.
/////////////////////////////////////////////////////////////////////
//
#define SERVER_PORT 9999
#define SERVER_TCP_BACKLOG 5
#define COUNTER_FILE "./serv1.counter.dat"

#define	LOG_FILE	"./log.serv1.txt"


/////////////////////////////////////////////////////////////////////
//	S T R U C T U R E S
/////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////
//	F U N C T I O N  D E F I N T I O N S
/////////////////////////////////////////////////////////////////////


//
// This function is (almost) copy of writen() function Richar Stevens book
//
int
Send (int socketId, const void *pData, size_t n)
{
  int iLeft;
  const char *pchData;
  ssize_t bytesWritten = 0;

  iLeft = n;
  pchData = pData;

  while (iLeft > 0)
  {
      bytesWritten = write (socketId, pchData, iLeft);
      if (bytesWritten <= 0)
      {
        if (errno == EINTR)
	{
	  bytesWritten = 0;
	}
	else
	{
	  return -1;
	}
      }

      iLeft -= bytesWritten;
      pchData += bytesWritten;
  }

  return n;
}

int FetchCounterFromFile()
{
  //Reads the file and gets the value of the counter.
  int iCounter=0;
  FILE *fp;
  
  fp = fopen(COUNTER_FILE, "r");
  if (NULL == fp)
  {
    // improve logging
    return -1;
  }
  
  fscanf(fp, "%d", &iCounter);  
  fclose(fp);
  
  return iCounter;
}

int UpdateCounterToFile(int value)
{
  //Reads the file and gets the value of the counter.
  FILE *fp;
  
  fp = fopen(COUNTER_FILE, "w+");
  if (NULL == fp)
  {
    // improve logging
    return -1;
  }
  
  fprintf(fp, "%d", value);  
  fclose(fp);
  
  return 0; //return 0 or -1 for success or failure.
}

int DoWork()
{
  int iRet, iCounter = 0;
  FILE *fp;
  
  iRet = access(COUNTER_FILE, F_OK);
  if ( 0 > iRet)
  {
    // lets create it.
    fp = fopen(COUNTER_FILE, "w");
    if (NULL == fp)
    {
      Log("fopen failed", "DoWork", LOGLEVEL_ERROR, NULL);
      return -1;
    }

    fprintf(fp, "%d", 0); // initialize persistent counter to zero.
    fclose(fp);
  }
  
  // read the file to fetch the counter.
  iCounter = FetchCounterFromFile();
  
  UpdateCounterToFile(++iCounter);

  return iCounter;
}


int main()
{
  int iSock, iNewSock, iRet, iLastError, iCounter, iOpt_enable=1;
  socklen_t clientSockAddr_len;
  struct sockaddr_in servSockAddr, clientSockAddr;

  InitLogging(LOG_FILE);
  iSock = socket(AF_INET, SOCK_STREAM, 0);
  if ( 0 > iSock)
  {
    Log("socket creation failed", "main", LOGLEVEL_FATAL_ERROR, NULL);
  }
  

  iRet = setsockopt(iSock, SOL_SOCKET, SO_REUSEADDR, &iOpt_enable, sizeof(iOpt_enable));
  
  //
  // Initialize the sockaddr_in structure
  // 
  servSockAddr.sin_family = AF_INET;
  servSockAddr.sin_port = htons(SERVER_PORT);
  servSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  
  //
  // Bind a port to the socket
  //
  iRet = bind(iSock, (struct sockaddr *) &servSockAddr, sizeof(servSockAddr) );
  if (0 > iRet)
  {
    close(iSock); 
    Log("bind failed", "main", LOGLEVEL_FATAL_ERROR, NULL);
  }
  
  //
  // Listen to the socket
  //
  iRet = listen(iSock, SERVER_TCP_BACKLOG);
  if (0 != iRet)
  {
    iLastError = errno;
    Log("listen failed", "main", LOGLEVEL_FATAL_ERROR, &iLastError);
  }
  
  //accept connection from a client
  while (1)
  {
    iNewSock = accept(iSock, (struct sockaddr *)&clientSockAddr, &clientSockAddr_len);
  
    iCounter = DoWork();
  
    iRet = Send(iNewSock, &iCounter, sizeof(iCounter));
    if ( 0 > iRet)
    {
      Log("Send failed", "main", LOGLEVEL_FATAL_ERROR, NULL);
    }
    printf("Sent the counter value %d to: %s\n", iCounter, inet_ntoa(clientSockAddr.sin_addr));
  
    close(iNewSock);
  }

  close(iSock);
  DeInitLogging();
  return 0; 
}
