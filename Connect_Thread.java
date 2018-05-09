import java.io.DataInputStream;
import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;

public class Connect_Thread extends Thread {
    private static final int maxSize = 256;

    private static String server; // server IP address
    private static ServerSocket serverSocket; // socket to listen to the server

    public Connect_Thread(String server, ServerSocket serverSocket){
        this.server = server;
        this.serverSocket = serverSocket;
    }

    public void run(){
        try {
            Socket clientSocket = serverSocket.accept(); // accept the connection CREO QUE ESTO PETA
            while(!interrupted()){

                DataInputStream input  = new DataInputStream(clientSocket.getInputStream()); // buffer reader

                byte[] msg = new byte[maxSize];
                if ( (input.read(msg)) < 0){ // return the String received
                    System.out.println("Error reading the message");
                }

                System.out.print("Message received: " + msg);
            }
        } catch (IOException e) {
            return;
        }
    }
}