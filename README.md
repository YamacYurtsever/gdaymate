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
	- Handle the client
		- Receive the data
		- Parse the data (according to custom protocol)
		- Log the message
		- Broadcast to other clients

---

### Client Logic

1. Create a TCP client
	- Create a socket
2. Connect to server
	- Define server socket address
	- Connect the client socket to the server socket address
2. Send a message to the server

---

### Custom Protocol

**Gdaymate Protocol (GDMP)** is a custom network protocol that operates at the application layer of the OSI model defining the structure of messages in Gdaymate

1. Client *serializes* the content into a message (with additional metadata)
2. Server *deserializes* the message

```
GDMP
Username: Michael
Timestamp: 2025-01-26T12:34:56Z
Content: "G'day mate!"
```
