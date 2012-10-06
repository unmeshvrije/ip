
#include "chat.h"
#include "common.h"

WINDOW *CreateWindow(int height, int width, int starty, int startx);
void DestroyWindow(WINDOW *local_win);

static WINDOW *winMain;
static WINDOW *winTitle;

int InitWindows(TALKINFO *pTalkInfoMe, TALKINFO *pTalkInfoOther)
{
	WINDOW *winMe, *winOther;

	int startx, starty, width, height;

	int  iRet;
	int iLenWelcome;
	char szWelcome[] = "*** KUUK Chat Application ***";

	winMain = initscr();			/* Start curses mode 		*/
	nocbreak();			/* Line buffering disabled, Pass on
					/ * everty thing to me 		*/
	//keypad(stdscr, TRUE);		/* I need that nifty F1 	*/

	height = LINES-1;
	width = COLS-2;
	starty = 1;
	startx = 1;
	winMain = CreateWindow(height, width, starty, startx);

	height = 8;
	width = COLS - 8;
	starty = 2;
	startx = 4;
	winTitle = CreateWindow(height, width, starty, startx);

	iLenWelcome = strlen(szWelcome);
	mvwprintw(winTitle, (8 / 2), ((width - 4) / 2) - (iLenWelcome / 2), szWelcome);
	wrefresh(winTitle);

	pTalkInfoMe->pWin->iHeight = (LINES - 4) - 10; 
	pTalkInfoMe->pWin->iWidth = (COLS / 2) - 2 - 4;
	pTalkInfoMe->pWin->iY= 11;
	pTalkInfoMe->pWin->iX= 4;
	winMe = CreateWindow(pTalkInfoMe->pWin->iHeight, pTalkInfoMe->pWin->iWidth, pTalkInfoMe->pWin->iY, pTalkInfoMe->pWin->iX);
	if (NULL == winMe)
	{
	   return -1;
	}
	
	pTalkInfoOther->pWin->iHeight = (LINES - 4) - 10; 
	pTalkInfoOther->pWin->iWidth = (COLS / 2) - 2 - 4;
	pTalkInfoOther->pWin->iY = 11;
	pTalkInfoOther->pWin->iX = (pTalkInfoMe->pWin->iX + pTalkInfoMe->pWin->iWidth) + 5;
	winOther = CreateWindow(pTalkInfoOther->pWin->iHeight, pTalkInfoOther->pWin->iWidth, pTalkInfoOther->pWin->iY, pTalkInfoOther->pWin->iX);
	if (NULL == winOther)
	{
	  return -1;
	}

	pTalkInfoMe->pWindow = winMe;
	pTalkInfoOther->pWindow = winOther;

	iRet = scrollok(winMe, TRUE);
	if (ERR == iRet)
	{
	  return -1;
	}

	iRet = scrollok(winOther, TRUE);
	if (ERR == iRet)
	{
	  return -1;
	}

	iRet = idlok(winMe, TRUE);
	if (ERR == iRet)
	{
	  return -1;
	}

	iRet = idlok(winOther, TRUE);
	if (ERR == iRet)
	{
	  return -1;
	}

	return 0;
}


void DeInitWindows(TALKINFO *pTalkInfoMe, TALKINFO *pTalkInfoOther)
{
  DestroyWindow(pTalkInfoMe->pWindow);
  DestroyWindow(pTalkInfoOther->pWindow);
  DestroyWindow(winMain);
  DestroyWindow(winTitle);
  Log("Calling endwin", "DeInitWindows", LOGLEVEL_INFO, NULL);
	endwin();			/* End curses mode		  */
}


WINDOW *CreateWindow(int height, int width, int starty, int startx)
{
	WINDOW *local_win;

	local_win = newwin(height, width, starty, startx);
	box(local_win, 0 , 0);		/* 0, 0 gives default characters 
					 * for the vertical and horizontal
					 * lines			*/
	wrefresh(local_win);		/* Show that box 		*/

	return local_win;
}

void DestroyWindow(WINDOW *local_win)
{	
	wborder(local_win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
	/* The parameters taken are 
	 * 1. win: the window on which to operate
	 * 2. ls: character to be used for the left side of the window 
	 * 3. rs: character to be used for the right side of the window 
	 * 4. ts: character to be used for the top side of the window 
	 * 5. bs: character to be used for the bottom side of the window 
	 * 6. tl: character to be used for the top left corner of the window 
	 * 7. tr: character to be used for the top right corner of the window 
	 * 8. bl: character to be used for the bottom left corner of the window 
	 * 9. br: character to be used for the bottom right corner of the window
	 */
	wrefresh(local_win);
	delwin(local_win);
}
