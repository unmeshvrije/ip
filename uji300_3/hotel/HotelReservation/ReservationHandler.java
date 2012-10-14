/*++

Module Name:
        ReservationHandler.java

Abstract:
        This module is the worker object that the server implementation calls into, to do the actual work.

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

public class ReservationHandler {

	private LinkedList<BookingRecord> recordStore;
	private Room[] rooms;
	private int totalRooms=50;
	private int classOneRooms_start=0, classOneRooms_end=9;
	private int classTwoRooms_start=10, classTwoRooms_end=29;
	private int classThreeRooms_start=30;
	
	public ReservationHandler()
	{
		recordStore = new LinkedList<BookingRecord>();
		rooms = new Room[totalRooms];
		
		for (int i=classOneRooms_start; i<= classOneRooms_end; i++)
		{
			rooms[i] = new Room(RoomType.ONE, 150.0);
		}
		
		for (int i=classTwoRooms_start; i<= classTwoRooms_end; i++)
		{
			rooms[i] = new Room(RoomType.TWO, 120.0);
		}
		
		for (int i=classThreeRooms_start; i< totalRooms; i++)
		{
			//remaining ones are classThree rooms.
			rooms[i] = new Room(RoomType.THREE, 100.0);
		}
		
	}
	
	// Returns a list of available rooms
	public LinkedList<Room> listRooms()
	{
		LinkedList<Room> availableRooms = new LinkedList<Room>();
		
		for(int i=0; i<totalRooms; i++)
		{
			if (rooms[i].isAvailable())
			{
				availableRooms.add(rooms[i]);
			}
		}
		return availableRooms;
	}
	
	
	// Doesn't validate if the guest name is already present in the record or not
	// So, one guest CAN book more than one rooms!
	public synchronized boolean bookRoom(RoomType roomType, String guestName)
	{
		if ("" == guestName)
		{
			return false;
		}
		
		switch (roomType)
		{
			case ONE: 
				for (int i=classOneRooms_start; i<=classOneRooms_end; i++)
				{
					if (rooms[i].isAvailable())
					{
						rooms[i].allocate();
						recordStore.add(new BookingRecord(guestName, rooms[i]));
						return true;
					}
				}
				return false;
				
			case TWO:
				for (int i=classTwoRooms_start; i<=classTwoRooms_end; i++)
				{
					if (rooms[i].isAvailable())
					{
						rooms[i].allocate();
						recordStore.add(new BookingRecord(guestName, rooms[i]));
						return true;
					}
				}
				return false;
				
			case THREE:
				for (int i=classThreeRooms_start; i<totalRooms; i++)
				{
					if (rooms[i].isAvailable())
					{
						rooms[i].allocate();
						recordStore.add(new BookingRecord(guestName, rooms[i]));
						return true;
					}
				}
				return false;
			
			default:
				return false;
		} // switch case ends
		
	}

	// Lists all the guest names who have been allocated rooms in the hotel.
	public String[] listAllocatedGuests()
	{
		Iterator<BookingRecord> iBookingRecord = recordStore.iterator();
		BookingRecord bR;
		String[] guests = new String[totalRooms];		
		
		for(int i=0; (i < totalRooms) && iBookingRecord.hasNext(); i++)
		{
			bR = iBookingRecord.next();
			guests[i] = bR.guestName;
		}
		
		return guests;
	}
}

