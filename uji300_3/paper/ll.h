/*++

Module Name:
	ll.h.

Abstract:
	This module writes linked list functions.

Revision History:
	Date:	Oct 08 2012.
	Author:	Unmesh Joshi.
	Desc:	Created.

--*/


#ifndef	_LL_H_
#define	_LL_H_


/////////////////////////////////////////////////////////////////////
//	H E A D E R S.
/////////////////////////////////////////////////////////////////////

#include "paperserver_proc.h"


/////////////////////////////////////////////////////////////////////
//	M A C R O S.
/////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////
//	C O N S T A N T S.
/////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////
//	S T R U C T U R E S.
/////////////////////////////////////////////////////////////////////

//
//	Sructure description.
//
typedef struct tagSERVER_PAPER_LIST
{
	SERVER_PAPER			Data;
	struct tagSERVER_PAPER_LIST	*pNext;

} SERVER_PAPER_LIST, *P_SERVER_PAPER_LIST;


/////////////////////////////////////////////////////////////////////
//	C L A S S E S.
/////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////
//	E X T E R N  V A R I A B L E S.
/////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////
//	P R O T O T Y P E S.
/////////////////////////////////////////////////////////////////////




//*******************************************************************
//
//	Function:
//		AddToList.
//
//	Parameters:
//
//	Description:
//		Linked list append function.
//
//	Returns:
//
//*******************************************************************
int
AddToList(
	SERVER_PAPER *pNodeData,
	SERVER_PAPER_LIST **ppListHead,
	SERVER_PAPER_LIST **ppListTail
	);

//*******************************************************************
//
//	Function:
//		AppendToList.
//
//	Parameters:
//
//	Description:
//
//	Returns:
//
//*******************************************************************
int
AppendToGenList(
	SERVER_PAPER_LIST **ppListToBeAppended,
	SERVER_PAPER_LIST **ppListHead,
	SERVER_PAPER_LIST **ppListTail
	);


//*******************************************************************
//
//	Function:
//		FreeList.
//
//	Parameters:
//
//	Description:
//		This function frees the list.
//
//	Returns:
//
//*******************************************************************
int
FreeList(
	SERVER_PAPER_LIST *pListHead
	);


SERVER_PAPER_LIST * GetNodeFromList(SERVER_PAPER_LIST *pHead, int);
int RemoveFromList(int iIndex, SERVER_PAPER_LIST **ppListHead,	SERVER_PAPER_LIST **ppListTail);

#endif	//_LL_H_
