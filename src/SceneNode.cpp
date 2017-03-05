#include "SceneNode.h"

#include "Logger.h"
#include "RenderMeshNode.h"
#include "Scene.h"

SceneNode::SceneNode(Scene* scene, SceneNode* parent, const QMatrix4x4& transform)
	: _scene(scene), _parent(parent)
{
	if (parent != nullptr) {
		_transform = transform;
		_toWorld = transform * _parent->transform();
	}
	else {
		_toWorld = transform;
	}
}

SceneNode::~SceneNode() = default;

void SceneNode::setTransDirty(bool dirty)
{
	_dirty = dirty;
	if (dirty) {
		for (auto&& c : _children) {
			c.second->setTransDirty(true);
		}
	}
}

QMatrix4x4 SceneNode::transform() const
{
	if (_dirty) {
		_toWorld = _parent->transform() * _transform;
		_dirty = false;
	}
	return _toWorld;
}

void SceneNode::setTransform(const QMatrix4x4& transform)
{
	if (_parent != nullptr) {
		_transform = transform;
		_toWorld = transform * _parent->transform();
	}
	else {
		_transform = transform;
		_toWorld = transform;
	}

	for (auto&& c : _children) {
		c.second->setTransDirty(true);
	}
}

bool SceneNode::visible() const
{
	if (!_visible) {
		return false;
	}
	else {
		if (_parent != nullptr && !_parent->visible()) {
			return false;
		}
		else {
			return true;
		}
	}
}

void SceneNode::setVisible(bool visible)
{
	_visible = visible;
}

ShadingMethod SceneNode::shadingMethod() const
{
	return _method;
}

void SceneNode::setShadingMethod(ShadingMethod method)
{
	_method = method;
}

int SceneNode::renderPass() const
{
	return _renderPass;
}

void SceneNode::addRenderPass(int renderPass)
{
	_renderPass = _renderPass | renderPass;
}

void SceneNode::removeRenderPass(int renderPass)
{
	_renderPass = _renderPass & (~renderPass);
}

void SceneNode::setRenderPass(int renderPass)
{
	_renderPass = renderPass;
}
