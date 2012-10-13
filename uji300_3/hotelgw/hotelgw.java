import java.lang.*;
import java.io.*;
import java.net.*;
import java.util.StringTokenizer;

/*
Q.C
Server implementation should be in thread
One thread per client.. {can be handled independently}

In case of book: contention is possible.

Q.D

Client-server protocol should be
1. Client requests Gateway (sends command)
2. Client waits till Gateway responds

Gateway will spawn a thread for every single client request.
And each thread will
1. Validate command
2. Execute if valid
   2.1	Send command to RMI server
   2.2	Wait till RMI server respondes (hidden)
   2.3	Return the result (returned from RMI server) to client
*/


class HotelGateway 
{
   //private long serverSocket = 9999;

//   private static string strRMIServerHost = "localhost";

   public static void main(String args[])
   {
      String data = "Toobie ornaught toobieJust like that\0";
      try 
      {
         String strCommand;
         String strCommandLine ="";
         ServerSocket srvr = new ServerSocket(9999);
         Socket skt;

         while (true)
         {
            skt = srvr.accept();
	    Thread thread = new GatewayThread(skt);
	    thread.start();
         }

         //srvr.close();
      }
      catch(Exception e)
      {
         e.printStackTrace(); 
      }
   }
}
