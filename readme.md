# Multifunctional Web Server with a custom application-layer protocol.

This is a first attempt at creating a multithreaded client-server application built on TCP, using C. The SocketsAPI is used for networking and PThreads to implement multithreading. A custom layer 5 protocol has also been designed to perform the various erver functionalities.

<for Mythili Ma'am> : A major update will be pushed this weekend to fix the chat and download functions. [7 Apr 2023]

## Prerequisites
1. Ensure you are using Linux (I use Ubuntu 20.04 running on a Virtual Machine)
2. Ensure ```make``` is installed.

## Usage
1. Compile the project via ```make```
2. Run the server program via ```./server <Server Port Number>```.
3. Run the client program in independent shell processes via ```./client <ClientIP> <Server Port Number>```.

## Overview
First, a server is started as a new process, and listens for incoming connections on a specified port and spawn a new thread for each accepted client. On intialisation, server and client create their own working directories. Each server thread prompts its client for a username, which is sent and stored in a global list of users in the server.


Clients are started as independent processes with two threads : ```c_clientinputhandler``` and ```c_servereresponsehandler```. The former sends command line input (requests) to the associated server thread, while the latter processes server responses. 

Raw string messages are wrapped in a ```packet``` header before being sent across the network. The header contains various control bits and the data payload. Note that the struct serialization is naive at present, but can be made more widely compatible later.


On the server side, each ```s_clienthandler``` thread processes messages messages from its associated client. Each thread implements logic to perform a different function based on the client request. I am currently modifying the server thread architecture to increase complexity.

Lastly, a downloadable server log file keeps track of all messages sent to the server along with timestamps and message details. 


## Functionality:
The server has various functionalities. Multiple clients can connect to and interact with the server concurrently. 
Some functions:
1. ```LIST``` : (done) Returns a list of connected clients to the requesting client.
2. ```EXIT``` : (done) Safely disconnects the requesting client from the server.
3. ```CHAT client1 client5 client3...``` : (in progress) Initiates a chat mode between requesting client and chat request client(s). A preliminary handshake is performed, inspired by the FTP protocol to confirm client availability. The goal is to implement multiple chat servers given a pool of clients connected to the server. Server maintains a log of all messages sent and received.
4. ```UP filename```   : (in progress) Uploads a file (any size/ type) to the server. Currently designing the protocol for .jpg files.
5. ```DOWN filename``` : (TO DO) Downloads a file (any size/type) from the server. Currently designing the protocol for .jpg files.

## Architecture Decisions
Server and all clients run _on the same machine_ as independent shell processes. As a result, there are no network-layer effects. Data is routed to and from local kernel buffers, instead of reaching the NIC.

The project is still in development. It has evolved from a simple single-client TCP echo server to a multiclient server with more complex functionalities. 

The most significant change has been the implementation of a ```packet``` struct for message passing instead of raw strings. This setup allows for more uniform control, which is required to implement certain FSMs in the applications.


### Application Layer Protocol:
- An advancement to the previous naive method of sending raw strings through sockets. Various changes imminent.
- Replaces the need for keyword processing and uses flags defined in a  ```packet``` struct. This struct will likely be modified in time.
- Methods implemented for chat handshake, file download/upload authentication. Can be made more featureful by adding encryption.

### Threads setup:
Server process listens for a connect request at the specified port, and spawns a new ```clienthandler``` thread to handle communication with the connected client.
Each client process, on initialisation spawns 2 threads: ```serverresponsehandler``` and ```clientinputhandler```. 
- ```serverresponsehandler``` implements an FSM based on certain ```packet``` flags. Used for chat authentication at present.
- ```clientinputhandler``` gets input from user terminal and sends it to the server for further processing.

### Server-Side User's List:
 - Each connected client is stored as a node in a Doubly-linked list of DLLNode structs.
 - Each node contains thread-local client details.
 - DLL Nodes are accessed instead of multiple File I/O operations to implement ```LIST```.


### Function Details:


## Drawbacks
- No performance testing/optimizations done as yet. 
- Program is not entirely thread-safe yet. Scalability is a concern and will likely lead to the discovery of concurrency bugs.
- Error checking not yet implemented for most functions.


