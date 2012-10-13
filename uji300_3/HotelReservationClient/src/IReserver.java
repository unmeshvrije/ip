/*++

Module Name:
        IReserver.java

Abstract:
        This is the interface that summarizes the hotel reservation server's functionalities.

Revision History:
        Date    : Oct 12 2012.

        Author  : Unmesh Joshi (S-2515965).
                : Koustubha Bhat (S-2516144).

        VUnetID : uji300
                : kbt350

        Desc    : Created.
        
--*/

import java.rmi.Remote;


public interface IReserver extends Remote {

	// Lists all the rooms along with the room type and price information 
	public String list() throws java.rmi.RemoteException;
	
	// Books a room given the kind of room and the guest name
	public boolean book(RoomType type, String guestName) throws java.rmi.RemoteException;
	
	// Gets a list of guests who have been allocated a room for the event
	public String[] guests() throws java.rmi.RemoteException;	

}
