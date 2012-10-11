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


/////////////////////////////////////////////////////////////////////
//	H E A D E R S.
/////////////////////////////////////////////////////////////////////

#include "ll.h"


/////////////////////////////////////////////////////////////////////
//	M A C R O S.
/////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////
//	C O N S T A N T S.
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
	)
{
	SERVER_PAPER_LIST *pNewNode;

	if (NULL == pNodeData || NULL == ppListHead || NULL == ppListTail)
	{
		return -1;
	}

	//
	//	Allocate new node.
	//
	pNewNode = (SERVER_PAPER_LIST *)malloc(sizeof(SERVER_PAPER_LIST));
	if (NULL == pNewNode)
	{
		return -1;
	}

	//
	//	Fill the node with data.
	//
	memset(pNewNode, 0, sizeof(SERVER_PAPER_LIST));
	pNewNode->pNext = NULL;
	pNewNode->Data = *pNodeData;

	//
	//	Append.
	//
	if (NULL == *ppListHead)
	{
		*ppListHead = *ppListTail = pNewNode;
	}
	else
	{
		(*ppListTail)->pNext = pNewNode;
		*ppListTail = pNewNode;
	}

	printf("Added");
	return 0;
}


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
	)
{
	SERVER_PAPER_LIST *pNav;
	SERVER_PAPER_LIST *pPrev;

	if (NULL == ppListToBeAppended || NULL == ppListHead || NULL == ppListTail)
	{
		return FALSE;
	}

	if (NULL == *ppListHead)
	{
		*ppListHead = *ppListTail = *ppListToBeAppended;
	}
	else
	{
		(*ppListTail)->pNext = *ppListToBeAppended;
	}

	pNav = *ppListTail;
	pPrev = *ppListTail;
	while (NULL != pNav)
	{
		pPrev = pNav;
		pNav = pNav->pNext;
	}

	*ppListTail = pPrev;
	return TRUE;
}


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
	)
{
	SERVER_PAPER_LIST *pTemp;
	SERVER_PAPER_LIST *pNavigate;

	if (NULL == pListHead)
	{
		return FALSE;
	}

	pNavigate = pListHead;
	while (NULL != pNavigate)
	{
		pTemp = pNavigate->pNext;
		free(pNavigate);
		pNavigate = pTemp;
	}

	return TRUE;
}


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
int RemoveFromList(int iIndex,	SERVER_PAPER_LIST **ppListHead,	SERVER_PAPER_LIST **ppListTail	)
{
	char chFound;
	SERVER_PAPER_LIST *pFollow;
	SERVER_PAPER_LIST *pNavigate;

	if (NULL == ppListHead || NULL == ppListTail)
	{
		  return -1;
	}

	chFound = 'n';
	pFollow = *ppListHead;
	pNavigate = *ppListHead;
	while (NULL != pNavigate)
	{
	    if (pNavigate->Data.iPaperIndex == iIndex)
	    {
	      if (*ppListHead == pNavigate)
	      {
			//
			//  Deleting first node: Adjust head
			//
			*ppListHead = (*ppListHead)->pNext;
	      }
	      else if (*ppListTail == pNavigate)
	      {
	        //
			// Deleting last node: Adjust tail
			//
			*ppListTail = pFollow;
	      }
	      else
	      {
			pFollow->pNext = pNavigate->pNext;
	      }
	      
	      chFound = 'y';
	      free(pNavigate);
	      break;
	    }
	    
	    pFollow = pNavigate;
	    pNavigate = pNavigate->pNext;
	}

	if ('n' == chFound)
		return -1;

	return 0;
}


