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

/*

*/
PAPER* fetchdetails_1_svc(FILEINFO *pFileInfo, struct svc_req *req)
{
  char chFound;
  char szLogBuffer[MAX_BUF];
  SERVER_PAPER_LIST *pNavigate;

  static PAPER Paper;
  memset(&Paper, 0, sizeof(Paper));

  int iPaperIndex;
  int iChunkIndex;
  long lRemainingSize;

  if(NULL == pFileInfo)
  {
    chFound = 'n';
  }
  else
  {
    iPaperIndex = pFileInfo->iPaperIndex;
    iChunkIndex = pFileInfo->iChunkIndex;
    lRemainingSize = pFileInfo->lRemainingSize;

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

	//
	// If it is first chunk, client does not know about file size
	// so we must read min (1024, file_Size), as
	// file size can be < 1024
	//
	if (0 == iChunkIndex)
         Paper.lChunkSize = MIN(MAX_BUF, pNavigate->Data.lFileSize);
	else
	 Paper.lChunkSize = MIN(MAX_BUF, lRemainingSize);

        sprintf(szLogBuffer, "ChunkIndex = %d, Chunksize = %ld, Remain = %ld", iChunkIndex, Paper.lChunkSize, lRemainingSize);
        Log(SERVER_LOG_FILE, szLogBuffer, "fetchdetails_1_svc", LOGLEVEL_INFO, NULL);

	memcpy(Paper.ByteFileChunk, pNavigate->Data.pFileData + (iChunkIndex * (MAX_BUF)), Paper.lChunkSize);
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


PAPER* fetchlist_1_svc(int *pithElement, struct svc_req *req)
{
  int iLoop;
  int iTimesJump;
  SERVER_PAPER_LIST *pNavigate;

  static PAPER Paper;
  memset(&Paper, 0, sizeof(Paper));

  iTimesJump = (*pithElement) - 1;

  pNavigate = pPaperListHead;

  for (iLoop = 1; iLoop <= iTimesJump; ++iLoop)
  {
    if (NULL == pNavigate)
      break;

    pNavigate = pNavigate->pNext;
  }

  if (NULL != pNavigate)
  {
    Paper.iPaperNo = pNavigate->Data.iPaperIndex;
    strcpy(Paper.szAuthors, pNavigate->Data.szAuthors[0]);
    strcpy(Paper.szPaperTitle, pNavigate->Data.szPaperTitle);
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
