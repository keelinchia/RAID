import java.net.*;
import java.io.*;
import java.nio.*;

/*
class AgentMonitor extends Thread 
{ 
    public void run() 
    { 
        try
        { 
  
        } 
        catch (Exception e) 
        { 
            // Throwing an exception 
            System.out.println ("Exception is caught"); 
        } 
    } 
}
*/

class BeaconListener implements Runnable 
{ 
    public void run() 
    { 
        try
	    {	
		// Receive UDP from BeaconSender
		byte[] buffer = new byte[1024]; 
		DatagramPacket incoming = new DatagramPacket(buffer, buffer.length);
		DatagramSocket ds = new DatagramSocket(53953); 
		ds.receive(incoming); 
		byte[] data = incoming.getData(); 

		System.out.println("Packet received.\n"); 

		/* Print out the data in correct formats. */
		System.out.println("Binary array of data: ");
		printBinaryArray(data, "");
		System.out.println("\n");

		byte[] arr1 = new byte[4];
	
		arr1[0] = data[0];
		arr1[1] = data[1];
		arr1[2] = data[2];
		arr1[3] = data[3];

		ByteBuffer wrapped1 = ByteBuffer.wrap(arr1);
		int id = wrapped1.getInt();
		
		System.out.println("Agent ID: " + id);


		byte[] arr2 = new byte[4];
		arr2[0] = data[4];
		arr2[1] = data[5];
		arr2[2] = data[6];
		arr2[3] = data[7];

		ByteBuffer wrapped2 = ByteBuffer.wrap(arr2);
		int time = wrapped2.getInt();

		System.out.println("Start Up Time: " + time);


		byte[] arr3 = new byte[4];
		arr3[0] = data[8];
		arr3[1] = data[9];
		arr3[2] = data[10];
		arr3[3] = data[11];

		/* Convert IP to string then print the result. */
		System.out.println("IP Address: " + getIpAddress(arr3));


		byte[] arr4 = new byte[4];
		arr4[0] = data[12];
		arr4[1] = data[13];
		arr4[2] = data[14];
		arr4[3] = data[15];

		ByteBuffer wrapped3 = ByteBuffer.wrap(arr4);
		int port = wrapped3.getInt();
		
		System.out.println("Cmd Port: " + port);
		
		//System.out.println("Port" +  incoming.getPort()  + 
		//		   " on "  +  incoming.getAddress()  + 
		//		   " sent this message:"); 
		
		// convert the binary bytes to string
		//String ret = new String(buf);
		
		// should be all upcases now
		//System.out.println(ret);
	    }

        catch (Exception e) 
	    { 
		// Throwing an exception 
		System.out.println ("Exception is caught"); 
	    }
    }
    
    static void printBinaryArray(byte[] b, String comment) {
	System.out.println(comment);
	for (int i=0; i<b.length; i++)
	    {
		System.out.print(b[i] + " ");
	    }
	System.out.println();
    }
    
    static private byte[] toBytes(int i)
    {
	byte[] result = new byte[4];
	result[0] = (byte) ((i >> 24) & 0xFF);
	result[1] = (byte) ((i >> 16) & 0xFF);
	result[2] = (byte) ((i >> 8) & 0xFF);
	result[3] = (byte) (i & 0xFF);

	return result;
    }
   
    static private int toInteger32(byte[] bytes)
    {
        int tmp = (bytes[0] << 24) +
	    (bytes[1] << 16) +
	    (bytes[2] << 8) +
	    bytes[3];

        return tmp;
    }

    private static String getIpAddress(byte[] rawBytes) {
        int i = 4;
        StringBuilder ipAddress = new StringBuilder();
        for (byte raw : rawBytes) {
            ipAddress.append(raw & 0xFF);
            if (--i > 0) {
                ipAddress.append(".");
            }
        }
        return ipAddress.toString();
    }
}

/* 
class ClientAgent implements Runnable 
{ 
    public void run() 
    { 
        try
        { 
        } 
        catch (Exception e) 
        { 
            // Throwing an exception 
            System.out.println ("Exception is caught"); 
        } 
    } 
} 
*/

public class manager  
{
    public static void main(String args[]) throws IOException
    {
	Thread bListener = new Thread (new BeaconListener());
	bListener.start();
    }   

}



	

 
