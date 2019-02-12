﻿#include "stdafx.h"


typedef struct {
	int in_use;
	int questing;
	uint8_t amt_wood;
	uint32_t timer_wood;
} Client;

const int MAX_PACKET = 255;
const int MAX_SOCKETS = 16;
const int WOOD_WAIT_TIME = 5000;
const int FLAG_QUIT = 0;
const int FLAG_WOOD_UPDATE = 16;

//index into the sockets and clients arrays for the next player that will connect to the server
int next_ind = 0;
//the servers socket
TCPsocket server_socket;
//array of clients
Client clients[MAX_SOCKETS];
//the socket set
SDLNet_SocketSet socket_set;
//array of sockets connected to server
TCPsocket sockets[MAX_SOCKETS];


/// <summary>
/// Function to close a socket from socket set
/// </summary>
/// <param name="index">index of the socket within the set</param>
void CloseSocket(int index)
{
	if (sockets[index] == NULL)
	{
		app::Console::writeLine({ "ERROR: Attempted to delete a NULL socket." });
		return;
	}
	if (SDLNet_TCP_DelSocket(socket_set, sockets[index]) == -1)
	{
		app::Console::writeLine({ "ERROR: SDLNet_TCP_DelSocket: [", SDLNet_GetError(), "]" });
	}
	memset(&clients[index], 0x00, sizeof(Client));
	SDLNet_TCP_Close(sockets[index]);
	sockets[index] = NULL;
}

/// <summary>
/// This function will accept a socket trying to connect
/// </summary>
/// <param name="index">index of the socket</param>
/// <returns>whether or not connection is successful</returns>
int AcceptSocket(int index)
{
	if (sockets[index])
	{
		app::Console::writeLine({ "Overriding a socket at index: ", std::to_string(index) });
	}

	sockets[index] = SDLNet_TCP_Accept(server_socket);
	if (sockets[index] == NULL) return 0;

	clients[index].in_use = 1;
	if (SDLNet_TCP_AddSocket(socket_set, sockets[index]) == -1)
	{
		app::Console::writeLine({ "ERROR: SDLNet_TCP_AddSocket [", SDLNet_GetError(), "]" });
	}
	return 1;
}

uint8_t* RecvData(int index, uint16_t* length, uint16_t* flag)
{
	uint8_t temp_data[MAX_PACKET];
	int num_recv = SDLNet_TCP_Recv(sockets[index], temp_data, MAX_PACKET);

	if (num_recv <= 0)
	{
		CloseSocket(index);
		const char* err = SDLNet_GetError();
		if (strlen(err) == 0)
		{
			app::Console::writeLine({ "Client disconnected" });
		}
		else
		{
			app::Console::writeLine({ "ERROR: SDL_Net_TCP_Recv: [", SDLNet_GetError(), "]" });
		}
		return NULL;
	}
	else
	{
		int offset = 0;
		*flag = *(uint16_t*) &temp_data[offset];
		offset += sizeof(uint16_t);

		*length = (num_recv - offset);
		uint8_t* data = (uint8_t*)malloc((num_recv - offset) * sizeof(uint8_t));
		memcpy(data, &temp_data[offset], (num_recv - offset));

		return data;
	}
}

/// <summary>
/// if client requests data from server, send it.
/// </summary>
/// <param name="index">index of client in socket set</param>
/// <param name="data">data to send</param>
/// <param name="length">length of buffer</param>
/// <param name="flag">flag for type</param>
void SendData(int index, uint8_t* data, uint16_t length, uint16_t flag)
{
	uint8_t temp_data[MAX_PACKET];

	int offset = 0;
	memcpy(temp_data + offset, &flag, sizeof(uint16_t));
	offset += sizeof(uint16_t);
	memcpy(temp_data + offset, data, length);
	offset += length;

	int num_sent = SDLNet_TCP_Send(sockets[index], temp_data, offset);
	if (num_sent < offset)
	{
		app::Console::writeLine({ "ERROR: SDLNet_TCP_Send: [", SDLNet_GetError(), "]" });
		CloseSocket(index);
	}

}


