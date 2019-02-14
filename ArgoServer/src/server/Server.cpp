﻿#include "stdafx.h"
#include "Server.h"

/// <summary>
/// Constructor for server,
/// will call initialization of server
/// </summary>
/// <param name="_port">port to open the server on</param>
app::net::Server::Server(int _port)
	: m_clientThreads()
	, m_serverSocket(NULL)
	, m_socketSet(NULL)
	, m_sockets()
	, m_totalConnections(0)
	, m_lobbies()
{
	m_sockets.fill(NULL);
	initServer(_port);
}

app::net::Server::~Server()
{
	this->sdlCleanup();
}

/// <summary>
/// Method will listen for sockets connecting as well as check if any sockets sent anything.
/// 
/// </summary>
bool app::net::Server::listenForSockets()
{
	bool got_socket = acceptSocket(m_totalConnections);
	if (!got_socket)
	{
		//app::Console::writeLine({ "Failed to accept the clients connection" });
		return false;
	}
	else
	{
		app::Console::writeLine({ "Client connected! ID: ", std::to_string(m_totalConnections) });
		if (auto const & foundClient = m_clientThreads.find(m_totalConnections); foundClient != m_clientThreads.end())
		{
			// Client is already in the map
		}
		else
		{
			// Client is not in the map
			m_clientThreads.insert(std::make_pair(m_totalConnections, std::make_pair(std::nullopt, false)));
			auto &[thread, stopThread] = m_clientThreads.at(m_totalConnections);
			thread = std::thread(&app::net::Server::clientHandlerThread, this, m_totalConnections, std::ref(stopThread));
		}
		++m_totalConnections;
		return true;
	}
}


/// <summary>
/// Method to initialize the server.
/// creates server at passed port
/// </summary>
/// <param name="_port">port to connect to</param>
void app::net::Server::initServer(int _port)
{
	//Initialize SDL stuff
	if (SDL_Init(SDL_INIT_EVERYTHING) != NULL)
	{
		app::Console::writeLine({ "ERROR: Initializing SDL [", SDL_GetError(), "]" });
	}
	if (SDLNet_Init() != NULL)
	{
		app::Console::writeLine({ "ERROR: Initializing SDLNet [", SDLNet_GetError(), "]" });
	}

	//start up the server
	IPaddress ip;
	//attempt to resolve the host
	if (SDLNet_ResolveHost(&ip, NULL, 27000) == -1)
	{
		app::Console::writeLine({ "ERROR: SDLNet_ResolveHost: [", SDLNet_GetError(), "]" });
		exit(EXIT_FAILURE);
	}
	if (SDLNet_ResolveIP(&ip) == NULL) {
		printf("SDLNet_ResolveIP: %s\n", SDLNet_GetError());
		exit(EXIT_FAILURE);
	}

	outputIP(ip);

	//open the servers socket
	m_serverSocket = SDLNet_TCP_Open(&ip);
	if (m_serverSocket == NULL)
	{
		app::Console::writeLine({ "ERROR: SDL_Net_TCP_Open: [", SDLNet_GetError(), "]" });
	}
	//set and add server socket to socket set (+1 to account for server socket).
	m_socketSet = SDLNet_AllocSocketSet(s_MAX_SOCKETS + 1);
	if (m_socketSet == NULL)
	{
		app::Console::writeLine({ "ERROR: SDLNet_AllocSocketSet [", SDLNet_GetError(), "]" });
	}
	if (SDLNet_TCP_AddSocket(m_socketSet, m_serverSocket) == -1)
	{
		app::Console::writeLine({ "ERROR: SDLNet_TCP_AddSocket [" , SDLNet_GetError(), "]" });
	}
}

/// <summary>
/// This function will accept a socket trying to connect
/// </summary>
/// <param name="index">index of the socket</param>
/// <returns>true if accepted successfully, false if unsuccessful</returns>
bool app::net::Server::acceptSocket(int index)
{
	if (m_sockets[index])
	{
		app::Console::writeLine({ "Overriding a socket at index: ", std::to_string(index) });
	}

	m_sockets[index] = SDLNet_TCP_Accept(m_serverSocket);
	if (m_sockets[index] == NULL) return false;

	if (SDLNet_TCP_AddSocket(m_socketSet, m_sockets[index]) == -1)
	{
		app::Console::writeLine({ "ERROR: SDLNet_TCP_AddSocket [", SDLNet_GetError(), "]" });
	}
	return true;
}

