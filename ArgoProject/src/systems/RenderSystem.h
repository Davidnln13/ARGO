﻿#ifndef _SYSTEM_RENDER_H
#define _SYSTEM_RENDER_H

#include "BaseSystem.h"
#include "graphics/Window.h"
#include "graphics/RenderRect.h"

namespace app::sys
{
	class RenderSystem : public BaseSystem
	{
	public: // Constructors/Destructor/Assignments
		RenderSystem();
		~RenderSystem() = default;

		RenderSystem(RenderSystem const &) = default;
		RenderSystem(RenderSystem &&) = default;

		RenderSystem & operator=(RenderSystem const &) = default;
		RenderSystem & operator=(RenderSystem &&) = default;

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
		app::gra::Window & m_window;
		app::gra::RenderRect m_renderRect;
		app::gra::View m_view;
	};
}

#endif // !_SYSTEM_RENDER_H
