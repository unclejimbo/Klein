#include "Core/Scene.h"
#include "Core/SceneNode.h"
#include "Core/Camera.h"
#include "Core/ResourceManager.h"
#include "Core/Logger.h"

Scene::Scene()
{
	_nodes["RootNode"] = std::make_unique<SceneNode>("RootNode", *this, nullptr);
	_root = _nodes["RootNode"].get();
	for (auto&& l : _lights) {
		l = { QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 0.0f, 0.0f) };
	}
}

Scene::~Scene() = default;

SceneNode* Scene::node(const std::string& name)
{
	if (_nodes.find(name) == _nodes.end()) {
		KLEIN_LOG_WARNING(QString("Can't find SceneNode %1").arg(name.c_str()));
		return nullptr;
	}
	else {
		return _nodes[name].get();
	}
}

SceneNode* Scene::node(unsigned id)
{
	for (auto& n : _nodes) {
		if (n.second->id() == id) {
			return n.second.get();
		}
	}
	KLEIN_LOG_WARNING(QString("Can't find SceneNode%1").arg(id));
	return nullptr;
}

SceneNode* Scene::rootNode()
{
	return _root;
}

SceneNode* Scene::addNode(const std::string& parentName, const std::string& name, const QMatrix4x4& transform)
{
	SceneNode* rtn = nullptr;
	if (_nodes.find(parentName) != _nodes.end()) {
		auto parent = _nodes[parentName].get();
		rtn = addNode(parent, name, transform);
	}
	else {
		KLEIN_LOG_CRITICAL(QString("Node %1 doesn't exist").arg(parentName.c_str()));
	}
	return rtn;
}

SceneNode* Scene::addNode(SceneNode* parent, const std::string& name, const QMatrix4x4& transform)
{
	SceneNode* rtn = nullptr;
	if (_nodes.find(name) != _nodes.end()) {
		KLEIN_LOG_CRITICAL(QString("Node %1 already exists").arg(name.c_str()));
	}
	else {
		_nodes[name] = std::make_unique<SceneNode>(name, *this, parent, transform);
		if (parent != nullptr) {
			parent->_children[name] = _nodes[name].get();
		}
		rtn = _nodes[name].get();
	}
	return rtn;
}

bool Scene::removeNode(const std::string& name)
{
	if (_nodes.find(name) == _nodes.end()) {
		KLEIN_LOG_WARNING(QString("Can't find SceneNode %1").arg(name.c_str()));
		return false;
	} else {
		// Remove all children
		while (!_nodes[name]->_children.empty()) { 
			auto child = _nodes[name]->_children.begin();
			while (!child->second->_children.empty()) { // Depth first
				child = child->second->_children.begin();
			}
			auto childName = child->first;

			// Remove child from cache
			auto graphics = child->second->graphicsComponent();
			if (graphics != nullptr) {
				if (graphics->transparent()) {
					_transparentGraphicsNodes.erase(childName);
				}
				else {
					_graphicsNodes.erase(childName);
				}
			}

			// Remove child from parent
			child->second->_parent->_children.erase(childName);

			// Remove self
			_nodes.erase(childName);
		}

		// Remove this from parent
		if (_nodes[name]->_parent != nullptr) {
			_nodes[name]->_parent->_children.erase(name);
		}

		// Remove this from cache
		auto graphics = _nodes[name]->graphicsComponent();
		if (graphics != nullptr) {
			if (graphics->transparent()) {
				_transparentGraphicsNodes.erase(name);
			}
			else {
				_graphicsNodes.erase(name);
			}
		}

		// Remove this
		_nodes.erase(name);
		return true;
	}
}

Camera* Scene::camera()
{
	return _camera.get();
}

void Scene::setCamera(const QVector3D& eye_w, const QVector3D& center_w, const QVector3D& up_w, float fov, float aspect)
{
	_camera = std::make_unique<Camera>(eye_w, center_w, up_w, fov, aspect);
}

bool Scene::setLight(int lightID, const QVector3D& position_w, const QVector3D& color)
{
	if (lightID > KLEIN_MAX_LIGHTS) {
		KLEIN_LOG_CRITICAL(QString("The maximum light number is ").append(KLEIN_MAX_LIGHTS));
		return false;
	}
	else {
		_lights[lightID] = { position_w, color };
		return true;
	}
}

void Scene::setShadingMethod(ShadingMethod shading)
{
	for (auto&& node : _graphicsNodes) {
		node.second->graphicsComponent()->setShadingMethod(shading);
	}
	for (auto&& node : _transparentGraphicsNodes) {
		node.second->graphicsComponent()->setShadingMethod(shading);
	}
}

void Scene::setPickingPrimitive(PickingPrimitive picking)
{
	_picking = picking;
}

void Scene::setUnlit(bool unlit)
{
	for (auto&& node : _graphicsNodes) {
		node.second->graphicsComponent()->setUnlit(unlit);
	}
	for (auto&& node : _transparentGraphicsNodes) {
		node.second->graphicsComponent()->setUnlit(unlit);
	}
}

void Scene::render(RenderPass renderPass)
{
	for (const auto& node : _graphicsNodes) {
		auto graphics = node.second->graphicsComponent();
		if (graphics->visible() && (graphics->renderPass() & renderPass)) {
			if (renderPass & RENDER_PICK) {
				graphics->renderPick(_picking, *_camera);
			}
			else {
				graphics->render(*_camera, _lights);
			}
		}
	}
	for (const auto& node : _transparentGraphicsNodes) {
		auto graphics = node.second->graphicsComponent();
		if (graphics->visible() && (graphics->renderPass() & renderPass)) {
			if (renderPass & RENDER_PICK) {
				graphics->renderPick(_picking, *_camera);
			}
			else {
				graphics->render(*_camera, _lights);
			}
		}
	}
}
