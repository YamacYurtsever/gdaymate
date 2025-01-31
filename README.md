# Gdaymate

**Gdaymate** is a simple CLI-based client-server messaging app

---

### Server Logic

1. Create a TCP server
	- Create a socket
	- Create a poll set
	- Setup the server
		- Define the server socket address
		- Bind server socket to server socket address
		- Add server socket to poll set
1. Start the server
	- Start listening for incoming connections
	- Start server loop
2. Stop the server (on SIGINT)
	- Close all sockets
	- Free the poll set
	

##### Server loop

1. Wait until at least one socket is ready
2. Check all sockets in poll set
	- If the socket is the server
		- Accept a connection (get a client)
		- Add it to the poll set
	- If the socket is a client
		- Receive the string
		- Parse the string
		- Validate the message
		- Process the message
		- Free the message
		  
##### Process Message

- Process Text Message
	1. Access the headers
	2. Log the content
	3. Broadcast to other clients
- Process Join Message

##### Multithreading

The server has a *multithreaded*  version using a thread pool, working as displayed in the following diagram

---

### Client Logic

1. Create a TCP client
	- Create a user interface
	- Create a socket
	- Setup the client
		- Define server socket address
		- Connect client socket to server socket address
1. Start the client
	- Get the username
	- Start client loop
2. Stop the client (on SIGINT)
	- Close the socket
	- Free the user interface

##### Client Loop

1. Get the content
2. Get the timestamp
3. Send the text message

##### Send Message

- Send Text Message
	1. Create a message
	2. Add headers to the message
	3. Serialize the message
	4. Send the string
	5. Display the message
- Send Join Message

---

### Custom Protocol

**Gdaymate Protocol (GDMP)** is a custom network protocol that operates at the *application layer* of the OSI model defining the structure of messages in Gdaymate

##### GDMP Messages

**GDMP messages** start with a message type followed by the data

```
GDMP_TEXT_MESSAGE
Username: Will
Content: G'day mate!
Timestamp: 14:18
```

##### GDMP Message Types

**GDMP message types** each have certain headers that they expect, if an expected header isn't found then the message is invalid, additional headers are ignored

1. GDMP_TEXT_MESSAGE
2. GDMP_JOIN_MESSAGE

##### GDMP Data

**GDMP data** is given in header-value pairs which are *case-sensitive* and *unordered*

- Username
- Content
- Timestamp

---

### Client User Interface

Client user interface is built using the `ncurses` library

- Display Message
	1. Scroll messages (if overflows)
	2. Add message to messages
	3. Print messages
- Display Input
	1. Print prompt
	2. Capture input
