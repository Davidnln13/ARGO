﻿#ifndef _DEBUG_SYSTEM_H
#define _DEBUG_SYSTEM_H

#include "BaseSystem.h"
#include "client/Client.h"

namespace app::sys
{
	class DebugSystem : public BaseSystem
	{
	public: // Constructors/Destructor/Assignments
		DebugSystem();
		virtual ~DebugSystem() = default;

		DebugSystem(DebugSystem const &) = default;
		DebugSystem & operator=(DebugSystem const &) = default;

		DebugSystem(DebugSystem &&) = default;
		DebugSystem & operator=(DebugSystem &&) = default;


	public: // Public Static Functions
	public: // Public Member Functions
		virtual void update(app::time::seconds const & dt) override;
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
		app::inp::KeyHandler & m_keyHandler;
		app::client::Client m_client;
		bool updateVariable = false;

	};
}

#endif // !_DEBUG_SYSTEM_H
