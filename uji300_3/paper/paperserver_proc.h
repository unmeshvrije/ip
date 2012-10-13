/*++
 
Module Name: 	
	paperserver_proc.h.
 
Abstract:
	This module writes common functions.
 
Revision History:
 
	Date:	Apr 01 2011.

	Author:	Unmesh Joshi.
		Koustubha Bhat

	Desc:	Created.
 
 --*/

#ifndef	_PAPERSERVER_PROC_H_
#define	_PAPERSERVER_PROC_H_


/////////////////////////////////////////////////////////////////////
//	H E A D E R S.
/////////////////////////////////////////////////////////////////////

#include "paper.h"
#include "../common.h"


/////////////////////////////////////////////////////////////////////
//	S T R U C T U R E S.
/////////////////////////////////////////////////////////////////////

//
// Server will maintain papers in this structure 
//
typedef struct tagSERVER_PAPER
{
  int 		iPaperIndex;
  char		szAuthors[MAX_AUTHOR_NAME];
  char		szPaperTitle[MAX_PAPER_NAME];
  BYTE*		pFileData;
  long		lFileSize;

} SERVER_PAPER, *P_SERVER_PAPER;


#endif	//_PAPERSERVER_PROC_H_
