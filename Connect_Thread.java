import java.io.DataInputStream;
import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;

public class Connect_Thread extends Thread {
    private static final String SEND_MESSAGE = "SEND_MESSAGE";

    private static ServerSocket serverSocket; // socket to listen to the server

    public Connect_Thread(ServerSocket serverSocket){
        this.serverSocket = serverSocket;
    }

    public void run(){
        if(!client.checkNullParameters(new ServerSocket[]{serverSocket},Thread.currentThread().getStackTrace()[1].getMethodName())){return;} // check for the validity of the parameters

        String operation; // the operation to be performed
        Socket clientSocket = new Socket();
        while(!interrupted()){ // Executed while the thread is not interrupted
            try {
                clientSocket = serverSocket.accept(); // accept the connection
            } catch (IOException e) {
                return;
                // Thread finished
            }
            DataInputStream input  = null; // buffer reader

            try {
                input = new DataInputStream(clientSocket.getInputStream());
            } catch (IOException e) {
                System.out.print("Server cannot connect to the client\nc> ");
                return;
            }

            if((operation = client.receiveString(input)) == null) { // get the operation to be performed
                System.out.print("Error receiving a message from the server\nc> ");
                closeSocket(clientSocket);
                break;
            }

            /* ********** SEND MESSAGE ************ */
            if(operation.equals(SEND_MESSAGE)){
                if(!sendOperation(input)){ // Execute send message operation
                    System.out.print("Error receiving a message from the server\nc> ");
                    closeSocket(clientSocket);
                    break;
                }
            }
            else{
                System.out.print("Error receiving a message from the server\nc> ");
            }
            closeSocket(clientSocket);
        }
    }

    private void closeSocket(Socket clientSocket) {
        try {
            clientSocket.close();
        } catch (IOException e) {
            System.out.println("Error receiving a message from the server");
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
            System.out.print("Error receiving a message from the server\nc> ");
            return false;
        }
        else if((msgId = client.receiveString(input)) == null){ // Read the msgId
            System.out.print("Error receiving a message from the server\nc> ");
            return false;
        }
        else if((msg = client.receiveString(input)) == null){ // Read the msg
            System.out.print("Error receiving a message from the server\nc> ");
            return false;
        }
        System.out.print("MESSAGE " + msgId + " FROM " + originUser + ":\n" + "   " + msg + "\n   END\nc>");
        return true;
    }

}