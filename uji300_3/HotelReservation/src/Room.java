/*++

Module Name:
        Room.java

Abstract:
        This class represents a room in the hotel's booking record.

Revision History:
        Date    : Oct 12 2012.

        Author  : Unmesh Joshi (S-2515965).
                : Koustubha Bhat (S-2516144).

        VUnetID : uji300
                : kbt350

        Desc    : Created.
        
--*/

public class Room {
	private RoomType type;
	private double price;
	private boolean available;
	
	public Room(RoomType type, double price)
	{
		this.type = type;
		this.price = price;
		this.available = true;
	}
	
	public RoomType getType()
	{
		return type;
	}
	
	public double getPrice()
	{
		return price;
	}
	
	public synchronized boolean isAvailable()
	{
		return available;
	}
	
	public synchronized void allocate()
	{
		this.available = false;
	}
}