int main(int argv, char** argc)
{

	if (SDL_Init(SDL_INIT_EVERYTHING) != NULL)
	{
		app::Console::writeLine({ "ERROR: Initializing SDL [", SDL_GetError(), "]" });
	}
	if (SDLNet_Init() != NULL)
	{
		app::Console::writeLine({ "ERROR: Initializing SDLNet [", SDLNet_GetError(), "]" });
	}
	//ip of the pc server is on
	IPaddress ip;
	//attempt to resolve the host
	if (SDLNet_ResolveHost(&ip, NULL, 27000) == -1)
	{
		app::Console::writeLine({ "ERROR: SDLNet_ResolveHost: [", SDLNet_GetError(), "]" });
		exit(EXIT_FAILURE);
	}
	if (SDLNet_ResolveIP(&ip) == NULL) {
		printf("SDLNet_ResolveIP: %s\n", SDLNet_GetError());
		exit(1);
	}
	// Get our IP address in proper dot-quad format by breaking up the 32-bit unsigned host address and splitting it into an array of four 8-bit unsigned numbers...
	Uint8 * dotQuad = (Uint8*)&ip.host;

	//... and then outputting them cast to integers. Then read the last 16 bits of the serverIP object to get the port number
	std::cout << "Successfully resolved server host to IP: " << (unsigned short)dotQuad[0] << "." << (unsigned short)dotQuad[1] << "." << (unsigned short)dotQuad[2] << "." << (unsigned short)dotQuad[3];
	std::cout << " port " << SDLNet_Read16(&ip.port) << std::endl << std::endl;
	//open the servers socket
	server_socket = SDLNet_TCP_Open(&ip);
	if (server_socket == NULL)
	{
		app::Console::writeLine({ "ERROR: SDL_Net_TCP_Open: [", SDLNet_GetError(), "]" });
	}
	//set and add server socket to socket set.
	socket_set = SDLNet_AllocSocketSet(MAX_SOCKETS + 1);
	if (socket_set == NULL)
	{
		app::Console::writeLine({ "ERROR: SDLNet_AllocSocketSet [", SDLNet_GetError(), "]" });
	}
	if (SDLNet_TCP_AddSocket(socket_set, server_socket) == -1)
	{
		app::Console::writeLine({ "ERROR: SDLNet_TCP_AddSocket [" , SDLNet_GetError(), "]" });
	}

	//main processing loop
	int running = 1;
	while (running)
	{
		int num_rdy = SDLNet_CheckSockets(socket_set, 1000);
		if (num_rdy <= 0)
		{
			//NOTE: none of the sockets are ready.
			for (int ind = 0; ind < MAX_SOCKETS; ++ind)
			{
				if (!clients[ind].in_use) continue;
				clients[ind].amt_wood += 4;
			}
		}
		else
		{
			//NOTE: some number of sockets are ready.
			if (SDLNet_SocketReady(server_socket))
			{
				int got_socket = AcceptSocket(next_ind);
				if (!got_socket)
				{
					num_rdy--;
					continue;
				}
				//NOTE: get a new index.
				int chk_count;
				for (chk_count = 0; chk_count < MAX_SOCKETS; ++chk_count)
				{
					if (sockets[(next_ind + chk_count) % MAX_SOCKETS] == NULL) break;
				}
				next_ind = (next_ind + chk_count) % MAX_SOCKETS;
				app::Console::writeLine({ "new connection, next_ind: ", std::to_string(next_ind) });
				num_rdy--;
			}

			for (int ind = 0; ind < MAX_SOCKETS; ++ind)
			{
				if (sockets[ind] == NULL) continue;

				uint8_t* data;
				uint16_t flag;
				uint16_t length;

				data = RecvData(ind, &length, &flag);
				if (data == NULL)
				{
					num_rdy--;
					continue;
				}

				switch (flag)
				{
					case FLAG_WOOD_UPDATE: 
					{
						uint16_t offset = 0;
						uint8_t send_data[MAX_PACKET];

						memcpy(send_data + offset, &clients[ind].amt_wood, sizeof(uint8_t));
						offset += sizeof(uint8_t);

						SendData(ind, send_data, offset, FLAG_WOOD_UPDATE);
						app::Console::writeLine({ "sending request to update wood of client: ", std::to_string(ind) });
					}
						break;
					case FLAG_QUIT:
					{
						running = 0;
						app::Console::writeLine({ "Shutdown by client id: ", std::to_string(ind) });
					}
						break;
					default:
						break;
				}

				free(data);
				num_rdy--;
			}
		}
	}

	//clean up everything when server is done.
	if (SDLNet_TCP_DelSocket(socket_set, server_socket) == -1)
	{
		app::Console::writeLine({ "ERROR: SDLNet_TCP_DelSocket: [", SDLNet_GetError(), "]" });
	}
	SDLNet_TCP_Close(server_socket);

	for (int i = 0; i < MAX_SOCKETS; ++i)
	{
		if (sockets[i] == NULL) continue;
		CloseSocket(i);
	}

	SDLNet_FreeSocketSet(socket_set);
	SDLNet_Quit();
	SDL_Quit();

	return EXIT_SUCCESS;
}
