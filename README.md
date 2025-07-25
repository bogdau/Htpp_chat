# Htpp_chat

### Requirements

* Connection Handling:
  *  The server must listen for and accept incoming TCP connections on a configurable port.
  *  It must handle multiple client connections concurrently without blocking.
  *  It must gracefully handle clients disconnecting, removing them from the active chat session.
*  Protocol
   *  The server must handle an initial HTTP GET request from a client.
   *  It must support the HTTP Upgrade mechanism to switch the connection from HTTP to the WebSocket protocol for real-time, bidirectional communication.
*  Chat Logic:
   *  When a message is received from one client via its WebSocket connection, the server must broadcast that message to all other currently connected clients.
   *  The server should manage a list of active client sessions.
   *  Messages should include the sender's identifier (e.g., username or a unique ID) to be displayed on other clients.
*  Configuration & Control:
   *  The server should be started from the command line, allowing the IP address and port to be specified as arguments.
   *  It should log basic status messages, such as "Server started," "Client connected," and "Client disconnected," to the console.


### Hot to build
Change current directory: 
```bash 
cd Http_chat
```
Build the project using the meson:
```bash 
meson setup build 
```
To execute and start the server use this command:
```bash 
./build/chat 127.0.0.1 8080
```