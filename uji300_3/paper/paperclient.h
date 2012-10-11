/*++
 
Module Name: 	
	paperclient.h.
 
Abstract:
	This module writes internal data structures for client.
 
Revision History:
 
	Date:	Apr 01 2011.

	Author:	Unmesh Joshi.
		Koustubha Bhat

	Desc:	Created.
 
 --*/

#ifndef	_PAPERCLIENT_H_
#define	_PAPERCLIENT_H_


/////////////////////////////////////////////////////////////////////
//	H E A D E R S.
/////////////////////////////////////////////////////////////////////

#include "paper.h"
#include "../common.h"


/////////////////////////////////////////////////////////////////////
//	S T R U C T U R E S.
/////////////////////////////////////////////////////////////////////


#define	COMMAND_LIST_STR	"list"
#define	COMMAND_ADD_STR		"add"
#define	COMMAND_REMOVE_STR	"remove"
#define	COMMAND_INFO_STR	"info"
#define	COMMAND_FETCH_STR	"fetch"


#define	COMMAND_LIST_ID		1
#define	COMMAND_ADD_ID		2
#define	COMMAND_REMOVE_ID	3
#define	COMMAND_INFO_ID		4
#define	COMMAND_FETCH_ID	5


//typedef enum COMMAND

typedef struct tagCOMMAND
{
	int iCommandId;
	char *pszCommand;
	int iNumParams;

}	COMMAND, *P_COMMAND;


#endif	//_PAPERCLIENT_H_
