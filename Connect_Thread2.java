import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;

public class Connect_Thread2 extends Thread {
    private static final String SEND_MESSAGE = "SEND_MESSAGE";
    private static final String SEND_ATTACH = "SEND_ATTACH";
    private static String PATH = null;

    private static ServerSocket serverSocket; // socket to listen to the server

    public Connect_Thread2(ServerSocket serverSocket){
        this.serverSocket = serverSocket;
    }

    public void run(){

        ClassLoader classLoader = clientPart2.class.getClassLoader();
        File file = new File(classLoader.getResource("Connect_Thread2.java").getFile());
        String path = file.getParentFile().getPath(); // Get the current path
        PATH = path;

        if(!clientPart2.checkNullParameters(new ServerSocket[]{serverSocket},Thread.currentThread().getStackTrace()[1].getMethodName())){return;} // check for the validity of the parameters

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

            if((operation = clientPart2.receiveString(input)) == null) { // get the operation to be performed
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

            /* ********** SEND MESSAGE ************ */
            else if(operation.equals(SEND_ATTACH)){
                if(!sendFileOperationn(input)){ // Execute send message operation
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
        if(!clientPart2.checkNullParameters(new DataInputStream[]{input},Thread.currentThread().getStackTrace()[1].getMethodName())){return false;} // check for the validity of the parameters

        String originUser, msgId, msg; // Strings to be received from the server
        if((originUser = clientPart2.receiveString(input)) == null){ // Read the originUser
            System.out.print("Error receiving a message from the server\nc> ");
            return false;
        }
        else if((msgId = clientPart2.receiveString(input)) == null){ // Read the msgId
            System.out.print("Error receiving a message from the server\nc> ");
            return false;
        }
        else if((msg = clientPart2.receiveString(input)) == null){ // Read the msg
            System.out.print("Error receiving a message from the server\nc> ");
            return false;
        }
        System.out.print("MESSAGE " + msgId + " FROM " + originUser + ":\n" + "   " + msg + "\n   END\nc> ");
        return true;
    }

    /**
     * @brief Receives a message with a file from the client
     *
     * @param input: the input channel to read the String sent by the server
     */
    private boolean sendFileOperationn(DataInputStream input) {
        if(!clientPart2.checkNullParameters(new DataInputStream[]{input},Thread.currentThread().getStackTrace()[1].getMethodName())){return false;} // check for the validity of the parameters

        String originUser, msgId, msg, fileName, fileContent; // Strings to be received from the server
        if((originUser = clientPart2.receiveString(input)) == null){ // Read the originUser
            System.out.print("Error receiving a message from the server\nc> ");
            return false;
        }
        else if((msgId = clientPart2.receiveString(input)) == null){ // Read the msgId
            System.out.print("Error receiving a message from the server\nc> ");
            return false;
        }
        else if((msg = clientPart2.receiveString(input)) == null){ // Read the msg
            System.out.print("Error receiving a message from the server\nc> ");
            return false;
        }

        else if((fileName = clientPart2.receiveString(input)) == null){ // Read the FileNamw
            System.out.print("Error receiving a message from the server\nc> ");
            return false;
        }

        else if((fileContent = clientPart2.receiveString(input)) == null){ // Read the FileNamw
            System.out.print("Error receiving a message from the server\nc> ");
            return false;
        }
        File newFile = new File(fileName);
        try {
            newFile.createNewFile();  // create the new file
            BufferedWriter writer = new BufferedWriter(new FileWriter(fileName));
            writer.write(fileContent); // write the content received into the new file
            writer.close();
        } catch (IOException e) {
            System.out.print("Error receiving a message from the server \nc> ");
            return false;
        }

        System.out.print("MESSAGE " + msgId + " FROM " + originUser + ":\n" + "   " + msg +
                "\n   File Name: " + fileName + "\n" + fileContent + "\n   END\nc> ");
        return true;
    }
}
