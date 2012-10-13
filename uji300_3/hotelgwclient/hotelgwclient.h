/*++
 
Module Name: 	
	hotelgwclient.h.
 
Abstract:
	This module writes internal data structures for hotel client.
 
Revision History:
 
	Date:	Oct 01 2012.

	Author:	Unmesh Joshi.
		Koustubha Bhat

	Desc:	Created.
 
 --*/

#ifndef	_HOTELGWCLIENT_H_
#define	_HOTELGWCLIENT_H_


/////////////////////////////////////////////////////////////////////
//	H E A D E R S.
/////////////////////////////////////////////////////////////////////

#include "../common.h"


/////////////////////////////////////////////////////////////////////
//	S T R U C T U R E S.
/////////////////////////////////////////////////////////////////////


#define	COMMAND_LIST_STR	"list"
#define	COMMAND_BOOK_STR		"book"
#define	COMMAND_GUESTS_STR	"guests"


#define	COMMAND_LIST_ID		1
#define	COMMAND_BOOK_ID		2
#define	COMMAND_GUESTS_ID	3


//typedef enum COMMAND

typedef struct tagCOMMAND
{
	int iCommandId;
	char *pszCommand;
	int iNumParams;

}	COMMAND, *P_COMMAND;


#endif	//_HOTELGWCLIENT_H_
