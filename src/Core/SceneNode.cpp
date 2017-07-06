#include "Core/SceneNode.h"
#include "Core/Scene.h"
#include "Core/GraphicsComponent.h"
#include "Core/Logger.h"

unsigned SceneNode::_inc = 1;

SceneNode::SceneNode(const std::string& name, Scene& scene, SceneNode* parent, const QMatrix4x4& transform)
	: _name(name), _scene(scene), _parent(parent), _id(_inc++)
{
	if (parent != nullptr) {
		_transform = transform;
		_toWorld = _parent->transform() * transform;
	}
	else {
		_toWorld = transform;
	}
}

unsigned SceneNode::id() const
{
	return _id;
}

void SceneNode::setTransDirty(bool dirty)
{
	_transDirty = dirty;
	if (dirty) {
		for (auto&& c : _children) {
			c.second->setTransDirty(true);
		}
	}
}

QMatrix4x4 SceneNode::transform() const
{
	if (_transDirty) {
		_toWorld = _parent->transform() * _transform;
		_transDirty = false;
	}
	return _toWorld;
}

void SceneNode::setTransform(const QMatrix4x4& transform)
{
	if (_parent != nullptr) {
		_transform = transform;
		_toWorld = _parent->transform() * transform;
	}
	else {
		_transform = transform;
		_toWorld = transform;
	}

	for (auto&& c : _children) {
		c.second->setTransDirty(true);
	}
}

GraphicsComponent* SceneNode::graphicsComponent()
{
	return _graphics.get();
}

void SceneNode::addGraphicsComponent(std::unique_ptr<GraphicsComponent> graphics)
{
	if (_graphics != nullptr) {
		KLEIN_LOG_WARNING("Existing graphics component is replaced");
	}
	_graphics = std::move(graphics);
	_graphics->attachTo(this);
	if (_graphics->transparent()) {
		_scene._transparentGraphicsNodes[_name] = this;
	}
	else {
		_scene._graphicsNodes[_name] = this;
	}
}
