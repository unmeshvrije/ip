/*++

Module Name:
	common.c.

Abstract:
	This module implements the commonly used methods across the
	program - including server-side, client side and the related.

Revision History:
	Date    : Sep 27 2012.

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

#include "common.h"


/////////////////////////////////////////////////////////////////////
//	M A C R O S.
/////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////
//	G L O B A L S.
/////////////////////////////////////////////////////////////////////

FILE *pLogFile;


/////////////////////////////////////////////////////////////////////
//	S T R U C T U R E S
/////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////
//	F U N C T I O N  D E F I N T I O N S
/////////////////////////////////////////////////////////////////////

int InitLogging(char *szFilePath)
{

	pLogFile = fopen(szFilePath, "w+");
	return 0;
}

void DeInitLogging()
{
   fclose(pLogFile);
}

void Log(const char* szMsg, const char* szFuncName, int iLevel, int *iErrorInfo)
{
  char szFormat[100];
  
  if ( 0 == strcmp (szMsg, ""))
  {
    return;
  }

  switch(iLevel)
  {
	case LOGLEVEL_INFO : 
	    fprintf(pLogFile,"%s:\tInfo: %s\n", szFuncName, szMsg);
	    break;
	    
	case LOGLEVEL_WARNING : 
	    sprintf(szFormat, "%s:\tWarning: %s LastError:%d\n", szFuncName, szMsg, ((iErrorInfo==NULL)? 0: *iErrorInfo) );
	    perror(szFormat);
	    break;
	    
	case LOGLEVEL_FATAL_ERROR :
	    sprintf(szFormat, "%s:\tFatal Error: %s LastError:%d\n", szFuncName, szMsg, ((iErrorInfo==NULL)? 0: *iErrorInfo) );
	    EXIT_ON_ERROR(szFormat);
	    break;
	    
	default : 
	    // LOGLEVEL_ERROR
	    sprintf(szFormat,"%s:\tError: %s LastError:%d\n", szFuncName, szMsg, ((iErrorInfo==NULL)? 0: *iErrorInfo) ); //A valid lastError cannot be zero as per man page of errno. Hence using 0 when lastError is not specified.
	    //perror(szFormat)i;
	    fprintf(pLogFile, "%s", szFormat);
	    break;
  }
  return;
}
