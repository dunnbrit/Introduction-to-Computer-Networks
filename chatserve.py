# Name: Brittany Dunn
# Program Name: chatserve.py
# Program Description: A simple chat system for two users. This user is a chat server
# Course: CS 372 - 400
# Last Modified: April 30, 2019


# Library to use sockets
import socket

# Library to get command line argument
import sys

# Referenced Lecture 15 and geeksforgeeks.org/socket-programming-python

# Create a socket object
try:
	serverSocket = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
	
except:
	print "Socket Creation FAILED"
	exit()

# Get the port number from command line
try:
	serverPort = int(sys.argv[1])
	
except:
	print "Port Number NOT saved"
	exit()

# Bind the socket to the port and host
# Host is an empty string so it accepts requests from other network computers
try:
	serverSocket.bind(('',serverPort))
	
except:
	print "Bind FAILED"
	exit()

# Start the server to listen for incoming TCP requests
try:
	# Only open 1 connection on the server
	serverSocket.listen(1)
	print "Server listening for requests"
except:
	print "Server NOT listening"




# Loop until interrupted
while 1:
	# Connect to client sending request by creating a new socket object
	clientConnectSocket,addr = serverSocket.accept()
	print 'Connected to' , addr
	while 1:
		# Receive Message
		recievedMsg = clientConnectSocket.recv(512)

		# If client ended connection
		if recievedMsg == "":
			print "Client has left chat"	
			break

		# Print Message
		print recievedMsg
		

		# Get message to send from user
		sendMsg = raw_input('Server> ')

		# if the message is quit
		if "\quit" in sendMsg:
			# close the connection
			clientConnectSocket.close()
			# break nested loop (skip next statement)
			break

		# send the message
		clientConnectSocket.send(sendMsg)



