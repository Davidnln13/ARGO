﻿#include "stdafx.h"
#include "CameraSystem.h"

// components
#include "components/Location.h"
#include "components/Dimensions.h"
#include "components/Render.h"
#include "components/Camera.h"

app::sys::CameraSystem::CameraSystem()
	: BaseSystem() 
{

}

void app::sys::CameraSystem::update(app::time::seconds const & dt)
{
	auto targetView = m_registry.view<comp::Location>();
	m_registry.view<comp::Camera>()
		.each([&, this](app::Entity const camEntity, comp::Camera & camera)
	{
		if (camera.target.has_value())
		{
			auto const & targetLocation = targetView.get(camera.target.value());
			camera.position = targetLocation.position - (camera.size / 2.0f);
		}
	});
}