/// <summary>
/// Close socket at index that is passed in.
/// </summary>
/// <param name="index">index at which socket we want to close in is</param>
void app::net::Server::closeSocket(int index)
{
	if (m_sockets[index] == NULL)
	{
		app::Console::writeLine({ "ERROR: Attempted to delete a NULL socket." });
		return;
	}
	if (SDLNet_TCP_DelSocket(m_socketSet, m_sockets[index]) == -1)
	{
		app::Console::writeLine({ "ERROR: SDLNet_TCP_DelSocket: [", SDLNet_GetError(), "]" });
	}
	SDLNet_TCP_Close(m_sockets[index]);
	m_sockets[index] = NULL;
}

/// <summary>
/// Clean up the SDL stuff when done
/// </summary>
void app::net::Server::sdlCleanup()
{
	for (auto &[id, mapValue] : m_clientThreads)
	{
		auto &[thread, stopThreadBool] = mapValue;
		if (thread.has_value())
		{
			stopThreadBool.store(true);
			if (thread->joinable()) { thread->join(); }
			thread.reset();
		}
	}

	//clean up everything when server is done.
	if (SDLNet_TCP_DelSocket(m_socketSet, m_serverSocket) == -1)
	{
		app::Console::writeLine({ "ERROR: SDLNet_TCP_DelSocket: [", SDLNet_GetError(), "]" });
	}
	SDLNet_TCP_Close(m_serverSocket);

	for (int i = 0; i < s_MAX_SOCKETS; ++i)
	{
		if (m_sockets[i] == NULL) continue;
		closeSocket(i);
	}

	SDLNet_FreeSocketSet(m_socketSet);
	SDLNet_Quit();
	SDL_Quit();
}

/// <summary>
/// Spin up a thread that will handle receiving and sending of packets.
/// </summary>
/// <param name="ID"></param>
void app::net::Server::clientHandlerThread(int ID, std::atomic<bool> & stopThread)
{
	Packet packetType;

	while (!stopThread.load())
	{
		if (!this->get(ID, packetType))
		{
			break;
		}
		if (!this->processPacket(ID, packetType))
		{
			break;
		}
	}
	app::Console::writeLine({ "Lost connection to client ID: ", std::to_string(ID) });
	this->closeSocket(ID);
}

/// <summary>
/// This method is used to receive all data,
/// it will keep calling receive until it gets the bytes it expects
/// </summary>
/// <param name="ID">ID of the socket to reveive from</param>
/// <param name="data">the data received from the other socket</param>
/// <param name="totalBytes">total bytes expected to receive from socket</param>
/// <returns>true if succeeds false if SDLNet_TCP_Recv returns error</returns>
bool app::net::Server::getAll(int ID, std::byte * data, int totalBytes)
{
	int bytesReceived = 0;
	while (bytesReceived < totalBytes)
	{
		int retnCheck = SDLNet_TCP_Recv(m_sockets[ID], data, totalBytes - bytesReceived);
		if (retnCheck <= 0)
		{
			return false;
		}
		bytesReceived += retnCheck;
	}
	return true;
}

/// <summary>
/// Send any type of data, it will call the send while there are still bytes to be sent.
/// </summary>
/// <param name="ID">ID of the socket to send to</param>
/// <param name="data">data to send to the socket</param>
/// <param name="totalBytes">total bytes we will send</param>
/// <returns>true if success, false if SDLNet_TCP_Send returns error</returns>
bool app::net::Server::sendAll(int ID, std::byte * data, int totalBytes)
{
	int bytesSent = 0;
	while (bytesSent < totalBytes)
	{
		int retnCheck = SDLNet_TCP_Send(m_sockets[ID], data + bytesSent, totalBytes - bytesSent);
		if (retnCheck < bytesSent)
		{
			return false;
		}
		bytesSent += retnCheck;
	}
	return true;
}

/// <summary>
/// Send an integer to another socket
/// </summary>
/// <param name="ID">ID of the socket to send to</param>
/// <param name="_int">integer to send</param>
/// <returns>true if success false if sendAll fails</returns>
bool app::net::Server::send(int ID, const int& _int)
{
	if (!sendAll(ID, (std::byte *)&_int, sizeof(int)))
	{
		return false;
	}
	return true;
}

/// <summary>
/// Await receiving of an integer from another socket.
/// </summary>
/// <param name="ID">ID of the socket to expect and int from</param>
/// <param name="_int">the int to assign received int to</param>
/// <returns>true if success, false if recvAll fails</returns>
bool app::net::Server::get(int ID, int & _int)
{
	if (!getAll(ID, (std::byte *)&_int, sizeof(int)))
	{
		return false;
	}
	return true;
}

