/*++

Module Name:
	talk.c.

Abstract:
	This module is talk program.

Revision History:
	Date    : Sep 28 2012.

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
//1. Server takes no parameter and starts listening on port
//2. Client takes server host name as parameter and has to connect to
//   the same
//3. Multiple servers on same machine talking to their
//   corresponding clients.
//4. A) Stop after connection is established.
//   b) Talk to each other (no synchronization)
//

//===================================================================
//      Test Cases
//===================================================================
//


/////////////////////////////////////////////////////////////////////
//      H E A D E R S.
/////////////////////////////////////////////////////////////////////

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>

#include "common.h"
#include "chat.h"


/////////////////////////////////////////////////////////////////////
//      M A C R O S.
/////////////////////////////////////////////////////////////////////

#define SERVER_PORT 9999
#define SERVER_TCP_BACKLOG 5

#define	CREATE_SERVER	0
#define	CREATE_CLIENT	1

#define CONTROL(x) 	((x) & 0x1F)

/////////////////////////////////////////////////////////////////////
//      G L O B A L S
/////////////////////////////////////////////////////////////////////

//struct sockaddr g_cmdLineServerAddress;

struct sockaddr_in g_serverAddress;


/////////////////////////////////////////////////////////////////////
//      S T R U C T U R E S
/////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////
//      F U N C T I O N  D E F I N T I O N S
/////////////////////////////////////////////////////////////////////

//
//
//
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
      return 0;
    }
  }

  return -1;
}

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
// This is thread function
// Its job is to read data from console and write it to network socket
//
// It should read data from ME part of console window.
//
void *ReadFromConsole (void *argp)
{
  int iRet;
  int iIndex;
  unsigned char chData;
  int iLastError;
  char arrchBuffer[MAX_BUF];

  TALKINFO *pTalkInfoMe = (TALKINFO *)argp;
  int iSocket = pTalkInfoMe->iSock;

  iIndex = 0;

  wmove(pTalkInfoMe->pWindow, 0/*pTalkInfoMe->pWin->iY+1*/, 0/*pTalkInfoMe->pWin->iX+1*/);
  wrefresh(pTalkInfoMe->pWindow);

  while (1)
    {
      iRet = wgetch(pTalkInfoMe->pWindow);
      if ((EOF == iRet) || (CONTROL('D') == iRet))
	{
	  break;
	}
      chData = (char)iRet;

      if ('\n' == chData)
	{
	  //
	  //  We won't send only new lines :)
	  //
	  if (0 == iIndex)
	    {
	      continue;
	    }

	  //sprintf(LogBuffer, "String of size %d sent\n", iIndex);
	  //Log(LogBuffer, "ReadFromConsole", LOGLEVEL_INFO, NULL);

	  //
	  //  Send buffer.
	  //
          arrchBuffer[iIndex++] = (char)chData;
	  iRet = Send (iSocket, arrchBuffer, iIndex);
	  if ( 0 > iRet)
	  {
	    iLastError = errno;
	    Log ("Error sending data.\n", "ReadFromConsole", LOGLEVEL_ERROR, &iLastError);
	  }

	  //
	  //  Reset buffer
	  //
	  memset (arrchBuffer, 0, sizeof (arrchBuffer));
	  iIndex = 0;
	}
	else
	{
          arrchBuffer[iIndex++] = (char)chData;
          if (iIndex == MAX_BUF - 1)
	  {
	    //To do
	    Send (iSocket, arrchBuffer, iIndex);
	    memset (arrchBuffer, 0, sizeof (arrchBuffer));
	    iIndex = 0;
	  }
	}
    }

  //
  //  Ctrl^D is pressed. Write remaining data
  //
  iRet = Send (iSocket, arrchBuffer, iIndex);
  if ( 0 > iRet)
  {
   iLastError = errno;
   Log ("Error sending data.\n", "ReadFromNetwork", LOGLEVEL_ERROR, &iLastError);
  }

  //
  // Tell other party that you are don with writing
  //
  shutdown(iSocket, SHUT_WR);

  return 0;
}


