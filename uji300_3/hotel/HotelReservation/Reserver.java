/*++

Module Name:
        Reserver.java

Abstract:
        This module has the server side implementation of IReserver interface.

Revision History:
        Date    : Oct 12 2012.

        Author  : Unmesh Joshi (S-2515965).
                : Koustubha Bhat (S-2516144).

        VUnetID : uji300
                : kbt350

        Desc    : Created.
        
--*/


import java.util.Iterator;
import java.util.LinkedList;

public class Reserver extends java.rmi.server.UnicastRemoteObject implements IReserver{

		private ReservationHandler resvHandler;
		
		public Reserver() throws java.rmi.RemoteException
		{
			super();
			resvHandler = new ReservationHandler();
		}
		
		// Lists all the rooms along with the room type and price information 
		public String list() throws java.rmi.RemoteException
		{
			LinkedList<Room> roomsList = resvHandler.listRooms();
			Iterator<Room> roomIterator = roomsList.iterator();
			Room room;
			int classOne=0, classTwo=0, classThree=0;
			
			while(roomIterator.hasNext())
			{
				room = roomIterator.next();

				switch (room.getType())
				{
				case ONE:
					classOne++;
					break;
				case TWO:
					classTwo++;
					break;
				case THREE:
					classThree++;
					break;
				}
			}
			
			return classOne + "\t" + classTwo + "\t" + classThree + "\n";
		}
		
		// Books a room given the kind of room and the guest name
		public boolean book(int type, String guestName) throws java.rmi.RemoteException
		{
			RoomType roomType;
			switch(type)
			{
				case 1: roomType = RoomType.ONE; break;
				case 2: roomType = RoomType.TWO; break;
				case 3: roomType = RoomType.THREE; break;
				default: roomType = RoomType.INVALID; break;
			}
			
			return resvHandler.bookRoom(roomType, guestName);
		}
		
		// Gets a list of guests who have been allocated a room for the event
		public String[] guests() throws java.rmi.RemoteException
		{
			return resvHandler.listAllocatedGuests();
		}
}
