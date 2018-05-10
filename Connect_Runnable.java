import java.io.*;
import java.net.*;

public class Connect_Runnable implements Runnable {
    private static final int maxSize = 256;

    private static String server; // server IP address
    private static int portNumber; // port number of the thread to listen to the server

    public Connect_Runnable(String server, int portNumber){
        this.server = server;
        this.portNumber = portNumber;
    }

    public void run(){
        try {
            ServerSocket serverSocket = new ServerSocket(portNumber); // socket to listen to the server
            Socket clientSocket = serverSocket.accept(); // accept the connection
            while(true){
                DataInputStream input  = new DataInputStream(clientSocket.getInputStream()); // buffer reader

                byte[] msg = new byte[maxSize];
                if ( (input.read(msg)) < 0){ // return the String received
                    System.out.println("Error reading the message");
                }

                System.out.print("Message received: " + msg);
            }

        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}