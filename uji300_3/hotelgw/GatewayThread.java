import java.io.*;
import java.net.*;
import java.util.StringTokenizer;
import java.rmi.Naming;
//import java..MalformedURLException;

class GatewayThread extends Thread
{
  private Socket sock;
  private static String strRMIServerHost = "localhost";

  public GatewayThread (Socket sock)
  {
    this.sock = sock;
  }

  private static String[] strValidCommands = { "list", "book", "guests" };

  private static boolean isValid (String strCommandLine)
  {
    StringTokenizer strTokenize = new StringTokenizer (strCommandLine, " ");

    if (strTokenize.hasMoreTokens ())
      {
	String strCommand = strTokenize.nextToken ();
	for (int i = 0; i < strValidCommands.length; ++i)
	  {
	    if (strCommand.equalsIgnoreCase (strValidCommands[i]))
	      {
		return true;
	      }
	  }
      }

    return false;
  }

  private static String getCommand (String strCommandLine)
  {
    String strCommand = "";
    StringTokenizer strTokenize = new StringTokenizer (strCommandLine, " ");

    if (strTokenize.hasMoreTokens ())
      {
	strCommand = strTokenize.nextToken ();
      }

    return strCommand;
  }

  private static RoomType getRoomType (String strCommandLine)
  {
    int iRoomType;
    StringTokenizer strTokenize = new StringTokenizer (strCommandLine, " ");

    if (strTokenize.hasMoreTokens ())
    {
	strTokenize.nextToken ();//Command
    }
    if (strTokenize.hasMoreTokens())
    {
      iRoomType = Integer.parseInt(strTokenize.nextToken());
      switch(iRoomType)
      {
         case 1: return RoomType.ONE;
	 case 2: return RoomType.TWO;
	 case 3: return RoomType.THREE;
      }
    }

    return RoomType.INVALID;
  }

  private static String getName (String strCommandLine)
  {
    String strName;
    StringBuffer strBuffer = new StringBuffer();
    StringTokenizer strTokenize = new StringTokenizer (strCommandLine, " ");

    if (strTokenize.hasMoreTokens ())
    {
      strTokenize.nextToken ();
      if (strTokenize.hasMoreTokens ())
      {
        strTokenize.nextToken ();
        while (strTokenize.hasMoreTokens())
        {
	  strBuffer.append(strTokenize.nextToken ());
	  strBuffer.append(" ");
        }
      }
    }

    strName = strBuffer.toString();
    return strName;
  }

  public void run ()
  {
    String strCommand = "";
    String strCommandLine = "";

    String strResult = "";
    String[]strArrayResult;

    BufferedReader in = null;
    PrintWriter out = null;
    try
    {
      in =  new BufferedReader (new InputStreamReader (sock.getInputStream ()));
      out = new PrintWriter (sock.getOutputStream (), true);

      boolean more = true;
      do
      {
	String temp = in.readLine ();
	if (temp == null)
	  more = false;
	else
	  strCommandLine = temp;

      }
      while (more);
    }
    catch(IOException ie)
    {
      ie.printStackTrace();
    }

    /*
       Process the command:
       1. Validate
       2. Send to Remote Object (RMI Hotel Server)
     */

    //
    // 1.Validate
    //
    if (!isValid (strCommandLine))
      {
	//
	// This '\0' is for 'C' client.
	//
	out.print ("Invalid command\n\0");
	out.close ();		// Flushes the buffer, it seems
      }
    else
      {
	//
	// RMI code here.
	//
	try
	{
	IReserver iReserve = (IReserver) Naming.lookup ("rmi://" + strRMIServerHost + "/HotelReserver");
	strCommand = getCommand (strCommandLine);

	if (strCommand.equalsIgnoreCase (strValidCommands[0]))	//list
	  {
	    strResult = iReserve.list ();
	  }
	else if (strCommand.equalsIgnoreCase (strValidCommands[2]))	//guests
	  {
	    strArrayResult = iReserve.guests ();
	    StringBuffer strBuffer = new StringBuffer ();
	    int i;
	    for (i = 0; (i < strArrayResult.length) && (null != strArrayResult[i]); ++i)
	      {
		strBuffer.append (strArrayResult[i]);
		if (strArrayResult[i].
		    charAt (strArrayResult[i].length () - 1) != '\n')
		  strBuffer.append ("\n");
		  
	      }

	    strResult = strBuffer.toString ();
	  }
	else			//book
	  {
	    RoomType room = getRoomType(strCommandLine);
	    String name = getName(strCommandLine);
	    boolean boRet = iReserve.book(room, name);
	    if (true == boRet)
	      strResult = "Booked!\n";
	    else
	      strResult = "Room not available\n";
	  }
	  }
	  catch(Exception e)
	  {
	    e.printStackTrace();
	  }

	out.print (strResult + "\0");
	out.close ();
      }

    //sock.close();
  }
}
