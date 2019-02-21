#ifndef _ANIMATED_IMAGE_FACTORY_H
#define _ANIMATED_IMAGE_FACTORY_H

#include "../EntityFactory.h"

namespace app::fact
{
	class AnimatedImageFactory : public EntityFactory
	{
	public: // Constructors/Destructor/Assignments
		AnimatedImageFactory(
			math::Vector2f const & position
			, math::Vector2f const & size
			, math::Vector2f const & origin
			, app::res::TextureKey const & textureKey
			, math::Vector2f const & frameSize
			, math::Vector2f const & noFrames);
		virtual ~AnimatedImageFactory() = default;
			
		AnimatedImageFactory(AnimatedImageFactory const &) = default;
		AnimatedImageFactory & operator=(AnimatedImageFactory const &) = default;

		AnimatedImageFactory(AnimatedImageFactory &&) = default;
		AnimatedImageFactory & operator=(AnimatedImageFactory &&) = default;

	public: // Public Static Functions
	public: // Public Member Functions
		virtual app::Entity const create() final override;
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
		math::Vector2f const & m_position;
		math::Vector2f const & m_size;
		math::Vector2f const & m_origin;
		app::res::TextureKey const & m_textureKey;
		math::Vector2f const & m_frameSize;
		math::Vector2f const & m_noFrames;
	};
}

#endif // !_ANIMATED_IMAGE_FACTORY_H
