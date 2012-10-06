/*++

Module Name:
	serv2.c.

Abstract:
	This module implements "one-process-per-request" TCP server.

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
//      Requirements
//===================================================================
//1. Server maintains a counter and this is a persistent variable.
//2. Server listens to client requests
//3. Each client request is processed by separate process
//4. Each Server sends the incremented counter value back to the client.
//5. The counter value 0 is the initial one and is never given to an
//   actual client. Essentially counter value cannot be negative.


//===================================================================
//      Test Cases
//===================================================================
//


/////////////////////////////////////////////////////////////////////
//      H E A D E R S.
/////////////////////////////////////////////////////////////////////

#include "common.h"

#include <sys/file.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <sys/wait.h>


/////////////////////////////////////////////////////////////////////
//      M A C R O S.
/////////////////////////////////////////////////////////////////////

#define SERVER_PORT 9999
#define SERVER_TCP_BACKLOG 5
#define COUNTER_FILE "./serv2.counter.dat"

#define	LOG_FILE	"./log.serv2.txt"


/////////////////////////////////////////////////////////////////////
//      S T R U C T U R E S
/////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////
//      F U N C T I O N  D E F I N T I O N S
/////////////////////////////////////////////////////////////////////

//
// This function is (almost) copy of writen() function W. Richard
// Stevens' book
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


void HandleSignal(int iSig)
{
  while (waitpid(0, NULL, WNOHANG) > 0){}

  signal(SIGCHLD, HandleSignal);
}

//
// This function reads the file and gets the value of the counter.
//
int
FetchCounterFromFile ()
{
  int iCounter = 0;
  int iFd;
  int iRet;

  iFd = open (COUNTER_FILE, O_RDONLY);
  if (0 > iFd)
  {
    Log("open failed", "FetchCounterFromFile", LOGLEVEL_FATAL_ERROR, NULL);
  }

  //
  // Mutually exclusive access to file (counter variable "repository").
  //
  iRet = flock(iFd, LOCK_EX);
  if (0 > iRet)
  {
    close(iFd);
    Log("flock failed", "FetchCounterFromFile", LOGLEVEL_ERROR, NULL);
    return -1;
  }

  iRet = read(iFd, &iCounter, sizeof(iCounter));
  if (sizeof(iCounter) != iRet)
  {
    flock(iFd, LOCK_UN);
    close(iFd);
    return -1;
  }

  flock(iFd, LOCK_UN);
  close (iFd);
  return iCounter;
}


int
UpdateCounterToFile (int iValue)
{
  int iFd;
  int iRet;

  iFd = open (COUNTER_FILE, O_WRONLY);
  if (0 > iFd)
  {
    Log("open failed", "FetchCounterFromFile", LOGLEVEL_FATAL_ERROR, NULL);
  }

  iRet = flock(iFd, LOCK_EX);
  if (0 > iRet)
  {
    close(iFd);
    Log("flock failed", "FetchCounterFromFile", LOGLEVEL_ERROR, NULL);
    return -1;
  }

  iRet = write(iFd, &iValue, sizeof(iValue));
  if (sizeof(iValue) != iRet)
  {
    flock(iFd, LOCK_UN);
    close(iFd);
    return -1;
  }

  flock(iFd, LOCK_UN);
  close (iFd);

  return 0;			//return 0 or -1 for success or failure.
}


int
DoWork ()
{
  int iRet, iCounter = 0;
  int iFd;

  iRet = access (COUNTER_FILE, F_OK);
  if (0 > iRet)
  {
      //
      // File does not exist.
      // Let's create it.
      //
      iFd = open (COUNTER_FILE, O_CREAT | O_RDWR, 0600);
      if (0 > iFd)
      {
	  Log("open failed"," DoWork()", LOGLEVEL_FATAL_ERROR, NULL);
      }

      iRet = flock(iFd, LOCK_EX);
      if (0 > iRet)
      {
	  Log("flock failed"," DoWork()", LOGLEVEL_ERROR, NULL);
	  return -1;
      }

      iRet = write(iFd, &iCounter, sizeof(iCounter));	// initialize persistent counter to zero.
      if (sizeof(iCounter) != iRet)
      {
         flock(iFd, LOCK_UN);
	 close(iFd);

	 Log("write failed"," DoWork()", LOGLEVEL_WARNING, NULL);
         return -1;
      }

      flock(iFd, LOCK_UN);
      close(iFd);
  }

  //
  // Read the file to fetch the counter.
  //
  iCounter = FetchCounterFromFile ();
////  printf("After fetch : iCounter = %d\n", iCounter);

  UpdateCounterToFile (++iCounter);

  printf("iCounter = %d\n", iCounter);
  return iCounter;
}


int
main ()
{
  int iSock, iNewSock, iRet, iLastError, iCounter, iOpt_enable = 1;
  socklen_t clientSockAddr_len;
  struct sockaddr_in servSockAddr, clientSockAddr;

  int iPID;

  InitLogging(LOG_FILE);

  iSock = socket (AF_INET, SOCK_STREAM, 0);
  if (0 > iSock)
  {
    Log("socket creation failed", "main", LOGLEVEL_FATAL_ERROR, NULL);
  }

  iRet = setsockopt (iSock, SOL_SOCKET, SO_REUSEADDR, &iOpt_enable,
		sizeof (iOpt_enable));

  //
  // Initialize the sockaddr_in structure
  //
  servSockAddr.sin_family = AF_INET;
  servSockAddr.sin_port = htons (SERVER_PORT);
  servSockAddr.sin_addr.s_addr = htonl (INADDR_ANY);

  //
  // Bind a port to the socket
  //
  iRet = bind (iSock, (struct sockaddr *) &servSockAddr, sizeof (servSockAddr));
  if (0 > iRet)
  {
    close (iSock);
    Log("bind failed", "main", LOGLEVEL_FATAL_ERROR, NULL);
  }

  //
  // Listen to the socket
  //
  iRet = listen (iSock, SERVER_TCP_BACKLOG);
  if (0 != iRet)
  {
    iLastError = errno;
    Log("listen failed", "main", LOGLEVEL_FATAL_ERROR, &iLastError);
  }

  signal(SIGCHLD, HandleSignal);

  //
  // Accept connection from a client
  //
  while (1)
  {
     iNewSock = accept (iSock, (struct sockaddr *) &clientSockAddr, &clientSockAddr_len);
     if (iNewSock < 0)
     {
        continue;
     }

     iPID = fork();
     if (0 > iPID)
     {
        close(iNewSock);
        Log("fork failed", "main", LOGLEVEL_FATAL_ERROR, NULL);
     }
     else if (0 == iPID)
     {
       close(iSock);
       iCounter = DoWork ();
       if (0 > iCounter)
       {
         close(iNewSock);
	 Log("Service denied", "main", LOGLEVEL_FATAL_ERROR, NULL);
       }

       iRet = Send(iNewSock, &iCounter, sizeof(iCounter));
       if (0 > iRet)
       {
         close(iNewSock);
	 Log("Send(write) failed","main", LOGLEVEL_FATAL_ERROR, NULL);
       }

       printf ("Sent the counter value %d to: %s\n", iCounter, inet_ntoa (clientSockAddr.sin_addr));

       close(iNewSock);
       exit(0);
    }
  }

  close (iSock);
  DeInitLogging();
  return 0;
}
