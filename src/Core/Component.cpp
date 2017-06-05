#include "Core/Component.h"

unsigned Component::_id = 0;

Component::Component() = default;

Component::Component(SceneNode* node)
	: _node(node)
{
}

Component::~Component() = default;

void Component::attachTo(SceneNode* node)
{
	_node = node;
}

SceneNode* Component::sceneNode() const
{
	return _node;
}

unsigned Component::id() const
{
	return _id++;
}
