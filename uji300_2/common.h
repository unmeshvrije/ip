/*++

Module Name:
	common.h.

Abstract:
	Store common headers here.

Revision History:
	Date    : Sep 26 2012.

	Author  : Unmesh Joshi (S-2515965).
		: Koustubha Bhat (S-2516144).

	VUnetID : uji300
		: kbt350

	Desc    : Created.
	
--*/


/////////////////////////////////////////////////////////////////////
//	H E A D E R S.
/////////////////////////////////////////////////////////////////////

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>	//For strcmp


/////////////////////////////////////////////////////////////////////
//	M A C R O S.
/////////////////////////////////////////////////////////////////////

#define	EXIT_ON_ERROR(msg)\
			do{fprintf(pLogFile, "%s", msg); exit(EXIT_FAILURE);} while(0);
			//perror(msg); exit(EXIT_FAILURE);}while(0);

#define	MAX_ARGC	10
#define	CMD_LEN		80
#define	MAX_BUF		1024

#define LOGLEVEL_INFO 1
#define LOGLEVEL_WARNING 2
#define LOGLEVEL_ERROR 3
#define LOGLEVEL_FATAL_ERROR 4


int InitLogging(char *);
void DeInitLogging();
void Log(const char* szMsg, const char* szFuncName, int iLevel, int *iErrorInfo);


////////E O F////////////////////////////////////////////////////////
