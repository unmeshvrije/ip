/*++

Module Name:
	paperserver_proc.c.

Abstract:
	This module implements paper server procedure.

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

//#include "paperserver_proc.h"
#include "ll.h"
#include "../common.h"


/////////////////////////////////////////////////////////////////////
//	M A C R O S.
/////////////////////////////////////////////////////////////////////

#define	exit_on_error(msg)\
			do{perror(msg); exit(EXIT_FAILURE);}while(0);

#define	MAX_ARGC	10
#define	CMD_LEN		80
#define	MAX_BUF		1024

#define SERVER_LOG_FILE "./server.log"


/////////////////////////////////////////////////////////////////////
//	G L O B A L S
/////////////////////////////////////////////////////////////////////

static int g_iPaperID = 0;

static SERVER_PAPER_LIST *pPaperListHead = NULL;
static SERVER_PAPER_LIST *pPaperListTail = NULL;


/////////////////////////////////////////////////////////////////////
//	F U N C T I O N  D E F I N T I O N S
/////////////////////////////////////////////////////////////////////


int* senddetails_1_svc(PAPER *pPaper, struct svc_req *req)
{
  static int iIndex;

  static SERVER_PAPER ServerPaper;

  if (NULL == pPaper)
  {
    return NULL;
  }

  //
  // This should be done only once.
  // We will same procedure for single file to transfer data
  //
  if (-1 == pPaper->iPaperNo)
  {
    //g_iPaperIndex = g_iPaperID;
    memset(&ServerPaper, 0, sizeof(ServerPaper));

    ++g_iPaperID;
    iIndex = g_iPaperID;
    
    ServerPaper.iPaperIndex = g_iPaperID;
    strcpy(ServerPaper.szAuthors[0], pPaper->szAuthors);
    strcpy(ServerPaper.szPaperTitle, pPaper->szPaperTitle);
    pPaper->iPaperNo = g_iPaperID;

    ServerPaper.lFileSize = pPaper->lFileSize;
  
//    iIndex = pPaper->iPaperNo;

    ServerPaper.pFileData =  (BYTE*)malloc(sizeof(BYTE) * pPaper->lFileSize);

    memcpy(ServerPaper.pFileData, pPaper->ByteFileChunk, pPaper->lChunkSize);
  }
  else if(-1 == pPaper->iChunkIndex)
  {
    //
    // Add the node
    //
    Log(SERVER_LOG_FILE, "Calling AddToList", "senddetails_1_svc", LOGLEVEL_INFO, NULL);
    AddToList(&ServerPaper, &pPaperListHead, &pPaperListTail);
  }
  else
  {
    memcpy(ServerPaper.pFileData + (pPaper->iChunkIndex * MAX_BUF), pPaper->ByteFileChunk, pPaper->lChunkSize);
  }

  return &iIndex; 

}


PAPER* fetchinfo_1_svc(int *piPaperIndex, struct svc_req *req)
{
  char chFound;
  int iPaperIndex;

  SERVER_PAPER_LIST *pNavigate;

  static PAPER Paper;
  memset(&Paper, 0, sizeof(Paper));

  if(NULL == piPaperIndex)
  {
    chFound = 'n';
  }
  else
  {
    iPaperIndex = *piPaperIndex;
    chFound = 'n';

    pNavigate = pPaperListHead;
    while (NULL != pNavigate)
    {
      if (pNavigate->Data.iPaperIndex == iPaperIndex)
      {
        Paper.iPaperNo = pNavigate->Data.iPaperIndex;
        strcpy(Paper.szAuthors, pNavigate->Data.szAuthors[0]);
        strcpy(Paper.szPaperTitle, pNavigate->Data.szPaperTitle);
        Paper.lFileSize = pNavigate->Data.lFileSize;
        chFound = 'y';
      }
      pNavigate = pNavigate->pNext;
    }
  }

  if('n' == chFound)
  {
    Paper.iChunkIndex = -1;
  }

  return &Paper;
}


PAPER* fetchdetails_1_svc(int *piPaperIndex, struct svc_req *req)
{
  char chFound;
  int iPaperIndex;
  char szLogBuffer[MAX_BUF];
  SERVER_PAPER_LIST *pNavigate;

  static PAPER Paper;
  memset(&Paper, 0, sizeof(Paper));

  static int iChunkIndex = 0;
  static long lRemainingBytes;

  if(NULL == piPaperIndex)
  {
    chFound = 'n';
  }
  else
  {
    iPaperIndex = *piPaperIndex;
    pNavigate = pPaperListHead;
    chFound = 'n';

    while (NULL != pNavigate)
    {
      if (pNavigate->Data.iPaperIndex == iPaperIndex)
      {
        chFound = 'y';
        Paper.iPaperNo = pNavigate->Data.iPaperIndex;
        strcpy(Paper.szAuthors, pNavigate->Data.szAuthors[0]);
        strcpy(Paper.szPaperTitle, pNavigate->Data.szPaperTitle);
        Paper.lFileSize = pNavigate->Data.lFileSize;

        if (0 == iChunkIndex)
        {
          lRemainingBytes = pNavigate->Data.lFileSize;      
        }
        else
        {
          lRemainingBytes -= MIN(MAX_BUF, lRemainingBytes);
        }

        if (0L == lRemainingBytes)
        {
          Paper.iChunkIndex = -1;
          //
          // Next call to fetch must start with chunk '0'
          //
          iChunkIndex = 0;
          break;
        }

        Paper.lChunkSize = MIN(MAX_BUF, lRemainingBytes);

        sprintf(szLogBuffer, "ChunkIndex = %d, Chunksize = %d, Remain = %d", iChunkIndex, Paper.lChunkSize,lRemainingBytes);
        Log(SERVER_LOG_FILE, szLogBuffer, "fetchdetails_1_svc", LOGLEVEL_INFO, NULL);

        memcpy(Paper.ByteFileChunk, pNavigate->Data.pFileData + (iChunkIndex * (MAX_BUF)), MIN(MAX_BUF, lRemainingBytes));
        ++iChunkIndex;
        break;
      }

      pNavigate = pNavigate->pNext;
    }
  }

  //
  //  To tell client that
  //  there is no paper with index he provided.
  //
  //  Note that we are using same variable with value '-1'
  //  to indicate client that fetching of files is finished.
  //
  //  Client will simply put data sent by server to console
  //  only if ChunkIndex field is not -1.
  //
  if ('n' == chFound)
  {
    Paper.iChunkIndex = -1;
  }

  return &Paper;
}


PAPER* fetchlist_1_svc(void *pVoid, struct svc_req *req)
{
  int iPaperIndex;
  char szLogBuffer[MAX_BUF];

  static short int siFirstTime = 0;
  static SERVER_PAPER_LIST *pNavigate;

  static PAPER Paper;
  //
  // This memset is crucial.
  // Otherwise this function is NOT idempotent!
  //
  memset(&Paper, 0, sizeof(Paper));

  if (0 == siFirstTime)
  {
    Log(SERVER_LOG_FILE, "siFirstTime...", "fetchdetails_1_svc", LOGLEVEL_INFO, NULL);
    pNavigate = pPaperListHead;
    ++siFirstTime;
  }

  //
  // If current node is the last one
  // then indicate so
  //
  if (NULL == pNavigate)
  {
    Log(SERVER_LOG_FILE, "pNavigate NULL !", "fetchdetails_1_svc", LOGLEVEL_INFO, NULL);
    Paper.iChunkIndex = -1;
    siFirstTime = 0;
  }
  else
  {
    Log(SERVER_LOG_FILE, "copying", "fetchdetails_1_svc", LOGLEVEL_INFO, NULL);
    Paper.iPaperNo = pNavigate->Data.iPaperIndex;
    strcpy(Paper.szAuthors, pNavigate->Data.szAuthors[0]);
    strcpy(Paper.szPaperTitle, pNavigate->Data.szPaperTitle);

    pNavigate = pNavigate->pNext;
  }

  return &Paper;
}

int* removedetails_1_svc(int *piPaperIndex, struct svc_req *req)
{
  static int iRet;

  if (NULL == piPaperIndex)
  {
    iRet = -1;
  }
  else
  {
    iRet = RemoveFromList(*piPaperIndex, &pPaperListHead, &pPaperListTail);
  }

  return &iRet;
}