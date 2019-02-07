﻿#ifndef _SCENE_MANAGER_H
#define _SCENE_MANAGER_H

#include "SplashScene.h"

namespace app::sce
{
	class SceneManager
	{
	protected: // Protected typedefs/Enums/Usings
		using Scene = std::variant<sce::SplashScene>;
	public: // Constructors/Destructor/Assignments
		SceneManager();
		~SceneManager();

		SceneManager(SceneManager const &) = default;
		SceneManager & operator=(SceneManager const &) = default;

		SceneManager(SceneManager &&) = default;
		SceneManager & operator=(SceneManager &&) = default;

	public: // Public Static Functions
	public: // Public Member Functions
		void update(app::time::seconds const & dt);
		void render(app::time::seconds const & dt);
	public: // Public Static Variables
	public: // Public Member Variables
	protected: // Protected Static Functions
	protected: // Protected Member Functions
	protected: // Protected Static Variables
	protected: // Protected Member Variables
	private: // Private Static Functions
	private: // Private Member Functions
		void changeScene();
	private: // Private Static Variables
		constexpr static bool DEBUG_MODE = true;
	private: // Private Member Variables
		std::unordered_map<SceneType, Scene> m_scenes;
		SceneType m_currentScene;
		SceneType m_targetScene;
	};
}

#endif // !_SCENE_MANAGER_H
