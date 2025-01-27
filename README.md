**Gdaymate** is a simple CLI-based client-server messaging app

---

### Server Logic

1. Create a TCP server
	- Create a socket 
	- Define server socket address
	- Bind the socket to the socket address
2. Create a thread pool
3. Start the server (listen for connections)
4. Server loop
	- Accept a connection (get a client)
	- Create a task to handle the client
	- Add the task to the thread pool's task queue

##### Handling the client
 
1. Receive the string
2. Parse the string
3. Get message type
4. Send message to processing (according to type)

##### Processing

- GDMP_MESSAGE
	1. Access the headers
	2. Log the content
	3. Broadcast to other clients
- GDMP_AUTH

---

### Client Logic

1. Create a TCP client
	- Create a socket
2. Connect to server
	- Define server socket address
	- Connect the client socket to the server socket address
3. Send a message to the server
	- Create a message
	- Add the headers
	- Serialize the message
	- Send the string
4. Log the message

---

### Custom Protocol

**Gdaymate Protocol (GDMP)** is a custom network protocol that operates at *the application layer* of the OSI model defining the structure of messages in Gdaymate

##### GDMP Messages

**GDMP messages** start with a message type followed by the data

```
GDMP_MESSAGE
Username: Will
Content: G'day mate!
```

##### GDMP Message Types

**GDMP message types** each have certain headers that they expect, if an expected header isn't found then the message is invalid, additional headers are ignored

1. GDMP_MESSAGE
2. GDMP_AUTH

##### GDMP Data

**GDMP data** is given in header-value pairs which are *case-sensitive* and *unordered*

- Username
- Content

---

### Client User Interface

Client user interface is built using the `ncurses` library

- Display Message
	1. Scroll the messages (if overflows)
	2. Form the message
	3. Copy the message to messages
	4. Print the messages
- Display Input Box
	1. Initialize a new window (on the bottom of the page)
	2. Draw the input box
	3. Display the prompt
	4. Capture the input (into the buffer)
