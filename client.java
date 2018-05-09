import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.Socket;
import java.net.ServerSocket;

import gnu.getopt.Getopt;

class client {

    /* ******************** TYPES ********************* */

    /**
     * @brief Return codes for the protocol methods
     */
    private enum RC {
        OK,
        ERROR,
        USER_ERROR,
        FAIL,
        SWITCH_ERROR // to check if the server sends a byte different to the expected ones
    }

    /******************* ATTRIBUTES *******************/

    private static String _server   = null;
    private static String userName = null; // user register in the system
    private static String idMessage = "-1"; // used for SEND

    private static ServerSocket javaServerPort = null;
    private static int _port = -1;
    private static Connect_Thread thread;

    private static final String REGISTER = "REGISTER";
    private static final String UNREGISTER = "UNREGISTER";
    private static final String CONNECT = "CONNECT";
    private static final String DISCONNECT = "DISCONNECT";
    private static final String SEND = "SEND";

    private static final int maxSize = 256;




    /* ******************** METHODS ******************* */

    /**
     * Receive a Byte from the server
     *
     * @param socket: the socket used in the communication
     * @return the Byte received from the server
     */
    private static Byte receiveByte(Socket socket) {
        try {
            DataInputStream input  = new DataInputStream(socket.getInputStream()); // buffer reader

            Byte response;
            if ( (response = input.readByte()) < 0){ // return the String received
                System.out.println("Error reading byte");
                return null;
            }
            return response;
        } catch (IOException e) {
            e.printStackTrace();
            return null;
        }
    }


    private static String receiveString(Socket socket) {
        return "-1";
    }

    /**
     * @brief Send an string to the server
     *
     * @param socket: the socket used in the communication
     * @param msg: String to be sent
     */
    private static void sendString(Socket socket, String msg) {
        try { // handle socket errors
            DataOutputStream outputObject = new DataOutputStream(socket.getOutputStream());

            outputObject.write(msg.getBytes());
            outputObject.write('\0');
        }  catch (IOException e) {
            System.out.println("IO exception");
            e.printStackTrace();
        }
    }