//
// This is thread function.
// Its job is to read data from network and send it to console
//
// It should send data to OTHER part of console window.
//
void * ReadFromNetwork (void *argp)
{
  //int iSock = *((int *) argp);
  int iLastError = 0;
  char buff[MAX_BUF];
  int iBytesRead = 0;

  TALKINFO *pTalkInfoOther = (TALKINFO *)argp;
  int iSock = pTalkInfoOther->iSock;

  //wmove(pTalkInfoOther->pWindow, pTalkInfoOther->pWin->iY+1, pTalkInfoOther->pWin->iX+1);
  //wrefresh(pTalkInfoOther->pWindow);
  do
    {
      iBytesRead = read (iSock, buff, MAX_BUF);
      if ((0 > iBytesRead) || (iBytesRead > MAX_BUF - 1))
	{
	  iLastError = errno;
	  Log ("Error in reading from socket.\n", "GetFromNetwork",
	       LOGLEVEL_ERROR, &iLastError);
	  break;
	}
      buff[iBytesRead] = '\0';

      wprintw(pTalkInfoOther->pWindow, buff);
      wrefresh(pTalkInfoOther->pWindow);
    }
  while (0 != iBytesRead);

  strcpy(buff, "\n!!!OTHER PARTY CLOSED!!!\n");
  iBytesRead = strlen(buff) + 1;
  wprintw(pTalkInfoOther->pWindow, buff);
  wrefresh(pTalkInfoOther->pWindow);

  return 0;
}



//
//  This function creates sender and receiver threads
//  and "joins" both
//
int
DoWork (int iSock, TALKINFO *pTalkInfoMe, TALKINFO *pTalkInfoOther)
{
  int iRet;
  void *status1;
  void *status2;
  pthread_t Threads[2];
  pthread_attr_t attr;

  iRet = pthread_attr_init (&attr);
  if (iRet < 0)
    {
      perror ("Thread attribute init failed");
    }

  iRet = pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_JOINABLE);
  if (iRet < 0)
    {
      perror ("setdetachstate failed");
    }

  iRet =  pthread_create (&Threads[1], &attr, ReadFromConsole, pTalkInfoMe);
  if (iRet < 0)
    {
      perror ("create thread failed");
      return -1;
    }

  iRet =  pthread_create (&Threads[0], &attr, ReadFromNetwork, pTalkInfoOther);
  if (iRet < 0)
    {
      perror ("create thread failed");
      return -1;
    }

  iRet = pthread_join (Threads[0], &status1);
  if (0 != iRet)
    {
      perror ("Cannot join thread : Do not expect expected output");
    }

  iRet = pthread_join (Threads[1], &status2);
  if (0 != iRet)
    {
      perror ("Cannot join thread : Do not expect expected output");
    }

  //
  // We should not call pthread_exit because 
  // it is for explicit termination of thread
  //
  // When we were using this, clean up was not done properly
  // because the control was not coming to main() function
  // which has call to endwin() method
  //
  //pthread_exit(status1);
  //pthread_exit(status2);
  return 0;
}


int
CreateClient (TALKINFO *pTalkInfoMe, TALKINFO *pTalkInfoOther)
{
  int iRet, iFdSock, iTalkPort, iTalkSock;

  iFdSock = socket (AF_INET, SOCK_STREAM, 0);
  if (0 > iFdSock)
    {
      Log ("socket failed", "CreateClient", LOGLEVEL_ERROR, NULL);
      return -1;
    }

  iRet =  connect (iFdSock, (struct sockaddr *) &g_serverAddress, sizeof (g_serverAddress));
  if (0 > iRet)
    {
      close (iFdSock);
      Log ("connect failed", "CreateClient", LOGLEVEL_ERROR, NULL);
      return -1;
    }

  Log ("connect succeeded", "CreateClient", LOGLEVEL_INFO, NULL);

  while ((iRet = read (iFdSock, &iTalkPort, sizeof (iTalkPort))) != sizeof (iTalkPort));
  
  Log ("Talk port is read succeeded", "CreateClient", LOGLEVEL_INFO, NULL);

  //
  // Close connection socket
  //
  close (iFdSock);

  iTalkSock = socket (AF_INET, SOCK_STREAM, 0);
  if (0 > iTalkSock)
    {
      Log ("socket failed", "CreateClient", LOGLEVEL_ERROR, NULL);
      return -1;
    }

  Log ("Talk socket created", "CreateClient", LOGLEVEL_INFO, NULL);

  //
  // Connect to talk port.
  //
  g_serverAddress.sin_port = htons (iTalkPort);

  iRet =   connect (iTalkSock, (struct sockaddr *) &g_serverAddress, sizeof (g_serverAddress));
  if (0 > iRet)
    {
      close (iTalkSock);
      Log ("connect failed", "CreateClient", LOGLEVEL_ERROR, NULL);
      return -1;
    }

  Log ("Talk connect succeeded", "CreateClient", LOGLEVEL_INFO, NULL);

  pTalkInfoMe->iSock = iTalkSock;
  pTalkInfoOther->iSock = iTalkSock;
  iRet = DoWork (iTalkSock, pTalkInfoMe, pTalkInfoOther);

  close (iTalkSock);

  if (-1 == iRet)
  {
    return  -1;
  }

  return 0;
}


