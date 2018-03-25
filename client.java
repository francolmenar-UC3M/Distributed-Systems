import java.io.*;
import java.net.Socket;
import java.net.SocketException;
import java.net.UnknownHostException;
import java.util.logging.SocketHandler;

import gnu.getopt.Getopt;

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


    /********************* METHODS ********************/

    /**
     * @param user - User name to register in the system
     *
     * @return OK if successful
     * @return USER_ERROR if the user is already registered
     * @return ERROR if another error occurred
     */
    static RC register(String user)
    {
        // Write your code here
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
        //*********** Variables to use ************/

        Socket socket = null;
		String serverName, stringToSend, response;
		int portNumber;
        OutputStream out = null;
        ObjectOutput outputObject;
        BufferedReader input;

		serverName = argv[1]; // name of the server
        portNumber = Integer.parseInt(argv[3]); // port number

        System.out.println("Server name: " +  serverName + "\nPort number: " + portNumber);

        try { // handle socket errors
            socket = new Socket(serverName, portNumber); // socket to connect to the server
        } catch (SocketException e) {
            System.out.println("Socket exception");
            e.printStackTrace();
            return; // no se si hay que salir si peta esto REVISARLO
        } catch (IOException e) {
            System.out.println("IO exception");
            e.printStackTrace();
            return; // no se si hay que salir si peta esto REVISARLO
        }
        try { // set the input and the output of the socket
            out = socket.getOutputStream(); // output stream
            outputObject = new ObjectOutputStream(out); // objectOutput
            input = new BufferedReader(new InputStreamReader(socket.getInputStream())); // buffer reader
        } catch (IOException e) {
            System.out.println("IO exception");
            e.printStackTrace();
            return; // no se si hay que salir si peta esto REVISARLO
        }
        stringToSend = "Test"; // test string
        try {
            outputObject.writeObject(stringToSend); // write the message in the socket
            outputObject.flush(); // send the request

            response = input.readLine();

            System.out.println("Response of the server: " + response);

            socket.close(); // close the socket
            out.close(); // close the outputStream
            outputObject.close(); // close the outputObject
            input.close(); //close the bufferedReader

        } catch (IOException e) {
            System.out.println("IO exception");
            e.printStackTrace();
            return; // no se si hay que salir si peta esto REVISARLO
        }

		shell();

    }
}
