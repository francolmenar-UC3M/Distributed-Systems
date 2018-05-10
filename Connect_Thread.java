import java.io.DataInputStream;
import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;

public class Connect_Thread extends Thread {
    private static final String SEND_MESSAGE = "SEND MESSAGE";

    private static String server; // server IP address
    private static ServerSocket serverSocket; // socket to listen to the server

    public Connect_Thread(String server, ServerSocket serverSocket){
        this.server = server;
        this.serverSocket = serverSocket;
    }

    public void run(){
        if(!client.checkNullParameters(new ServerSocket[]{serverSocket},Thread.currentThread().getStackTrace()[1].getMethodName())){return;} // check for the validity of the parameters

        String operation; // the operation to be performed
        Socket clientSocket = new Socket();
        try {
            clientSocket = serverSocket.accept(); // accept the connection
        } catch (IOException e) {
            // Thread finished
        }
            while(!interrupted()){ // Executed while the thread is not interrupted

                DataInputStream input  = null; // buffer reader

                try {
                    input = new DataInputStream(clientSocket.getInputStream());
                } catch (IOException e) {
                    System.out.println("IO exception in disconnect");
                    e.printStackTrace();
                }

                if((operation = client.receiveString(input)) == null) { // get the operation to be performed
                    System.out.println("Error reading the operation");
                    break;
                }

                /* ********** SEND MESSAGE ************ */
                if(operation.equals(SEND_MESSAGE)){
                    if(!sendOperation(input)){ // Execute send message operation
                        System.out.println("Error while send message");
                        break;
                    }
                }
                else{
                    System.out.println("Wrong operation received");
                }
            }
    }

    /**
     * @brief Receives a message from the client
     *
     * @param input: the input channel to read the String sent by the server
     */
    private boolean sendOperation(DataInputStream input){
        if(!client.checkNullParameters(new DataInputStream[]{input},Thread.currentThread().getStackTrace()[1].getMethodName())){return false;} // check for the validity of the parameters

        String originUser, msgId, msg; // Strings to be received from the server
        if((originUser = client.receiveString(input)) == null){ // Read the originUser
            System.out.println("Error reading the origin user");
            return false;
        }
        else if((msgId = client.receiveString(input)) == null){ // Read the msgId
            System.out.println("Error reading the msgId");
            return false;
        }
        else if((msg = client.receiveString(input)) == null){ // Read the msg
            System.out.println("Error reading the msg");
            return false;
        }
        System.out.println("c> MESSAGE " + msgId + " FROM " + originUser + ":\n" + "   " + msg + "\n   END");
        return true;
    }

}