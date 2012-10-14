/*++

Module Name:
        HotelReservationClient.java

Abstract:
        This module implements the client for working with the hotel reservation server.

Revision History:
        Date    : Oct 12 2012.

        Author  : Unmesh Joshi (S-2515965).
                : Koustubha Bhat (S-2516144).

        VUnetID : uji300
                : kbt350

        Desc    : Created.
        
--*/

import java.net.MalformedURLException;
import java.rmi.Naming;

public class HotelReservationClient {
	
	private enum Command
	{
		INVALID, LIST, BOOK, GUESTS;
	}
	
	static String argServerName;
	static Command operation;
	static String[] operationArgs;
	static int iRoomType;
	static int mandatoryArgLength;
	static int optionalArgLength;
	
	private static boolean cmdLineValidateAndSet(String[] args)
	{
		argServerName = "";
		operation = Command.INVALID;
		operationArgs = new String[2];			
		mandatoryArgLength = 2;
		optionalArgLength = 2;
		
		if (args.length < mandatoryArgLength )
		{
			return false;
		}
		
		argServerName = args[0];
	
		if (args[1].equalsIgnoreCase("list"))
		{
			operation = Command.LIST;
			return true;
		}
		else if (args[1].equalsIgnoreCase("guests"))
		{
			operation = Command.GUESTS;
			return true;
		}
		else if (args[1].equalsIgnoreCase("book"))
		{
			if (args.length < (mandatoryArgLength + optionalArgLength))
			{
				return false;
			}
			operation = Command.BOOK;

			try
			{
				iRoomType = Integer.parseInt(args[2]);
			}
			catch(NumberFormatException ne)
			{
				return false;
			}

			if ((iRoomType <= 0) || (iRoomType > 3))
			{
				return false;
			}
			
			operationArgs[0] = args[3];
			// Guest's name, which should support being space separated
			for(int i=4; i<args.length; i++)
			{
				operationArgs[0] += " " + args[i];
			}
			return true;
		}
		return false;
	}

	private static void displayUsage()
	{
		System.out.println("\nUsage: HotelReservationClient <server_name> <command> [<arguments>]");
		System.out.println("Commands");
		System.out.println("\tlist\t- lists the availability of rooms for first, second and third type rooms");
		System.out.println("\tbook\t- books the specified <kind of room> for the particular <guest>. e.g.: book 1 John");
		System.out.println("\tguests\t- lists the guests who have been allocated rooms already.\n");
	}

	public static void main(String[] args)
	{
		try
		{
			IReserver reservationClient = (IReserver)Naming.lookup("rmi://localhost/HotelReserver");

			if (false == cmdLineValidateAndSet(args))
			{
				displayUsage();
				return;
			}
			
			switch (operation)
			{
			
			case LIST:
				System.out.print(reservationClient.list());
				break;
			
			case BOOK:
				if (true == reservationClient.book(iRoomType, operationArgs[1]))
				{
					System.out.println("Booking successful.");
				}
				break;
			
			case GUESTS:
				String[] guests = reservationClient.guests();
				for(int i=0; (i < guests.length) && (guests[i]!= null); i++)
				{
					System.out.println(guests[i]);
				}
				break;
			}
			
		}
		catch (Exception e)
		{
			System.out.println("Exception caught in the main() of client program.");
			e.printStackTrace();
		}
	}
}
