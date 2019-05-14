/* Name: Brittany Dunn
 * Program Name: chatclient.c
 * Program Description: A simple chat system for a pair of users. This user is a chat client
 * Course: CS 372 - 400
 * Last Modified: April 30, 2019
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>

//Referenced Beej's Guide throughout

//This set of lines is directly from: https://beej.us/guide/bgnet/html/multi/clientserver.html#simpleserver
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
            return &(((struct sockaddr_in*)sa)->sin_addr);
	    }
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


/* 
 * Pre-conditions: Uses the serveName(char*) and port(char*) passed to it
 * Post-conditions: Returns the clientScoket(int) or if error program is exited
 * Function: Attempts to make a connection to server. If sucessful then the clientSocket is returned.
 * If unsuccessful an error message will appear and the program will exit
*/
	int initiateContact( char* serverName , char* port){
	//Get host information (IP address) using getaddrinfo
	//(referencing code from  Beej's guide at: https://beej.us/guide/bgnet/html/multi/getaddrinfoman.html )

	//hints will be a zero structure which is edited to define what getaddrinfo() searches for
	struct addrinfo hints;

	//servinfo will be a linked list of addrinfo structs with all the addresses matching hints when getaddrinfo is called
	struct addrinfo *servinfo;

	//Set the structure of hints to all zeros
	memset(&hints,0,sizeof hints);

	//Set the address family and connection oriented TCP protocol
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	//Call getaddrinfo to get server IP addresses and store in servinfo
	if(getaddrinfo(serverName,port,&hints,&servinfo) != 0){
		//If 0 is not returned then there was an error
		printf("Error: getting server info\n");
		exit(1);
	}

	//Make a connection to the host
	//(still referencing https://beej.us/guide/bgnet/html/multi/getaddrinfoman.html)

	//Use temp to hold addrinfo struct to go through servinfo
	struct addrinfo *temp;

	//Variable to hold socket
	int clientSocket;

	//Go through servinfo until a connection is made or none of the adddresses work
	for( temp = servinfo; temp != NULL; temp = temp->ai_next){
		//First try to create a socket
		if((clientSocket = socket(temp->ai_family, temp->ai_socktype, temp->ai_protocol)) == -1){
			//If -1 is returned then there was an error
			printf("Error: creating socket\n");
			continue;
		}

		//Then try to connect to socket
		if(connect(clientSocket, temp->ai_addr, temp->ai_addrlen) == -1){
			//If -1 returned then there was an error
			printf("Error: connecting to socket\n");
			close(clientSocket);
			continue;
		}

		//Successful connection
		break;
	}

	//Went through all of servinfo without a successful connection
	if(temp == NULL){
		printf("Error: No connection made\n");
		exit(2);
	}

	//These 3 lines are directly from https://beej.us/guide/bgnet/html/multi/clientserver.html#simpleserver
	char s[INET6_ADDRSTRLEN];
	inet_ntop(temp->ai_family, get_in_addr((struct sockaddr *)temp->ai_addr), s, sizeof s); //Edit p to temp
	printf("Client connected to: %s\n", s); //slightly edited this line


	//Remove structure once done
	freeaddrinfo(servinfo);

	//Return the socket info
	return clientSocket;
	
}

/*
 * Pre-conditions: Uses the handle(char*) and clientSocket(int) passed to it
 * Post-conditions: returns int of 0 or 1 based on if the there were any errors
 * Function: Client attempts to send a message to server. If successful 0 will be returned if not 1 will be returned
 */
int sendMessage(char* handle, int clientSocket){

	//Variables to hold sending messages
	char sendMsg[500];
	char completeSendMsg[512];

	//Reset send to zeros
	memset(sendMsg,0,sizeof sendMsg);
	memset(completeSendMsg,0,sizeof completeSendMsg);

	//Print handle so client knows to chat
	printf("%s",handle);

	//Get message from client(use fgets because of whitespace chars)
	fgets(sendMsg,500,stdin);

	//If the sending message is \quit then exit while loop by return 1
	if(strcmp(sendMsg,"\\quit\n") == 0){
		return 1;
	}

	//Prepend handle to send msg
	strcat(completeSendMsg,handle);
	strcat(completeSendMsg,sendMsg);

	//Send message to server
	if(send(clientSocket,completeSendMsg,(strlen(completeSendMsg)-1),0) == -1){
		//If -1 then there was an error
		printf("Error: sending message\n");
		//Exit program
		exit(1);
	}

	//If sending message successful return 0 to keep while loop going
	return 0;
}


/*
 * Pre-conditions: Uses the clientSocket(int) passed to it
 * Post-Conditions: return int of 0 upon success
 * Function: Client attempts to receive message from server. If successful 0 will be returned if not program will exit
 */
int receiveMessage(int clientSocket){

	//Variables to hold receiving messages
	char receiveMsg[500];

	//Reset receive to zeros
	memset(receiveMsg,0,sizeof receiveMsg);

	//Receive message from server
	int messageStatus = recv(clientSocket,receiveMsg,500,0); 
	
	//If there was an error getting message
	if(messageStatus == -1){
		//If -1 then there was an error
		printf("Error: receiving message\n");
		//Exit program
		exit(1);
	}

	//If server exits chat
	if(messageStatus == 0){
		printf("Server has left chat\n");
		//Exit program
		exit(0);
	}

	//Print received message
	printf("Server> '%s'\n",receiveMsg);

	//If message received successfully return 0 to keep loop going
	return 0;
}


int main(int argc, char** argv)
{

//Verify number of arguments before calling initiateContact

	//Check the command line has two additional arguments
	if(argc != 3){
		//If not exit program
		printf("Error: incorrect number of arguments");
		exit(1);
	}

	//If so get server name and port number from command line
	char* serverName = argv[1];
	char* port = argv[2];

	//Call initiateContact to try to make a connection to server
	//Store result in client socket
	int clientSocket = initiateContact(serverName, port);


//Get client's handle

	//First get users handle
	printf("Please enter a handle(10 characters or less):\n");

	//Variable to accept input
	char handle[12];

	//Read user input(use scanf to not store '\n')
	scanf("%10s",handle);

	//Get the length of handle
	int handleLength = strlen(handle);

	//add "> " to handle
	handle[handleLength] = '>';
	handle[handleLength+1] = ' ';

	//Clear input buffer
	while((getchar()) != '\n');

//Begin chat session

	//Variables to break while loop to end chat
	int endSend = 0;
	int endReceive = 0;

	//Begin while loop to continue chat until its interrupted
	while(endSend == 0 && endReceive == 0){
		
		//Call function to send message
		endSend = sendMessage(handle,clientSocket);

		//If sendMessage returns 1 then break loop to end chat
		if(endSend == 1){
			break;
		}

		//Call function to receive message
		endReceive = receiveMessage(clientSocket);

	}
	
//Disconnect after chat ends
	//Close connection
	close(clientSocket);

return 0;
}

