import java.rmi.Remote;


public interface IReserver extends Remote {

	// Lists all the rooms along with the room type and price information 
	public String list() throws java.rmi.RemoteException;
	
	// Books a room given the kind of room and the guest name
	public boolean book(int type, String guestName) throws java.rmi.RemoteException;
	
	// Gets a list of guests who have been allocated a room for the event
	public String[] guests() throws java.rmi.RemoteException;	

}
