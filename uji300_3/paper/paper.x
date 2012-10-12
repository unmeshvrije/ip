/*++

Module Name:
	paper.x.

Abstract:
	This is Interface Definition Language(IDL) file for common
	data structures needed by paper server and client

Revision History:
	Date    : Oct 06 2012.

	Author  : Unmesh Joshi (S-2515965).
		: Koustubha Bhat (S-2516144).

	VUnetID : uji300
		: kbt350

	Desc    : Created.
	
--*/


const MAX_BUF 		= 1024;
const MAX_AUTHORS	= 5;
const MAX_FILE_NAME	= 256;
const MAX_PAPER_NAME	= 200;
const MAX_AUTHOR_NAME	= 50;


struct PAPER
{
  int		iPaperNo;
  /*char		szAuthors[MAX_AUTHORS][MAX_AUTHOR_NAME];*/
  char		szAuthors[MAX_AUTHOR_NAME];
  char		szPaperTitle[MAX_PAPER_NAME];
  char		szFileName[MAX_FILE_NAME];
  long		lFileSize;
  opaque	ByteFileChunk[MAX_BUF];
  int		iChunkIndex;
  long		lChunkSize;
};

typedef struct PAPER PAPER;

struct FILEINFO
{
  int iPaperIndex;
  int iChunkIndex;
  long lRemainingSize;
};

typedef struct FILEINFO FILEINFO;

program PAPER_STORAGE_SERVER
{
  version VERSION1
  {
    int SendDetails(PAPER *) = 1;
    PAPER FetchInfo(int) = 2; /* I wonder this declares function which returns PAPER* after rpcgen */
    PAPER FetchDetails(FILEINFO *) = 3;
    PAPER FetchList(int) = 4;
    int RemoveDetails(int) = 5;

  } = 1;

} = 0x20000001;