/// <summary>
/// Send a packet type to another socket
/// </summary>
/// <param name="ID">ID of the socket to send to</param>
/// <param name="_packetType">packet type to send</param>
/// <returns>true if success, false if sendAll fails</returns>
bool app::net::Server::send(int ID, const Packet& _packetType)
{
	if (!sendAll(ID, (std::byte *)&_packetType, sizeof(Packet)))
	{
		return false;
	}
	return true;
}

/// <summary>
/// Expect a packet type from another socket
/// </summary>
/// <param name="ID">ID of the socket to expect a packet from</param>
/// <param name="_packetType">packet variable to assign the received packet type to</param>
/// <returns>true if success, false if recvAll fails</returns>
bool app::net::Server::get(int ID, Packet & _packetType)
{
	if (!getAll(ID, (std::byte *)&_packetType, sizeof(Packet)))
	{
		return false;
	}
	return true;
}

/// <summary>
/// Send a string to a socket.
/// Note the packet passed into this function should be a packet that processes a string ONLY
/// </summary>
/// <param name="ID">ID of the socket to send the string to</param>
/// <param name="_string">string to send</param>
/// <param name="_packetType">type of packet the other socket is to expect (defines how it will be processed by the other socket)</param>
/// <returns>true if success, false if any of the sends fail</returns>
bool app::net::Server::send(int ID, const std::string & _string, const Packet & _packetType)
{
	if (!send(ID, _packetType))
	{
		return false;
	}
	int bufferLen = _string.size();
	if (!send(ID, bufferLen))
	{
		return false;
	}
	if (!sendAll(ID, (std::byte *)&_string, sizeof(_string)))
	{
		return false;
	}
	return true;
}


/// <summary>
/// expect string from other socket
/// </summary>
/// <param name="ID">ID of the socket to get string from</param>
/// <param name="_string">string to assign the received string to</param>
/// <returns>true if success, false if any receives fail</returns>
bool app::net::Server::get(int ID, std::string & _string)
{
	int bufferLength;
	if (!get(ID, bufferLength))
	{
		return false;
	}
	std::byte * buffer = new std::byte[bufferLength + 1];
	buffer[bufferLength] = static_cast<std::byte>('\0');
	if (!getAll(ID, buffer, bufferLength))
	{
		delete[] buffer;
		return false;
	}
	_string = reinterpret_cast<char *>(buffer);
	delete[] buffer;
	return true;
}

/// <summary>
/// Process the packets that were received
/// </summary>
/// <param name="ID">ID of the socket the packet is from</param>
/// <param name="_packetType">type of packet received</param>
/// <returns>true if successful processing of packet, false if the processing fails</returns>
bool app::net::Server::processPacket(int ID, Packet _packetType)
{
	switch (_packetType)
	{
	case Packet::CLIENT_NAME:
	{
		std::string Message;
		if (!get(ID, Message))
		{
			return false;
		}
		if constexpr (s_DEBUG_MODE)
		{
			app::Console::writeLine(Message);
		}
		break;
	}
	break;
	case Packet::LOBBY_CREATE:
	{
		//handle creation of a lobby
		std::string playerName;
		if (!get(ID, playerName))
		{
			return false;
		}
		m_lobbies.push_back(Lobby());
		m_lobbies.back().setName(playerName + "'s Lobby");
		m_lobbies.back().addPlayer(ID, playerName);
		app::Console::writeLine({ "Player with ID [", std::to_string(ID), "] created a lobby with name: ", m_lobbies.back().getName() });

		//send out lobby created message to everyone else
		app::net::Packet packetType = app::net::Packet::LOBBY_CREATE;
		for (int i = 0; i < m_totalConnections; i++)
		{
			if (i == ID)
			{
				continue;
			}
			//send the vector of servers to client whenever a new server is added or just send a single lobby that was added.

		}

	}
	break;
	default:
		break;
	}
	return true;
}

void app::net::Server::outputIP(IPaddress const & ip)
{
	// Get our IP address in proper dot-quad format by breaking up the 32-bit unsigned host address and splitting it into an array of four 8-bit unsigned numbers...
	std::uint8_t const * dotQuad = reinterpret_cast<std::uint8_t const *>(&ip.host);
	
	auto const & toString = [](std::uint8_t const & num) { return std::to_string(static_cast<std::uint16_t>(num)); };
	//std::uint16_t
	//... and then outputting them cast to integers. Then read the last 16 bits of the serverIP object to get the port number
	app::Console::write({ "Successfully resolved server host to IP: ", toString(dotQuad[0]), ".", toString(dotQuad[1]), ".", toString(dotQuad[2]), ".", toString(dotQuad[3]) });
	app::Console::writeLine({ " port ", std::to_string(SDLNet_Read16(&ip.port)) });
	app::Console::writeLine();
}



