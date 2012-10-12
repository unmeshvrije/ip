/*++

Module Name:
	paperclient.c.

Abstract:
	This module implements paper client.

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

#include "paperclient.h"

#include "../common.h"

#include <fcntl.h>
#include <sys/io.h>
#include <string.h>

#include <unistd.h>


/////////////////////////////////////////////////////////////////////
//	M A C R O S.
/////////////////////////////////////////////////////////////////////

#define	TEST_ADD	0
#define	TEST_SHOW	0
#define TEST_FETCH 0
#define TEST_LIST 1

#define CLIENT_LOG_FILE "./client.log"

/////////////////////////////////////////////////////////////////////
//	G L O B A L S
/////////////////////////////////////////////////////////////////////

COMMAND Commands[] = {
                      {COMMAND_LIST_ID, COMMAND_LIST_STR, 0},
                      {COMMAND_ADD_ID, COMMAND_ADD_STR, 3},
                      {COMMAND_REMOVE_ID, COMMAND_REMOVE_STR, 1},
                      {COMMAND_INFO_ID, COMMAND_INFO_STR, 1},
                      {COMMAND_FETCH_ID, COMMAND_FETCH_STR, 1},
                    };


/////////////////////////////////////////////////////////////////////
//	F U N C T I O N  D E F I N T I O N S
/////////////////////////////////////////////////////////////////////
 

//
// This function cannot have name as "SendDetails"
// May be because the same name (though lowercase)
// has been taken by senddetails_1() server procedure
//
int SendDetailsInternal(PAPER *pPaper, CLIENT *pClient)
{
  int iFd;
  int iRead;
  int *piIndex;
  int iChunkIndex;

  iFd = open(pPaper->szFileName, O_RDONLY);
  if (-1 == iFd)
  {
    return -1;
  }

  pPaper->lFileSize = lseek(iFd, 0L, SEEK_END);
  pPaper->iChunkIndex = 0;
  printf("Reading file of size = %ld", pPaper->lFileSize);
  printf("Reading  = %s", pPaper->szAuthors);
  printf("Reading  = %s", pPaper->szPaperTitle);
  printf("Reading  = %s", pPaper->szFileName);
  getchar();
  lseek(iFd, 0, SEEK_SET);

  
  //
  // Prepare entire node here and send, so that
  // server's job is just to add the node to his list
  // or
  // we can send chunk by chunk from client and
  // somehow collect all chunks in the same node and then
  // add the node to server list
  //
  while (1)
  {
    iRead = read(iFd, pPaper->ByteFileChunk, ARRAY_SIZE(pPaper->ByteFileChunk));
    if (-1 == iRead || 0 == iRead)
    {
      // some error
      // may be end of file
      break;
    }

    pPaper->lChunkSize = (long)iRead;
    piIndex = senddetails_1(pPaper, pClient);
    if (NULL == piIndex)
    {
      printf("fail\n");
    }
    else
    {
      printf("Index = %d, Chunk Size = %d\n", *piIndex, iRead);
      pPaper->iPaperNo = *piIndex;
    }

    //printf("We are at %ld", lseek(iFd, 0, SEEK_CUR));
    //getchar();
    ++pPaper->iChunkIndex;
  }

  pPaper->iChunkIndex = -1;
  piIndex = senddetails_1(pPaper, pClient);
  if (NULL == piIndex)
   printf("fail last\n");
  else
    printf("LAST content\n");

  close(iFd);

  return 0;
}

int FetchDetailsInternal(int *piIndex, CLIENT *pClient)
{
  int iLoop;
  PAPER *pPaper;
  char szLogBuffer[MAX_BUF];

  int iChunkIndex = 0;
  long lRemainingSize = -1;

  FILEINFO FileInfo;
  memset(&FileInfo, 0, sizeof(FileInfo));
  FileInfo.iPaperIndex = *piIndex;

  while (1)
  {
    //
    // By default, client asks for MAX_BUF bytes each time
    //
    FileInfo.iChunkIndex = iChunkIndex;
    FileInfo.lRemainingSize = lRemainingSize;

    pPaper = fetchdetails_1(&FileInfo, pClient);

    if (NULL == pPaper)
    {
      printf("fail\n");
    }
    else
    {
      sprintf(szLogBuffer, "lChunkSize = %ld", pPaper->lChunkSize);
      Log(CLIENT_LOG_FILE, szLogBuffer, "FetchDetailsInternal", LOGLEVEL_INFO, NULL);

      for (iLoop = 0; iLoop < pPaper->lChunkSize; ++iLoop)
        printf("%c", pPaper->ByteFileChunk[iLoop]);

      sleep(5);
      if (0 == iChunkIndex)
      {
        lRemainingSize = pPaper->lFileSize - pPaper->lChunkSize;
      }
      else
      {
        lRemainingSize -= pPaper->lChunkSize;
      }

      if (0 >= lRemainingSize)
      {
        break;
      }
      ++iChunkIndex;
    }
  }
}


int FetchInfoInternal(int *piIndex, CLIENT *pClient)
{
  PAPER *pPaper;

  pPaper = fetchinfo_1(piIndex, pClient);

  if (NULL == pPaper)
  {
    printf("fail\n");
  }
  else
  {
    if (-1 != pPaper->iChunkIndex)
    {
      printf("%s\t%s\n", pPaper->szAuthors, pPaper->szPaperTitle);
    }
  }

  return 0;
}

int FetchListInternal(CLIENT *pClient)
{
  int iIndex;
  PAPER *pPaper;
  char szLogBuffer[MAX_BUF];

  //
  // Start asking records from 1
  // Note that this is NOT same as asking for Paper having index 1
  //
  // Ill:
  //	We have added 5 records (indexes: 1-5 given by server)
  //	Then we delete these 5
  //	Again if we add 	(server will give indexes 6,7)
  //	So we should list 6th PAPER record and
  //			  7th PAPER record
  //	Thus, Starting from index 1 means, 1st record in server's
  //	Linked list.
  //
  //	Ordering has to be maintained by client so as to have
  //	correct behavior, when running MULTIPLE clients CONCURRENTLY.
  //
  iIndex = 1;
  while (1)
  {
    //
    // Simply call the FetchInfo for each record
    //
    pPaper = fetchlist_1(&iIndex, pClient);

    if (NULL == pPaper)
    {
      printf("fail\n");
      break;
    }

    sleep(5);
    if (0 == pPaper->iPaperNo)
    {
      Log(CLIENT_LOG_FILE, "Got 0 PaperNo", "FetchListInternal", LOGLEVEL_INFO, NULL);
      break;
    }

    printf("%d\t%s\t%s\n", pPaper->iPaperNo, pPaper->szAuthors, pPaper->szPaperTitle);
    ++iIndex;
  }
}

int RemoveDetailsInternal(int *piIndex, CLIENT *pClient)
{
  int *piRet;

  if (NULL == piIndex)
  {
    return -1;
  }

  piRet = removedetails_1(piIndex, pClient);
  if (NULL == piRet)
  {
    printf("fail\n");
  }
  else if(-1 == *piRet)
  {
    printf("Paper record not found\n");
  }
  else
  {
    printf("Paper record deleted\n");
  }
}

void DisplayHelp()
{
  printf("\nPAPER STORAGE client Help\n");
}

int ValidateString(const char *pszString)
{
  size_t sLen;
  if (NULL == pszString)
  {
    return -1;
  }

  sLen = strlen(pszString);
  if (0 == sLen)
  {
    return -1;
  }

  // if (
  //     ('\'' != pszString[0] ) ||
  //     ('\'' != pszString[sLen-1])
  //     )
  // {
  //   printf("Why hre ?[ %c ] , [ %c ] ", pszString[0], pszString[sLen-1]);
  //   return -1;
  // }

  return 0;
}

int ProcessArgument(int argc, char *argv[], PAPER *pPaper)
{
  int iRet;
  int iLoop;

  if (1 == argc || 2 == argc)
  {
    return -1;
  }

  switch(argc)
  {
    case 3:
    {
      //
      //  This has to be a "list" command
      //
      if (0 != strcasecmp(argv[2], COMMAND_LIST_STR))
      {
        return -1;
      }

      return COMMAND_LIST_ID;
      break;
    }

    case 4:
    {
      //
      //  Commands having 4 arguments in total are:
      //  info, fetch, remove
      //
      for (iLoop = 0; iLoop < ARRAY_SIZE(Commands); ++iLoop)
      {
        if (
            (1 == Commands[iLoop].iNumParams) &&
            (0 == strcmp(Commands[iLoop].pszCommand, argv[2]))
            )
        {
            if (NULL == pPaper)
            {
              return -1;
            }

            iRet = atoi(argv[3]);
            if (0 == iRet)
            {
              return -1;
            }

            pPaper->iPaperNo = iRet;
            return Commands[iLoop].iCommandId;
        }
      }

      break;
    }


    case 6:
    {
      //
      //  This has to be add
      //
      if (0 != strcasecmp(argv[2], COMMAND_ADD_STR))
      {
        return -1;
      }

      if (NULL == pPaper)
      {
        return -1;
      }

      //
      //  Validate author's names
      //
      iRet = ValidateString(argv[3]);
      if (-1 == iRet)
      {
        printf("Error: Author name/s not valid\n<%s>\n",argv[3]);
        return -1;
      }
      strcpy(pPaper->szAuthors, argv[3]);

      //
      //  Validate paper title
      //
      iRet = ValidateString(argv[4]);
      if (-1 == iRet)
      {
        printf("Error: Paper title not valid\n<%s>\n",argv[4]);
        return -1;
      }
      strcpy(pPaper->szPaperTitle, argv[4]);

      //
      //  Check whether file exists
      //
      iRet = access(argv[5], F_OK);
      if (0 != iRet)
      {
        printf("Error: File does not exist\n<%s>\n",argv[5]);
        return -1;
      }
      strcpy(pPaper->szFileName, argv[5]);

      return COMMAND_ADD_ID;

      break;
    }

    default:
    {
      break;
    }
  }

  return -1;
}

int main(int argc, char *argv[])
{
  int iRet;
  int iCommandId;
  CLIENT *pClient;
  PAPER Paper;
  
  int iIndex;

  memset(&Paper, 0, sizeof(Paper));
  iCommandId = ProcessArgument(argc, argv, &Paper);
  if (-1 == iCommandId)
  {
    DisplayHelp();
    return 0;
  }

  pClient = clnt_create(argv[1], PAPER_STORAGE_SERVER, VERSION1, "tcp");
  if (NULL == pClient)
  {
    printf("Error: RPC Connection could not be established.\n");
    return 0;
  }

  switch(iCommandId)
  {
    case COMMAND_ADD_ID:
    {
      Paper.iPaperNo = -1;
      iRet = SendDetailsInternal(&Paper, pClient);
      if (-1 == iRet)
      {
        printf("Error: SendDetailsInternal failed.\n");
      }

      break;
    }

    case COMMAND_LIST_ID:
    {
      FetchListInternal(pClient);    
      break;
    }

    case COMMAND_FETCH_ID:
    {
      FetchDetailsInternal(&Paper.iPaperNo, pClient);
      break;
    }

    case COMMAND_INFO_ID:
    {
      FetchInfoInternal(&Paper.iPaperNo, pClient);
      break;
    }
    
    case COMMAND_REMOVE_ID:
    {
      RemoveDetailsInternal(&Paper.iPaperNo, pClient);
      break;
    }
  }

  return 0;
}
