import java.io.DataInputStream;
import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;

public class Connect_Thread extends Thread {
    private static final String SEND_MESSAGE = "SEND MESSAGE";
    private static final int maxSize = 256;

    private static String server; // server IP address
    private static ServerSocket serverSocket; // socket to listen to the server
    private static int maxSizeMsg = 256; // Maximum size of the message received by the user

    public Connect_Thread(String server, ServerSocket serverSocket){
        this.server = server;
        this.serverSocket = serverSocket;
    }

    public void run(){
        try {
            String operation; // the operation to be performed
            Socket clientSocket = serverSocket.accept(); // accept the connection
            while(!interrupted()){

                DataInputStream input  = new DataInputStream(clientSocket.getInputStream()); // buffer reader

                if((operation = receiveString(input)) == null) { // get the operation to be performed
                    System.out.println("Error reading the operation");
                    break;
                }
                if(operation.equals(SEND_MESSAGE)){ // SEND MESSAGE operation
                    if(!sendOperation(input)){ // Execute send message operation
                        System.out.println("Error while send message");
                        break;
                    }
                }
                else{
                    System.out.println("Wrong operation received");
                }
            }
        } catch (IOException e) {
            return;
        }
    }

    /**
     * @brief Receive a String from the server up to maxSize bytes
     *
     * @param input: the input channel to read the String sent by the server
     * @return the String read from the user. In case of any error null is returned
     */
    private String receiveString(DataInputStream input){
        byte[] msg = new byte[maxSize];
        try {
            if ( (input.read(msg, 0, maxSizeMsg)) < 0){ // read the String sent by the server
                System.out.println("Error reading the message");
            }
        } catch (IOException e) {
            return null;
        }
        return String.valueOf(msg);
    }

    /**
     * @brief Receives a message from the client
     *
     * @param input: the input channel to read the String sent by the server
     */
    private boolean sendOperation(DataInputStream input){
        String originUser, msgId, msg; // Strings to be received from the server
        if((originUser = receiveString(input)) == null){ // Read the originUser
            System.out.println("Error reading the origin user");
            return false;
        }
        else if((msgId = receiveString(input)) == null){ // Read the msgId
            System.out.println("Error reading the msgId");
            return false;
        }
        else if((msg = receiveString(input)) == null){ // Read the msg
            System.out.println("Error reading the msg");
            return false;
        }
        System.out.println("c> MESSAGE " + msgId + " FROM " + originUser + ":\n" + "   " + msg + "\n   END");
        return true;
    }

}