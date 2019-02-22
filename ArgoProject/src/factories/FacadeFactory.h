﻿#ifndef _FACTORY_FACADE_H
#define _FACTORY_FACADE_H

#include "EntityFactory.h"

namespace app::fact
{
	class FacadeFactory : public EntityFactory
	{
	public: // Constructors/Destructor/Assignments
		FacadeFactory(app::math::Vector2f const & pos, app::math::Vector2f const & size);
		~FacadeFactory() = default;

		FacadeFactory(FacadeFactory const &) = default;
		FacadeFactory & operator=(FacadeFactory const &) = default;

		FacadeFactory(FacadeFactory &&) = default;
		FacadeFactory & operator=(FacadeFactory &&) = default;

	public: // Public Static Functions
	public: // Public Member Functions
		virtual app::Entity const create() override;
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
		app::math::Vector2f const & m_position;
		app::math::Vector2f const & m_size;
	};
}

#endif // !_FACTORY_FACADE_H