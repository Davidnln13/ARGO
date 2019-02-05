﻿#ifndef _COMPONENTS_ANIMATOR_H
#define _COMPONENTS_ANIMATOR_H

namespace app::comp
{
	struct Animator
	{
	public: // Constructors/Destructor/Assignments
		Animator() = default;
		~Animator() = default;

		Animator(Animator const &) = default;
		Animator & operator=(Animator const &) = default;

		Animator(Animator &&) = default;
		Animator & operator=(Animator &&) = default;

	public: // Public Static Functions
	public: // Public Member Functions
	public: // Public Static Variables
	public: // Public Member Variables
		std::size_t currentFrame;
		float time, perFrame;
		std::vector<SDL_Rect> frames;
	protected: // Protected Static Functions
	protected: // Protected Member Functions
	protected: // Protected Static Variables
	protected: // Protected Member Variables
	private: // Private Static Functions
	private: // Private Member Functions
	private: // Private Static Variables
	private: // Private Member Variables
	};
}

#endif // !_COMPONENTS_ANIMATOR_H