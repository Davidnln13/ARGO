﻿#include "stdafx.h"
#include "NetworkSystem.h"
#include "singletons/ClientSingleton.h"
#include "commands/buttons/ButtonLobbySelectCommand.h"
#include "factories/entities/LobbyDisplayFactory.h"

#include "components/LobbyDisplay.h"
#include "components/Widget.h"
#include "tags/LobbyTag.h"

app::sys::NetworkSystem::NetworkSystem(app::sce::SceneType & sceneControl)
	: BaseSystem()
	, m_client(app::sin::Client::get())
	, m_packetType()
	, m_sceneControl(sceneControl)
{
}

void app::sys::NetworkSystem::update(app::time::seconds const & dt)
{
	if (m_client.hasInit() && m_client.checkSocket())
	{
		m_packetType = app::net::PacketType::UNKNOWN;
		if (!m_client.get(m_packetType))
		{
			this->output("Failed to retrieve packet type");
		}
		if (m_client.processPacket(m_packetType))
		{
			this->output("Processed packet successfully");
			switch (m_sceneControl)
			{
				case app::sce::SceneType::LobbySelect:
					this->handlePacketLobbyWasCreated();
					this->handlePacketLobbyWasJoined();
				default:
					break;
			}
		}
		else
		{
			this->output("Failed to process packet");
		}
	}
}

void app::sys::NetworkSystem::output(std::string const & msg) const
{
	if constexpr (s_DEBUG_MODE)
	{
		app::Console::writeLine(msg);
	}
}

void app::sys::NetworkSystem::output(std::initializer_list<app::Console::Variant> const & msgs) const
{
	if constexpr (s_DEBUG_MODE)
	{
		app::Console::writeLine(msgs);
	}
}

void app::sys::NetworkSystem::handlePacketLobbyWasCreated()
{
	if (m_packetType != app::net::PacketType::LOBBY_WAS_CREATED) { return; }

	auto buttonView = m_registry.view<comp::Widget>();
	auto lobbyDisplayView = m_registry.view<comp::Widget, comp::LobbyDisplay>();
	auto entities = std::vector<app::Entity>();
	for (auto const & entity : buttonView)
	{
		if (!lobbyDisplayView.contains(entity)) { entities.push_back(entity); }
	}

	auto const & lobbies = m_client.getLobbies();
	auto params = par::LobbyDisplayFactoryParameters();
	params.position = math::Vector2f{ -450.0f, -300.0f };
	params.lobbies = lobbies;
	params.entities.insert(params.entities.end(), entities.begin(), entities.end());
	entities = fact::LobbyDisplayFactory(params, m_sceneControl).create();
}

void app::sys::NetworkSystem::handlePacketLobbyWasJoined()
{
	if (m_packetType != app::net::PacketType::LOBBY_JOINED) { return; }

	auto buttonView = m_registry.view<comp::Widget>();
	auto lobbyDisplayView = m_registry.view<comp::Widget, comp::LobbyDisplay>();
	auto entities = std::vector<app::Entity>();
	for (auto const & entity : buttonView)
	{
		if (!lobbyDisplayView.contains(entity)) { entities.push_back(entity); }
	}

	auto const & lobbies = m_client.getLobbies();
	auto params = par::LobbyDisplayFactoryParameters();
	params.position = math::Vector2f{ -450.0f, -300.0f };
	params.lobbies = lobbies;
	params.entities.insert(params.entities.end(), entities.begin(), entities.end());
	entities = fact::LobbyDisplayFactory(params, m_sceneControl).create();
}
