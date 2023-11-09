#include "socket_facade.h"

/***************************************************
 * Function: socketCreate()
 * Purpose: Creates a socket for IPv4, TCP/IP.
 * Returns: The socket_desc on success, -1 on failure.
 **************************************************/
int socketCreate()
{
	int socket_desc;
	int optval = 1;

	if( (socket_desc = socket(AF_INET , SOCK_STREAM , IPPROTO_IP)) < 0 )
	{
		return errno;
	}	

	/* Make sure we can reuse this port a second time on a crash: */
	setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

	return socket_desc;
}

/******************************************************
 * Function: socketBind
 * Purpose: Bind the Socket to all interfaces and port number.
 * @Param socket_desc: Socket Descriptor 
 * @Param port: Bind to all interfaces @ port.
 * Returns: 0 on Success and errno on failure.
 * 
 ******************************************************/

int socketBind(int socket_desc, int port)
{
	struct sockaddr_in server;
	server.sin_addr.s_addr = htonl(INADDR_ANY); /* connect to any ip address associated with me, the server */
	server.sin_family = AF_INET;
    server.sin_port = htons( port ); /* convert port number to standard network format */
 	
	if(bind(socket_desc, (struct sockaddr*)&server, sizeof(server)) < 0)
	{
		return errno;
	}

	return 0;
}



/******************************************************
 * Function: socketListen
 * Purpose: Listen to socket (socket_desc).
 * @Param socket_desc: Socket Descriptor 
 * @Param max_num_clients: maximum connections.
 * Returns: 0 on Success and errno on failure.
 * 
 ******************************************************/

int socketListen(int socket_desc, int max_num_clients)
{
	if(listen(socket_desc, max_num_clients) < 0)
	{
		return errno;
	}

	return 0;
}


/******************************************************
 * Function: socketAccept
 * Purpose: Accept the incoming connection.
 * @Param socket_desc: Socket Descriptor 
 * Returns: connected socket descriptor on Success and errno on failure.
 ******************************************************/
int socketAccept(int socket_desc)
{
	int connected_socket_desc;
	if( (connected_socket_desc = accept(socket_desc, (struct sockaddr*)NULL, NULL)) < 0 )
	{
		return errno;
	}

	return connected_socket_desc;
}

/******************************************************
 * Function: socketConnect
 * Purpose: Request connection to server on ip_address:port.
 * @Param socket_desc: Socket Descriptor
 * @Param ip_address: IP address of server.
 * @Param port: port of server.
 * Returns: 0 on Success and errno on failure.
 ******************************************************/
int socketConnect(int socket_desc, char* ip_address, int port)
{
	struct sockaddr_in server;
	server.sin_addr.s_addr = inet_addr(ip_address); /* convert ip string to long */
	server.sin_family = AF_INET;
    server.sin_port = htons( port ); /* convert port number to standard network format */
 	
 	if (connect(socket_desc , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        return errno;
    }
 
 	return 0;  
}

/******************************************************
 * Function: socketConnect
 * Purpose: Disconnect the socket.
 * @Param socket_desc: Socket Descriptor 
 * Returns: 0 on Success and errno on failure.
 ******************************************************/

int socketDisconnect(int socket_desc)
{
	return close(socket_desc);
}


/******************************************************
 * Function: socketSend
 * Purpose: Send Message data to server through connected socket socket_desc.
 * @Param socket_desc: Socket Descriptor 
 * @Param message: data to be sent to server
 * @Param length: length of data to be sent
 * Returns: 0 on Success and errno on failure.
 ******************************************************/

int socketSend(int socket_desc, char* message, int length)
{
	if( length < 0 )
		length = strlen(message);

	if( send(socket_desc , message, length, 0) < 0)
    {
        return errno;
    }

    return 0;
}

/******************************************************
 * Function: socketReceive
 * Purpose: Receive Message data from client/server socket socket_desc.
 * @Param socket_desc: Socket Descriptor 
 * @Param data: data received from client/server
 * @Param length: length of data received data
 * @Param usec: Timeout, if -1 wait forever
 * Returns: 0 on Success and errno on failure.
 ******************************************************/

int socketReceive(int socket_desc, char* data, int length, int usecs)
{
	int ret;
	if(usecs == -1) {
		if( (ret = recv(socket_desc, data , length , 0)) <= 0) /* len of zero means connection closed */
	    {
	        return errno;
	    }
	}
	else {
		if( (ret = socketPoll(socket_desc, usecs)) <= 0 )
		{
			return errno;
		}

		return socketReceive_nowait( socket_desc, data, length);
	}

    return ret;
}


int socketReceive_nowait(int socket_desc, char* data, int length)
{
	int ret;

	if( (ret = recv(socket_desc, data , length , MSG_DONTWAIT)) <= 0) /* len of zero means connection closed */
    {
        return errno;
    }

    return ret;
}

/******************************************************
 * Function: socketPoll
 * Purpose: poll on socket till data received or timeout.
 * @Param socket_desc: Socket Descriptor 
 * @Param usec: timeout
 * Returns: 0 on Success and errno on failure.
 ******************************************************/
int socketPoll(int socket_desc, int usecs)
{
	/*
	// Wait for data in receive buffer or until time out before returning
	// Return number of bytes recieved when receive buffer has data
	// Return 0 on a timeoue
	// Return -1 on error
	*/
	int ret;
	struct pollfd pfd;
	struct timespec req;

	pfd.fd = socket_desc;
	pfd.events = POLLIN;

	req.tv_sec = usecs / 1000000;
	req.tv_nsec = (usecs % 1000000) * 1000L;

	if( (ret = ppoll(&pfd, 1, &req, NULL)) <= 0 )
	{
		return ret;
	}

	return ret;
}