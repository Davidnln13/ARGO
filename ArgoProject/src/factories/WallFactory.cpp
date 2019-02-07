﻿#include "stdafx.h"
#include "WallFactory.h"

#include "components/Location.h"
#include "components/Dimensions.h"
#include "components/Animator.h"
#include "components/Render.h"

app::fact::WallFactory::WallFactory(app::math::Vector2f pos, app::math::Vector2f size)
	: m_texture(std::make_shared<decltype(m_texture)::element_type>()), m_position(pos), m_size(size)
{
	m_texture->load(m_renderer, "./res/image.png");
}

app::Entity const app::fact::WallFactory::create()
{
	app::Entity const entity = m_registry.create();

	auto location = comp::Location();
	location.position = m_position;
	location.orientation = 0.0f;
	m_registry.assign<decltype(location)>(entity, std::move(location));

	auto dimensions = comp::Dimensions();
	dimensions.size = m_size;
	dimensions.origin = dimensions.size / 2.0f;
	m_registry.assign<decltype(dimensions)>(entity, std::move(dimensions));

	auto render = comp::Render();
	render.texture = m_texture;
	m_registry.assign<decltype(render)>(entity, std::move(render));

	return entity;
}