int
CreateServer (TALKINFO *pTalkInfoMe, TALKINFO *pTalkInfoOther)
{
  int iSock, iRet, iOpt_enable = 1, iNewSock, iTalkSock, iTalkPort, iLastError, iNewTalkSock;

  struct sockaddr_in servSockAddr, clientSockAddr;
  socklen_t clientSockAddr_len;

  iSock = socket (AF_INET, SOCK_STREAM, 0);
  if (0 > iSock)
    {
      Log ("socket creation failed", "CreateServer", LOGLEVEL_ERROR, NULL);
      return -1;
    }

  Log ("socket created", "CreateServer", LOGLEVEL_INFO, NULL);

  iRet = setsockopt (iSock, SOL_SOCKET, SO_REUSEADDR, &iOpt_enable, sizeof (iOpt_enable));

  //
  // Initialize the sockaddr_in structure
  //
  servSockAddr.sin_family = AF_INET;
  servSockAddr.sin_port = htons (SERVER_PORT);	//Known port
  servSockAddr.sin_addr.s_addr = htonl (INADDR_ANY);

  //
  // Bind a port to the socket
  //
  iRet = bind (iSock, (struct sockaddr *) &servSockAddr, sizeof (servSockAddr));
  if (0 > iRet)
    {
      close (iSock);
      Log ("bind failed", "CreateServer", LOGLEVEL_ERROR, NULL);
      return -1;
    }

  Log ("bind succeeded", "CreateServer", LOGLEVEL_INFO, NULL);

  //
  // Listen to the socket
  //
  iRet = listen (iSock, SERVER_TCP_BACKLOG);
  if (0 != iRet)
    {
      iLastError = errno;
      close (iSock);
      Log ("listen failed", "CreateServer", LOGLEVEL_ERROR, &iLastError);

      return -1;
    }

  Log ("listen succeeded", "CreateServer", LOGLEVEL_INFO, NULL);
  iNewSock = accept (iSock, (struct sockaddr *) &clientSockAddr, &clientSockAddr_len);
  if (0 > iNewSock)
    {
      Log ("accept failed", "CreateServer", LOGLEVEL_ERROR, NULL);
      return -1;
    }

  Log ("accept succeeded", "CreateServer", LOGLEVEL_INFO, NULL);

  iTalkPort = SERVER_PORT + (random () % SERVER_PORT) + 1;

  iTalkSock = socket (AF_INET, SOCK_STREAM, 0);
  if (0 > iTalkSock)
    {
      Log ("socket failed for TalkSock", "CreateServer", LOGLEVEL_ERROR,
	   NULL);
      return -1;
    }

  Log ("Talk socket created", "CreateServer", LOGLEVEL_INFO, NULL);

  iRet = setsockopt (iTalkSock, SOL_SOCKET, SO_REUSEADDR, &iOpt_enable, sizeof (iOpt_enable));

  //
  // Initialize the sockaddr_in structure
  //
  servSockAddr.sin_family = AF_INET;
  servSockAddr.sin_port = htons (iTalkPort);	//Known port
  servSockAddr.sin_addr.s_addr = htonl (INADDR_ANY);

  // 
  // Bind a port to the socket
  //
  iRet = bind (iTalkSock, (struct sockaddr *) &servSockAddr, sizeof (servSockAddr));
  if (0 > iRet)
    {
      close (iTalkSock);
      Log ("bind failed", "CreateServer", LOGLEVEL_ERROR, NULL);
      return -1;
    }

  Log ("Talk bind succeeded", "CreateServer", LOGLEVEL_INFO, NULL);

  //
  // Listen to the socket
  //
  iRet = listen (iTalkSock, SERVER_TCP_BACKLOG);
  if (0 > iRet)
    {
      close (iTalkSock);
      Log ("listen on Talk port failed", "CreateServer", LOGLEVEL_ERROR,
	   NULL);
      return -1;
    }

  Log ("Talk listen succeeded", "CreateServer", LOGLEVEL_INFO, NULL);
  iRet = Send (iNewSock, &iTalkPort, sizeof(iTalkPort));
  if (0 > iRet)
    {
      close (iTalkSock);
      Log ("Sending  Talk port to new port failed", "CreateServer",
	   LOGLEVEL_ERROR, NULL);
      return -1;
    }

  Log ("Talk port SENT to client", "CreateServer", LOGLEVEL_INFO, NULL);

  iNewTalkSock = accept (iTalkSock, (struct sockaddr *) &clientSockAddr, &clientSockAddr_len);
  if (0 > iNewSock)
    {
      Log ("accept failed", "CreateServer", LOGLEVEL_ERROR, NULL);
      return -1;
    }

  Log ("Got client request", "CreateServer", LOGLEVEL_INFO, NULL);

  //
  // Close connection ports
  //
  close (iNewSock);
  close (iSock);

  pTalkInfoMe->iSock = iNewTalkSock;
  pTalkInfoOther->iSock = iNewTalkSock;
  iRet = DoWork(iNewTalkSock, pTalkInfoMe, pTalkInfoOther);

  close (iNewTalkSock);
  close (iTalkSock);

  if (-1 == iRet)
  {
    return -1;
  }

  return 0;
}


