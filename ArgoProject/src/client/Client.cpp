﻿#include "stdafx.h"
#include "Client.h"

void app::client::Client::CloseSocket()
{
	if (SDLNet_TCP_DelSocket(socket_set, socket) == -1)
	{
		app::Console::writeLine({ "ERROR: SDLNet_TCP_DelSocket [", SDLNet_GetError(), "]" });
	}
	SDLNet_FreeSocketSet(socket_set);
	SDLNet_TCP_Close(socket);
}

void app::client::Client::SendData(uint8_t * data, uint16_t length, uint16_t flag)
{
	std::array<std::uint8_t, MAX_PACKET> temp_data;
	memcpy(temp_data.data(), &flag, sizeof(decltype(flag)));
	memcpy(temp_data.data() + sizeof(decltype(flag)), data, length);
	int const offset = sizeof(decltype(flag)) + length;

	int num_sent = SDLNet_TCP_Send(socket, temp_data.data(), offset);
	if (num_sent < offset)
	{
		Console::writeLine({ "ER: SDLNet_TCP_Send: [", SDLNet_GetError(), "]" });
		CloseSocket();
	}
}

uint8_t * app::client::Client::RecvData(uint16_t * length)
{
	app::Console::writeLine("received some data");
	std::array<std::uint8_t, MAX_PACKET> temp_data;
	int num_recv = SDLNet_TCP_Recv(socket, temp_data.data(), MAX_PACKET);
	if (num_recv <= 0)
	{
		CloseSocket();

		const char* err = SDLNet_GetError();
		if (strlen(err) == 0)
		{
			app::Console::writeLine({ "Server shutdown" });
		}
		else
		{
			app::Console::writeLine({ "ERROR: SDLNet_TCP_Recv: [",  SDLNet_GetError(), "]" });
		}
		return NULL;
	}
	else
	{
		*length = num_recv;
		
		uint8_t* data = (uint8_t*)malloc(num_recv * sizeof(uint8_t));
		memcpy(data, temp_data.data(), num_recv);
		return data;
	}
}

void app::client::Client::ProcessData(uint8_t * data, uint16_t * offset)
{
	//dont process if no data
	if (data == NULL) return;

	//get the flag from received buffer
	uint16_t flag = *(uint16_t*) &data[*offset];
	//offset now at start of data stream (size of flag)
	*offset += sizeof(uint16_t);
	 
	//for each flag type
	switch (flag)
	{
		case FLAG_WOOD_UPDATE:
		{
			app::Console::writeLine({ "Wood amount is currently: ", std::to_string(amt_wood) });
			//set wood by accessing data size of wood amount
			amt_wood += *data;
			*offset += sizeof(uint8_t);
			app::Console::writeLine({ "after server update: ", std::to_string(amt_wood) });

		}
		break;
		case FLAG_WOOD_GETTIME:
		{
			//set wood timer by accessing data size of timer
			timer_wood = *(uint32_t*) &data[*offset];
			*offset += sizeof(uint32_t);
		}
		break;
		case FLAG_WOOD_QUEST:
		{
			//NOTE: quest completed
			questing = 0;
		}
		break;
	}
}

void app::client::Client::InitNetwork(std::string const & pIP, int iPort)
{
	socket_set = SDLNet_AllocSocketSet(1);



	IPaddress ip;
	if (SDLNet_ResolveHost(&ip, pIP.c_str(), iPort) != NULL);
	{
		app::Console::writeLine({ "ERROR: SDLNet_ResolveHost: [", SDLNet_GetError(), "]" });
	}
	// Get our IP address in proper dot-quad format by breaking up the 32-bit unsigned host address and splitting it into an array of four 8-bit unsigned numbers...
	Uint8 * dotQuad = (Uint8*)&ip.host;
	//... and then outputting them cast to integers. Then read the last 16 bits of the serverIP object to get the port number
	std::cout << "Successfully resolved server host to IP: " << (unsigned short)dotQuad[0] << "." << (unsigned short)dotQuad[1] << "." << (unsigned short)dotQuad[2] << "." << (unsigned short)dotQuad[3];
	std::cout << " port " << SDLNet_Read16(&ip.port) << std::endl << std::endl;
	//open the servers socket

	socket = SDLNet_TCP_Open(&ip);
	if (socket == NULL)
	{
		app::Console::writeLine({ "ERROR: SDLNer_TCP_Open: [", SDLNet_GetError(), "]" });
	}

	if (SDLNet_TCP_AddSocket(socket_set, socket) == -1)
	{
		app::Console::writeLine({ "ERROR: SDLNet_TCP_AddSocket: [", SDLNet_GetError(), "]" });
	}
}

bool app::client::Client::CheckSocket()
{
	if (SDLNet_CheckSockets(socket_set, 0) == -1)
	{
		app::Console::writeLine({ "ERROR: SDLNet_CheckSockets [", SDLNet_GetError(), "]" });
	}
	return SDLNet_SocketReady(socket);
}
