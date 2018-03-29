import java.io.*;
import java.net.Socket;
import java.net.SocketException;
import java.net.UnknownHostException;
import java.util.logging.SocketHandler;

import gnu.getopt.Getopt;

import static java.lang.Thread.sleep;

class client {

    /********************* TYPES **********************/

    /**
     * @brief Return codes for the protocol methods
     */
    private static enum RC {
        OK,
        ERROR,
        USER_ERROR
    };

    /******************* ATTRIBUTES *******************/

    private static String _server   = null;
    private static int _port = -1;

    private static final String REGISTER = "REGISTER";


    /********************* METHODS ********************/

    /**
     * Receive a Byte from the server
     *
     * @param socket
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

    /**
     * Send an string and the operation to be performed to the server by the given port
     *
     * @param operation: the operation to be done in the server
     * @param msg: String to be sent
     */
    private static void sendString(Socket socket, String operation, String msg) {
        try { // handle socket errors
            DataOutputStream outputObject = new DataOutputStream(socket.getOutputStream());

            outputObject.write(operation.getBytes());
            outputObject.write('\0');
            outputObject.write(msg.getBytes());
            outputObject.write('\0');

        }  catch (IOException e) {
            System.out.println("IO exception");
            e.printStackTrace();
        }
    }

    /**
     * Send an integer to the server
     *
     * @param serverName
     * @param portNumber
     * @param msg: int to send to the server
     */
    private static void sendInt(String serverName, int portNumber, int msg) {
        try { // handle socket errors
            Socket socket = new Socket(serverName, portNumber); // socket to connect to the server
            DataOutputStream outputObject = new DataOutputStream(socket.getOutputStream());

            outputObject.writeInt(msg); // write the message in the socket

            socket.close(); // close the socket
            outputObject.close(); // close the outputObject

        } catch (SocketException e) {
            System.out.println("Socket exception");
            e.printStackTrace();
        } catch (IOException e) {
            System.out.println("IO exception");
            e.printStackTrace();
        }
    }

    /**
     * @param user - User name to register in the system
     *
     * @return OK if successful
     * @return USER_ERROR if the user is already registered
     * @return ERROR if another error occurred
     */
    static RC register(String user)
    {
        try {
            Socket socket = new Socket(_server, _port); // socket to connect to the server
            sendString(socket, REGISTER, user); // send the message to the server
            Byte result = receiveByte(socket); // get the response byte
            System.out.println("Byte: " + result);
            socket.close(); // close the socket
            switch (result){ // check the error byte of the server
                case 0x00:
                    System.out.println("Success");
                    return RC.OK;
                case 0x01:
                    System.out.println("User error");
                    return RC.USER_ERROR;
                case 0x02:
                    System.out.println("Error");
                    return RC.ERROR;
                default:
                    System.out.println("Nunca tiene que salir esto");
                    return RC.ERROR;
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
        return RC.ERROR;
    }

    /**
     * @param user - User name to unregister from the system
     *
     * @return OK if successful
     * @return USER_ERROR if the user does not exist
     * @return ERROR if another error occurred
     */
    static RC unregister(String user)
    {
        // Write your code here
        return RC.ERROR;
    }

    /**
     * @param user - User name to connect to the system
     *
     * @return OK if successful
     * @return USER_ERROR if the user does not exist or if it is already connected
     * @return ERROR if another error occurred
     */
    static RC connect(String user)
    {
        // Write your code here
        return RC.ERROR;
    }

    /**
     * @param user - User name to disconnect from the system
     *
     * @return OK if successful
     * @return USER_ERROR if the user does not exist
     * @return ERROR if another error occurred
     */
    static RC disconnect(String user)
    {
        // Write your code here
        return RC.ERROR;
    }

    /**
     * @param user    - Receiver user name
     * @param message - Message to be sent
     *
     * @return OK if the server had successfully delivered the message
     * @return USER_ERROR if the user is not connected (the message is queued for delivery)
     * @return ERROR the user does not exist or another error occurred
     */
    static RC send(String user, String message)
    {
        // Write your code here
        return RC.ERROR;
    }

    /**
     * @brief Command interpreter for the client. It calls the protocol functions.
     */
    static void shell()
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
                    /*********** REGISTER *************/
                    if (line[0].equals("REGISTER")) {
                        if  (line.length == 2) {
                            register(line[1]); // userName = line[1]
                        } else {
                            System.out.println("Syntax error. Usage: REGISTER <userName>");
                        }
                    }

                    /********** UNREGISTER ************/
                    else if (line[0].equals("UNREGISTER")) {
                        if  (line.length == 2) {
                            unregister(line[1]); // userName = line[1]
                        } else {
                            System.out.println("Syntax error. Usage: UNREGISTER <userName>");
                        }
                    }

                    /************ CONNECT *************/
                    else if (line[0].equals("CONNECT")) {
                        if  (line.length == 2) {
                            connect(line[1]); // userName = line[1]
                        } else {
                            System.out.println("Syntax error. Usage: CONNECT <userName>");
                        }
                    }

                    /********** DISCONNECT ************/
                    else if (line[0].equals("DISCONNECT")) {
                        if  (line.length == 2) {
                            disconnect(line[1]); // userName = line[1]
                        } else {
                            System.out.println("Syntax error. Usage: DISCONNECT <userName>");
                        }
                    }

                    /************** SEND **************/
                    else if (line[0].equals("SEND")) {
                        if  (line.length >= 3) {
                            // Remove first two words
                            String message = input.substring(input.indexOf(' ')+1).substring(input.indexOf(' ')+1);
                            send(line[1], message); // userName = line[1]
                        } else {
                            System.out.println("Syntax error. Usage: SEND <userName> <message>");
                        }
                    }

                    /************** QUIT **************/
                    else if (line[0].equals("QUIT")){
                        if (line.length == 1) {
                            exit = true;
                        } else {
                            System.out.println("Syntax error. Use: QUIT");
                        }
                    }

                    /************* UNKNOWN ************/
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
    static void usage()
    {
        System.out.println("Usage: java -cp . client -s <server> -p <port>");
    }

    /**
     * @brief Parses program execution arguments
     */
    static boolean parseArguments(String [] argv)
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
        /* ********** Variables to use *********** */
//        String stringToSend = "TEST"; // String to send to the server
//        String serverName = argv[1]; // name of the server
//        int portNumber = Integer.parseInt(argv[3]); // port number
//
//        String msg = "SEND"; /* message to send to the server */
//
//        sendInt(serverName, portNumber, msg.length()); /* send integer */
//
//        sendString(serverName, portNumber, msg); /* send a string */
//
//        Byte input = receiveByte(serverName, portNumber); /* receive a String from the server */
//
//        if(input == 0)  System.out.println("0");
//        else if(input == 1)  System.out.println("1");
//        if(input == 2)  System.out.println("2");

        _server = argv[1]; // name of the server
        _port = Integer.parseInt(argv[3]); // port number
        shell();
    }

}
