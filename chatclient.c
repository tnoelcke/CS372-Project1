//Author: Thomas Noelcke
//CS 372 Spring 2017
//
//Description: This is the client side of an instant messanger chat client. 
//Client takes two arguments the host name for the computer the server is running on
//and the port number for the sever. This program will then ask you to enter in your handle 
//a single work 10 character name. then you will be connected to the server and a chat session will begin
//the chat session can be ended at any time by typing \quit.

//#includes
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>

//headers
int setUpConnection(char* host, int portNumber);
int sendMessage(int socketFD, char* handle);
int reciveMessage(int socketFD);
char* getHandle();
char* getInput();
void introduction();

int main(int argc, char** argv) {
	introduction();
	if(argc != 3){
		fprintf(stderr, "Usage: chatclient <server-hostname> <port#>\n");
		exit(1);
	}
	int portNum = atoi(argv[2]);
	char host[100];
	memset(host, '\0', sizeof(host));
	strcpy(host, argv[1]);
	
	//sets up the socket and gets the socket file descriptor so data can be sent and recived from the 
	//socket later.
	int socketFD = setUpConnection(host, portNum);
	char* handle = getHandle();
	int userQuit = 0;
	int serverQuit = 0;
	while(userQuit == 0 && serverQuit == 0){
		serverQuit = reciveMessage(socketFD);
		userQuit = sendMessage(socketFD, handle);
	}
}

void introduction(){
	printf("Hello, This is a chat client for an instant messanger\n");
        printf("you will be connected with another user and will send messages to eachother.\n");
	printf("The Host you are connecting to will send the first message. After you recive it you");
	printf("May send a message. You will then alternate. Type \\quit into your message to disconect\n");
	printf("You will be asked to enter a handle it needs to be 10 characters or less with no spaces\n");
}

//this function sets up a connection with the server.
int setUpConnection(char* host, int portNumber){
	int socketFD;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	
	//sets up the server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); //clear out the address struct
	serverAddress.sin_family = AF_INET; //create a network-capable socket
	serverAddress.sin_port = htons(portNumber); //stor the portnumber with out regards to indianess
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); //copy in the address
	
	//set up the socket
	socketFD = socket(AF_INET, SOCK_STREAM, 0); //create the socket
	if(socketFD < 0){
		fprintf(stderr, "Error Openening The Socket\n");
		exit(1);
	}
	//connects to the server.
	if(connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0){
		fprintf(stderr, "Error Connecting to the server");
		exit(1);
	}
	return socketFD;
	
	
}

//recives and dispalys a message form the server.
//If the server sends the message \quit this function returns
// 1. this connection is then closed.
int reciveMessage(int socketFD){
	char buffer[513]; // max message size is 500 characters so 500 characters plus the handle + > + " " + \0 = 513 characters
	memset(buffer, '\0', sizeof(buffer));
	int charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0);
	if (charsRead < 0){
		fprintf(stderr, "error reading from the socket\n");
		exit(1);
	}
	if(strstr(buffer, "\\quit") != NULL){
		return 1;
	}
	printf("%s\n", buffer);
	return 0;
}

//sends a message to the server. If the user types quit it sends that
//message to the server cause the server to terminate the connection
//this function then returns 1 if that happnes.
int sendMessage(int socketFD, char* handle){
	char message[514];
	memset(message, '\0', sizeof(message));
	strcpy(message, handle);
	char* buffer;
	printf("%s", handle);
	buffer = getInput();
	int size = 0;
	if(strlen(buffer) < 500){
		size = strlen(buffer);
	} else {
		size = 500;
	}
	strncat(message, (const char*) buffer, size);
	strcat(message, "\n");
	int charsWriten = send(socketFD, message, strlen(message), 0);
	if(charsWriten < 0){
		fprintf(stderr,"error writing to the socket\n");
		exit(1);
	}
	if(strstr(buffer, "\\quit") != NULL){
		free(buffer);
		return 1;
	}
	free(buffer);
	return 0;
}

//gets the user handle and returns it. Makes sure the handle is less than 10 characters
//DOES NO MEMORY CLEAN UP YOU ARE RESPOSIBLE FOR MAKING SURE THAT 
//THE MEMORY IS FREED.
char* getHandle(){
	int isValid = 0;
	char* handle = NULL;
	while(isValid == 0){
		//propts the user and gets the handle
		printf("Please enter your Handle:");
		handle = getInput();
		//checks that the handle is valid and ends the loop if it is
		if(handle == NULL){
			printf("That is an invalid handle Try again.\n");
		} else if(strlen(handle) > 10) {
			printf("Your handle is too long. Try again.\n");
		} else {
			isValid = 1;
		}
	}
	char* toReturn = malloc(sizeof(char) * (strlen(handle) + 3));
	strcpy(toReturn, handle);
	strcat(toReturn, "> ");
	return toReturn;
}

char* getInput(){
	//make sure every thing has been displayed to the screen
	fflush(stdout);
	char* line;
	line = NULL;
	size_t lineSize = 0;
	int num = getline(&line, &lineSize, stdin);
	// the user didn't enter any thing.
	if(line[0] == 10){
		return NULL;
	}
	//discards newline.
	line[num - 1] = '\0';
	return line;
}
