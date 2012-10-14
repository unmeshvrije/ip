import java.rmi.Naming;

public class HotelReservationServer {
	
	public HotelReservationServer()
	{
		try
		{
			IReserver hotelReserver = new Reserver();
			Naming.rebind("rmi://localhost/HotelReserver", hotelReserver);
		}
		catch (Exception e)
		{
			System.out.println("Error in initializing the hotel reserver.");
			e.printStackTrace();
		}
	}
	
	public static void main(String args[])
	{
		new HotelReservationServer();
	}

}
