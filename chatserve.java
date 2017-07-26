// Author: Thomas Noelcke
// Cs 372
// Project 1
//
// Description: This program is the server side of an instant messanger program.
// This program waits for a connection on a specified port. Once the client sends the message
// PORTNUM a connection is made between the two programs. The programs will then start sending messages to
// eachother starting with The client. The Programs must take turns sending and reciving messages.
// Either program can terminate the connection by saying \quit.
// This code was written with the help of the oracle java documentation on sockets.


import java.net.*;
import java.io.*;

class chatserve {
	//hard codes the tag for Noelcket
	public static final String HANDLE = "noelcket> ";
	
	public static void main(String[] args) throws IOException {
		
		//if the user doesn't provide a port number let the user know they need to
		//specify a port number
		if(args.length != 1){
			System.err.println("Usage: java chatserve <port number>");
			System.exit(1);
		}
		intro();
		//gets port number from the arguments
		int portNumber = Integer.parseInt(args[0]);
		ServerSocket serverSocket = new ServerSocket(portNumber);
		//runs until the users stops the program
		while(true){
			//sets up connection.
			System.out.println("Waiting for another user to connect. Please Wait");
			Socket clientSocket = serverSocket.accept();
			// sets up input and out put streams for communication
			PrintWriter out = new PrintWriter(clientSocket.getOutputStream(), true);
			BufferedReader in = new BufferedReader( new InputStreamReader(clientSocket.getInputStream()));
			System.out.println("Another user has connected please send a message.");
			boolean clientQuit = false;
			boolean serverQuit = false;
			while(true){
				serverQuit = sendMessage(out);
				if(serverQuit) {
					break;
				}
				clientQuit = readMessage(in);
				if(clientQuit){
					break;
				}
			}
			clientSocket.close();
			System.out.println("The client has been disconnected waiting for a new connection...");
		}
	}
	
	//introduces the user to the program and gives them some basic instructions.
	public static void intro(){
		System.out.println("This is a chat messenger program\n" +
				   "This program will wait for a client to connect and with the propt you to enter a message to send\n" +
				   "You will then need to wait while the user on the other host sends their message and you may\n" +
				   "Then reply. If you type \\quit the user at the other end will be disconnected and this program will\n" +
				   "wait for a new user to connect. Press ctrl c to kill this program\n");
	}	


	//Reads in a message from the network connection.
	public static boolean readMessage (BufferedReader in) throws IOException {
		String inputLine;
		inputLine = in.readLine();
		if(inputLine != null) {
			System.out.println(inputLine);
		}
		if(inputLine.contains("\\quit")){
			return true;
		}
		return false;
	}

	//sends a message out to the network connection
	public static boolean sendMessage(PrintWriter out) throws IOException {
		String outputLine, buffer, toSend;
		System.out.print(HANDLE);
		BufferedReader in = new BufferedReader(new InputStreamReader(System.in));
		outputLine = new String(HANDLE);
		buffer = in.readLine();
		if(buffer.length() > 500){
			toSend = buffer.substring(0, 500);
			outputLine += toSend;
		} else {
			outputLine += buffer;
		}
		out.println(outputLine);
		if(outputLine.contains("\\quit")){
			return true;
		}
		return false;
	}
}
