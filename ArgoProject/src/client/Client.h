﻿#ifndef _CLIENT_H
#define _CLIENT_H
#include"stdafx.h"

namespace app::net
{

	enum Packet {
		P_CLIENT_NAME_STRING
	};
	class Client
	{
	public: // Constructors/Destructor/Assignments
		Client() = default;
		~Client() = default;

		Client(Client const &) = default;
		Client & operator=(Client const &) = default;

		Client(Client &&) = default;
		Client & operator=(Client &&) = default;

	public: // Public Static Functions
	public: // Public Member Functions
		void CloseSocket();
		bool InitNetwork(std::string const & pIP, int iPort);
		bool CheckSocket();
		bool processPacket(Packet _packetType);


		//Part of SendGetMethods cpp
		bool sendAll(char* data, int totalBytes);
		bool recvAll(char * data, int totalBytes);

		bool sendString(std::string& _string, app::net::Packet& _packetToProcessString);
		bool getString(std::string& _string);

		bool sendPacketType(Packet& _packetType);
		bool getPacketType(Packet& _packetType);

		bool sendInt(int _int);
		bool getInt(int& _int);

		bool sendPlayerName(std::string _name);
		
	public: // Public Static Variables
	public: // Public Member Variables
	protected: // Protected Static Functions
	protected: // Protected Member Functions
	protected: // Protected Static Variables
	protected: // Protected Member Variables
	private: // Private Static Functions
	private: // Private Member Functions
	private: // Private Static Variables
	private: // Private Member Variables


		TCPsocket socket;
		SDLNet_SocketSet socket_set;
	};
}

#endif // !_CLIENT_H
