/*++

Module Name:
	serv2.c.

Abstract:
	This module implements "preforked" TCP server.

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
//2. Server listens to client requests.
//3. Each client request is processed by one of preforked processes.
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
#include <sys/ipc.h>
#include <sys/sem.h>


/////////////////////////////////////////////////////////////////////
//      M A C R O S.
/////////////////////////////////////////////////////////////////////

#define SERVER_PORT 9999
#define SERVER_TCP_BACKLOG 5
#define COUNTER_FILE "./serv3.counter.dat"

#define	MAX_PROC		10
#define	THRESHOLD_PERCENT	70

#define	LOG_FILE	"./log.serv3.txt"


/////////////////////////////////////////////////////////////////////
//      G L O B A L S
/////////////////////////////////////////////////////////////////////

int g_iDeficit = MAX_PROC;


/////////////////////////////////////////////////////////////////////
//      S T R U C T U R E S
/////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////
//      F U N C T I O N  D E F I N T I O N S
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
      Log ("open failed", "FetchCounterFromFile", LOGLEVEL_FATAL_ERROR, NULL);
    }

  iRet = flock (iFd, LOCK_EX);
  if (0 > iRet)
    {
      close (iFd);
      Log ("flock failed", "FetchCounterFromFile", LOGLEVEL_ERROR, NULL);
      return -1;
    }

  iRet = read (iFd, &iCounter, sizeof (iCounter));
  if (sizeof (iCounter) != iRet)
    {
      flock (iFd, LOCK_UN);
      close (iFd);
      return -1;
    }

  flock (iFd, LOCK_UN);
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
      Log ("open failed", "FetchCounterFromFile", LOGLEVEL_FATAL_ERROR, NULL);
    }

  iRet = flock (iFd, LOCK_EX);
  if (0 > iRet)
    {
      close (iFd);
      Log ("flock failed", "FetchCounterFromFile", LOGLEVEL_ERROR, NULL);
      return -1;
    }

  iRet = write (iFd, &iValue, sizeof (iValue));
  if (sizeof (iValue) != iRet)
    {
      flock (iFd, LOCK_UN);
      close (iFd);
      return -1;
    }

  flock (iFd, LOCK_UN);
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
	  Log ("open failed", " DoWork()", LOGLEVEL_FATAL_ERROR, NULL);
	}

      iRet = flock (iFd, LOCK_EX);
      if (0 > iRet)
	{
	  Log ("flock failed", " DoWork()", LOGLEVEL_ERROR, NULL);
	  return -1;
	}

      iRet = write (iFd, &iCounter, sizeof (iCounter));	// initialize persistent counter to zero.
      if (sizeof (iCounter) != iRet)
	{
	  flock (iFd, LOCK_UN);
	  close (iFd);

	  Log ("write failed", " DoWork()", LOGLEVEL_WARNING, NULL);
	  return -1;
	}

      flock (iFd, LOCK_UN);
      close (iFd);
    }

  //
  // Read the file to fetch the counter.
  //
  iCounter = FetchCounterFromFile ();
  ////printf ("After fetch : iCounter = %d\n", iCounter);

  UpdateCounterToFile (++iCounter);

  printf ("iCounter = %d\n", iCounter);
  return iCounter;
}


void ChildProcess(int iSock, int iSemId)
{
  int iRet, iLastError;
  int iCounter;
  int iNewSock;
  socklen_t clientSockAddr_len;
  struct sockaddr_in clientSockAddr;
  struct sembuf up = { 0, 1, 0 };
  struct sembuf down = { 0, -1, 0 };


  //
  // Acquire the semaphore
  //
  semop(iSemId, &down, 1);
  
  //
  // Accept connection from a client
  //
  iNewSock =  accept (iSock, (struct sockaddr *) &clientSockAddr, &clientSockAddr_len);
  
  //
  // Release the semaphore
  //
  semop(iSemId, &up, 1);
	
     printf("accept returned %d\n", iNewSock);
    if (iNewSock < 0)
    {
      iLastError = errno;
      Log ("accept failed", "ChildProcess", LOGLEVEL_FATAL_ERROR, &iLastError);
    }
 
    Log("Connection accepted.\n", "ChildProcess", LOGLEVEL_INFO, NULL);
 
  iCounter = DoWork ();
  
  if (0 > iCounter)
  {
    close (iNewSock);
    Log ("Service denied", "main", LOGLEVEL_FATAL_ERROR, NULL);
  }

  iRet =  Send (iNewSock, &iCounter, sizeof(iCounter));
  if (0 > iRet)
  {
      Log("Send failed", "ChildProcess", LOGLEVEL_ERROR, NULL);
  }

  close(iNewSock);
  exit(0);
}


int
main ()
{
  int iLoop, iStatus;
  int iSock, iRet, iLastError, iOpt_enable = 1;
  int iPID;
  int iSemId;
  struct sockaddr_in servSockAddr ;

  union semun 
  { 
    int val; 
    struct semids_d *buf; 
    unsigned short *array; 
    struct seminfo *__buf; 
  } arg;

  InitLogging(LOG_FILE);
  
  iSock = socket (AF_INET, SOCK_STREAM, 0);
  if (0 > iSock)
  {
    Log("socket creation failed", "main", LOGLEVEL_FATAL_ERROR, NULL);
  }

  iRet = setsockopt (iSock, SOL_SOCKET, SO_REUSEADDR, &iOpt_enable, sizeof (iOpt_enable));

  //
  // Initialize the sockaddr_in structure
  //
  servSockAddr.sin_family = AF_INET;
  servSockAddr.sin_port = htons (SERVER_PORT);
  servSockAddr.sin_addr.s_addr = htonl (INADDR_ANY);


  //
  // Bind a port to the socket
  //
  iRet =   bind (iSock, (struct sockaddr *) &servSockAddr, sizeof (servSockAddr));
  if (0 > iRet)
  {
      iLastError = errno;
      close (iSock);
      Log("bind failed", "main", LOGLEVEL_FATAL_ERROR, &iLastError);
  }
  
  //
  // Listen to the socket
  //
  iRet = listen (iSock, SERVER_TCP_BACKLOG);
  if (0 != iRet)
  {
      iLastError = errno;
      Log("Error in listening", "main", LOGLEVEL_FATAL_ERROR, &iLastError);
  }

  //
  // Initialize semaphore for mutual exclusion during accept call by child processes.
  //  
  iSemId = semget(IPC_PRIVATE, 1 , 0600);
	if ( iSemId < 0 )
	{
		iLastError = errno;
		Log("Error getting semaphore. Errno", "main", LOGLEVEL_FATAL_ERROR, &iLastError);
		// program exits here
	}

	arg.val = 1;
	if (0 > semctl (iSemId, 0, SETVAL, arg))
	{
		iLastError = errno;
		Log("Error initializing the semaphore", "main", LOGLEVEL_FATAL_ERROR, &iLastError);
		// program exits here
	}	

  
  g_iDeficit = MAX_PROC;
  
    for (iLoop = 0; iLoop < g_iDeficit; ++iLoop)
    {
	iPID = fork ();
	if (-1 == iPID)
	{
	    Log ("fork failed", "main", LOGLEVEL_WARNING, NULL);
	}

	if (0 == iPID)
	{
	  ChildProcess(iSock, iSemId);
	}
    }

  //
  // Wait for all children to die
  //
  while ((iPID = waitpid (-1, &iStatus, 0)) > 0)
  {
    printf("pid %d returned\n", iPID);
  }

  //
  // Destroy the semaphore
  //
  semctl(iSemId, 0, IPC_RMID);
  
  //
  // Close the socket
  //
  close(iSock);
  DeInitLogging();

  return 0;
}