int
ProcessCommandLine (int argc, char *argv[])
{
  int iRet;
  char szIPAddress[30];
  //struct addrinfo hints, *pResults;
  //

  if (1 == argc)
    {
      return CREATE_SERVER;
    }
  else if (2 == argc)
    {
      g_serverAddress.sin_family = AF_INET;
      g_serverAddress.sin_port = htons (SERVER_PORT);
      /*hints.ai_flags = 0;
         hints.ai_family = AF_INET;
         hints.ai_socktype = SOCK_STREAM;
         hints.ai_protocol = 0;
         hints.ai_addrlen = sizeof(g_cmdLineServerAddress);
         hints.ai_canonname = NULL;

         iRet = getaddrinfo(argv[1], NULL, &hints, &pResults); */
	 iRet = GetIPAddressFromHostName(argv[1], szIPAddress);
	 if (-1 == iRet)
	 {
	   Log ("Invalid server name specified.\n", "ProcessCommandLine",
	       LOGLEVEL_ERROR, NULL);
	   return -1;
	 }

	 Log(szIPAddress, "ProcessArg", LOGLEVEL_INFO, NULL);

      iRet = inet_pton (AF_INET, szIPAddress, &g_serverAddress.sin_addr);
      if (iRet < 0)
	{
	  Log ("Invalid server name specified.\n", "ProcessCommandLine",
	       LOGLEVEL_ERROR, NULL);
	  return -1;
	}

      //    g_cmdLineServerAddress = *(pResults->ai_addr);
      //   free(pResults);

      return CREATE_CLIENT;
    }
  else
    {
      return -1;
    }

  return -1;
}

int
main (int argc, char *argv[])
{
  int iRet;
  int iCreate;
  TALKINFO TalkInfoMe;
  TALKINFO TalkInfoOther;

  //
  // For logging in ProcessCommandLine
  //
  if (argc > 1)
  {
    InitLogging("./log.client.txt");
  }
  else
  {
    InitLogging("./log.server.txt");
  }
  
  iCreate = ProcessCommandLine (argc, argv);
  if (-1 == iCreate)
  {
    DeInitLogging();
    exit (0);
  }

  memset(&TalkInfoMe, 0, sizeof(TalkInfoMe));
  memset(&TalkInfoOther, 0, sizeof(TalkInfoOther));

  TalkInfoMe.pWin = (WIN*)malloc(sizeof(WIN));
  TalkInfoOther.pWin = (WIN*)malloc(sizeof(WIN));

  //
  // Create basic CHAT windows using ncurses
  //
  iRet = InitWindows(&TalkInfoMe, &TalkInfoOther);
  if (-1 == iRet)
  {
    Log("Initwindows() failed", "main", LOGLEVEL_FATAL_ERROR, NULL);
  }

  if (CREATE_CLIENT == iCreate)
    {
      CreateClient (&TalkInfoMe, &TalkInfoOther);
    }
  else if (CREATE_SERVER == iCreate)
    {
      CreateServer (&TalkInfoMe, &TalkInfoOther);
    }

  Log("End of MAIN", "main", LOGLEVEL_INFO, NULL);

  //
  // Clean up ncurses windows...
  //
  DeInitWindows(&TalkInfoMe, &TalkInfoOther);

  DeInitLogging();

  //
  // WINDOW structures ar freed by DeInitWindows()
  // We have to free WIN structures, which we are
  // using for our convenience
  //
  free(TalkInfoMe.pWin);
  free(TalkInfoOther.pWin);

  return 0;
}