    /**
     * @brief Perform a basic communication sending the username and the operation to be executed in the server
     *
     * @param user: User name to register in the system
     * @param operation: The name of the operation to perform (register,...)
     * @param port: the port sent to the server in some operations, such as connect
     * @param message: the message to be sent to a user
     *
     * @return OK if successful
     * @return USER_ERROR in case of any error related to the user
     * @return ERROR if another error occurred
     */
    private static RC registerCommunication(String user, String operation, String port, String message){
        try {
            Socket socket = new Socket(_server, _port); // socket to connect to the server
            sendString(socket, operation); // send the operation to the server

            if(!operation.equals(SEND)){ sendString(socket, user);} // send the username to the server

            /* CONNECT */
            if(operation.equals(CONNECT)){ sendString(socket, port); } // send the port to the server if it is a valid operation for it

            /* SEND */
            else if(operation.equals(SEND)){
                sendString(socket, userName); // If the operation is a SEND, the message is sent too
                sendString(socket, user); // send the username to the server
                sendString(socket, message); // If the operation is a SEND, the message is sent too
            }
            Byte result = receiveByte(socket); // get the response byte

            /* SEND AND CORRECT RESULT */
            if(operation.equals(SEND) && result == 0x00) { idMessage = receiveString(socket);} // get the id associated to the message sent (only in SENT)

            socket.close(); // close the socket

            switch (result){ // check the error byte of the server
                case 0x00:
                    return RC.OK;
                case 0x01:
                    return RC.USER_ERROR;
                case 0x02:
                    return RC.ERROR;
                case 0x03:
                    return RC.FAIL;
                default:
                    return RC.SWITCH_ERROR;
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
        if (operation.equals(CONNECT)  || operation.equals(DISCONNECT)) {return RC.FAIL;}
        else {return RC.ERROR;}
    }

    /**
     * @brief Deal with the errors of the operation's execution
     *
     * @param error: the return value of the execution of an operation
     * @param msg: Array of error messages to print
     */
    private static RC dealWithErrors(RC error, String [] msg){
        switch(error){
            case OK:
                System.out.println(msg[0]);
                break;
            case USER_ERROR:
                System.out.println(msg[1]);
                break;
            case ERROR:
                System.out.println(msg[2]);
                break;
            case FAIL:
                System.out.println(msg[3]);
                break;
            default:
                System.out.println("DEFAULT CASE (IT SHOULD NEVER ARRIVED HERE)");
                break;
        }
        return error;
    }

    /**
     * @brief Send the register request to the server
     *
     * @param user - User name to register in the system
     */
    private static void register(String user){
        String [] msg = {"c> REGISTER OK", "c> USERNAME IN USE", "c> REGISTER FAIL", "REGISTER BROKEN"}; // error messages
        dealWithErrors(registerCommunication(user, REGISTER,"-1", "NONE"), msg); // Register the new user
    }

    /**
     * @brief Send the unregister request to the server
     *
     * @param user - User name to unregister from the system
     */
    private static void unregister(String user){
        String [] msg = {"c> UNREGISTER OK", "c> USER DOES NOT EXIST", "c> UNREGISTER FAIL", "UNREGISTER BROKEN"}; // error messages
        RC result = dealWithErrors(registerCommunication(user, UNREGISTER,"-1", "NONE"), msg); // Perform the unregistration
        if(result == RC.OK || result == RC.FAIL){ userName = null;} // set the register user to null
    }

    /**
     * @brief Send the connect request to the server
     *
     * @param user - User name to connect to the system
     */
    private static void connect(String user){
        String [] msg = {"c> CONNECT OK", "c> CONNECT FAIL, USER DOES NOT EXIST", "c> USER ALREADY CONNECTED", "c> CONNECT FAIL"}; // error messages
        try{
            javaServerPort = new ServerSocket(0); // socket to listen to the server
            thread = new Connect_Thread(_server,javaServerPort);
            thread.start();
            if((dealWithErrors(registerCommunication(user, CONNECT,Integer.toString(javaServerPort.getLocalPort()), "NONE"), msg)) == RC.OK){ userName = user;} // Connect the user
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    /**
     * @brief Disconnect an user
     *
     * @param user - User name to disconnect from the system
     */
    private static void disconnect(String user){
        String [] msg = {"c> DISCONNECT OK", "c> DISCONNECT FAIL / USER DOES NOT EXIST", "c> DISCONNECT FAIL / USER NOT CONNECTED", "c> DISCONNECT FAIL"}; // error messages
        dealWithErrors(registerCommunication(user, DISCONNECT, "-1", "NONE"), msg); // Perform the connection
        thread.interrupt();
        try {
            javaServerPort.close();
        } catch (IOException e) {
            return;
        }
    }

    /**
     * @brief Send a message to a user
     *
     * @param user    - Receiver user name
     * @param message - Message to be sent
     *
     * @return OK if the server had successfully delivered the message
     * @return USER_ERROR if the user is not connected (the message is queued for delivery)
     * @return ERROR the user does not exist or another error occurred
     */
    private static void send(String user, String message){
        String [] msg = {"c> SEND OK - MESSAGE " , "c> SEND FAIL / USER DOES NOT EXIST", "c> SEND FAIL", "SEND BROKEN"}; // error messages
        RC result = registerCommunication(user, SEND, "-1", message); // Execute the sending and obtaining the result
        msg[0] += "" + idMessage; // Concatenate the id of the message to the message
        dealWithErrors(result, msg); // Perform the connection
    }

    /**
     * @brief Command interpreter for the client. It calls the protocol functions.
     */
    private static void shell()
    {
        boolean exit = false;
        String input;
        String [] line;
        BufferedReader in = new BufferedReader(new InputStreamReader(System.in));

        while (!exit) {
            try {
                System.out.print("c> ");
                input = in.readLine();
                line = input.split("\\s");

                if (line.length > 0) {
                    /* ********** REGISTER ************ */
                    if (line[0].equals("REGISTER")) {
                        if  (line.length == 2) {
                            register(line[1]); // userName = line[1]
                        } else {
                            System.out.println("Syntax error. Usage: REGISTER <userName>");
                        }
                    }

                    /* ********* UNREGISTER *********** */
                    else if (line[0].equals("UNREGISTER")) {
                        if  (line.length == 2) {
                            unregister(line[1]); // userName = line[1]
                        } else {
                            System.out.println("Syntax error. Usage: UNREGISTER <userName>");
                        }
                    }

                    /* *********** CONNECT ************ */
                    else if (line[0].equals("CONNECT")) {
                        if  (line.length == 2) {
                            connect(line[1]); // userName = line[1]
                        } else {
                            System.out.println("Syntax error. Usage: CONNECT <userName>");
                        }
                    }

                    /* ********* DISCONNECT *********** */
                    else if (line[0].equals("DISCONNECT")) {
                        if  (line.length == 2) {
                            disconnect(line[1]); // userName = line[1]
                        } else {
                            System.out.println("Syntax error. Usage: DISCONNECT <userName>");
                        }
                    }

                    /* ************* SEND ************* */
                    else if (line[0].equals("SEND")) {
                        if  (line.length >= 3) {
                            // Remove first two words
                            String message = input.substring(input.indexOf(' ')+1).substring(input.indexOf(' ')+1);
                            send(line[1], message); // userName = line[1]
                        } else {
                            System.out.println("Syntax error. Usage: SEND <userName> <message>");
                        }
                    }

                    /* ************* QUIT ************* */
                    else if (line[0].equals("QUIT")){
                        if (line.length == 1) {
                            exit = true;
                        } else {
                            System.out.println("Syntax error. Use: QUIT");
                        }
                    }

                    /* ************ UNKNOWN *********** */
                    else {
                        System.out.println("Error: command '" + line[0] + "' not valid.");
                    }
                }
            } catch (java.io.IOException e) {
                System.out.println("Exception: " + e);
                e.printStackTrace();
            }
        }
    }

    /**
     * @brief Prints program usage
     */
    private static void usage()
    {
        System.out.println("Usage: java -cp . client -s <server> -p <port>");
    }

    /**
     * @brief Parses program execution arguments
     */
    private static boolean parseArguments(String [] argv)
    {
        Getopt g = new Getopt("client", argv, "ds:p:");

        int c;
        String arg;

        while ((c = g.getopt()) != -1) {
            switch(c) {
                //case 'd':
                //	_debug = true;
                //	break;
                case 's':
                    _server = g.getOptarg();
                    break;
                case 'p':
                    arg = g.getOptarg();
                    _port = Integer.parseInt(arg);
                    break;
                case '?':
                    System.out.print("getopt() returned " + c + "\n");
                    break; // getopt() already printed an error
                default:
                    System.out.print("getopt() returned " + c + "\n");
            }
        }

        if (_server == null)
            return false;

        if ((_port < 1024) || (_port > 65535)) {
            System.out.println("Error: Port must be in the range 1024 <= port <= 65535");
            return false;
        }

        return true;
    }

    /********************* MAIN **********************/

    public static void main(String[] argv)
    {
		if(!parseArguments(argv)) {
			usage();
			return;
		}
        _server = argv[1]; // name of the server
        _port = Integer.parseInt(argv[3]); // port number
        shell();
    }

}
