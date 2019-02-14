﻿#include "stdafx.h"
#include "MotionSystem.h"

// components
#include "components/Location.h"
#include "components/Motion.h"
#include "components/AirMotion.h"
#include "components/PlatformDrop.h"
#include "components/Dimensions.h"

void app::sys::MotionSystem::update(app::time::seconds const & dt)
{
	m_registry.view<comp::Location, comp::Motion>()
		.each([&, this](app::Entity const entity, comp::Location & location, comp::Motion & motion)
	{
		auto const & velocity = (math::toVector(motion.direction) * motion.speed).truncate(comp::Motion::MAX_SPEED);
		if (motion.speed >= comp::Motion::DRAG_CUTOFF) 
		{
			//simulate drag
			motion.speed *= comp::Motion::DRAG;
		}
		else
		{
			motion.speed = 0;
		}
		location.position += velocity * dt.count();
		location.orientation += motion.angularSpeed * dt.count();
	});
}
