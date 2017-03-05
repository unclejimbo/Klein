#include "Scene.h"

#include "Logger.h"
#include "PrimitiveNode.h"
#include "RenderMeshNode.h"
#include "ResourceManager.h"
#include "SceneNode.h"
#include "VertexColorMeshNode.h"

#ifdef _WIN32
#include <CodeAnalysis/Warnings.h>
#pragma warning(push)
#pragma warning(disable : ALL_CODE_ANALYSIS_WARNINGS)
#endif

#include <QOpenGLWidget>

#ifdef _WIN32
#pragma warning(pop)
#endif

Scene::Scene(QOpenGLWidget* context)
	: _screenAspect(context->width() / (context->height() + 0.00001f)), _context(context)
{
	auto root = std::make_unique<SceneNode>(this, nullptr);
	_root = root.get();
	_nodes["RootNode"] = std::move(root);
}

Scene::~Scene() = default;

void Scene::resize(int width, int height)
{
	_screenAspect = width / (height + 0.00001f);
}

SceneNode* Scene::addNode(SceneNode* parent, SceneNodeType type, const std::string& name, const QMatrix4x4& transform)
{
	SceneNode* rtn = nullptr;
	if (_nodes.find(name) != _nodes.end()) {
		KLEIN_LOG_CRITICAL(QString("Node %1 already exists").arg(name.c_str()));
	}
	else {
		switch (type) {
		case SceneNodeType::sceneNode:
		{
			auto node = std::make_unique<SceneNode>(this, parent, transform);
			rtn = node.get();
			if (parent != nullptr) {
				parent->_children[name] = node.get();
			}
			_nodes[name] = std::move(node);
			break;
		}
		case SceneNodeType::renderMeshNode:
		{
			auto node = std::make_unique<RenderMeshNode>(_context, this, parent, transform);
			rtn = node.get();
			_meshNodes[name] = node.get();
			if (parent != nullptr) {
				parent->_children[name] = node.get();
			}
			_nodes[name] = std::move(node);
			break;
		}
		case SceneNodeType::vertexColorMeshNode:
		{
			auto node = std::make_unique<VertexColorMeshNode>(_context, this, parent, transform);
			rtn = node.get();
			_vertexColorMeshNodes[name] = node.get();
			if (parent != nullptr) {
				parent->_children[name] = node.get();
			}
			_nodes[name] = std::move(node);
			break;
		}
		case SceneNodeType::alphaVertexColorMeshNode:
		{
			auto node = std::make_unique<VertexColorMeshNode>(_context, this, parent, transform);
			rtn = node.get();
			_alphaVertexColorMeshNodes[name] = node.get();
			if (parent != nullptr) {
				parent->_children[name] = node.get();
			}
			_nodes[name] = std::move(node);
			break;
		}
		case SceneNodeType::primitiveNode:
		{
			auto node = std::make_unique<PrimitiveNode>(_context, this, parent, transform);
			rtn = node.get();
			_primitiveNodes[name] = node.get();
			if (parent != nullptr) {
				parent->_children[name] = node.get();
			}
			_nodes[name] = std::move(node);
			break;
		}
		default:
			break;
		}
	}
	return rtn;
}

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
			if (typeid(*child->second) == typeid(RenderMeshNode)) {
				_meshNodes.erase(childName);
			}
			if (typeid(*child->second) == typeid(VertexColorMeshNode)) {
				if (_vertexColorMeshNodes.find(childName) != _vertexColorMeshNodes.end()) {
					_vertexColorMeshNodes.erase(childName);
				}
				else {
					_alphaVertexColorMeshNodes.erase(childName);
				}
			}
			if (typeid(*child->second) == typeid(PrimitiveNode)) {
				_primitiveNodes.erase(childName);
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
		if (typeid(*_nodes[name]) == typeid(RenderMeshNode)) { 
			_meshNodes.erase(name);
		}
		if (typeid(*_nodes[name]) == typeid(VertexColorMeshNode)) {
			if (_vertexColorMeshNodes.find(name) != _vertexColorMeshNodes.end()) {
				_vertexColorMeshNodes.erase(name);
			}
			else {
				_alphaVertexColorMeshNodes.erase(name);
			}
		}
		if (typeid(*_nodes[name]) == typeid(PrimitiveNode)) {
			_primitiveNodes.erase(name);
		}

		// Remove this
		_nodes.erase(name);
		return true;
	}
}

void Scene::addLight(const QVector3D& position_w, const QVector3D& color)
{
	if (_lightCount < KLEIN_MAX_LIGHTS) {
		_lights[_lightCount++] = { position_w, color };
	}
	else {
		KLEIN_LOG_WARNING(QString("Can't add more than %1 light sources").arg(KLEIN_MAX_LIGHTS));
	}
}

void Scene::setCamera(const QVector3D& eye_w, const QVector3D& center_w, const QVector3D& up_w, float fov)
{
	_camera = std::make_unique<Camera>(eye_w, center_w, up_w, fov, _screenAspect);
}

Camera* Scene::camera()
{
	return _camera.get();
}

SceneNode* Scene::rootNode()
{
	return _root;
}

void Scene::setShadingMethod(ShadingMethod method)
{
	_method = method;
}

void Scene::render(RenderPass renderPass)
{
	auto meshShader = ResourceManager::instance().shaderProgram("KLEIN_CookTorrance");
	meshShader->bind();
	for (const auto& node : _meshNodes) {
		if (node.second->visible() && node.second->renderPass() & renderPass) {
			if (node.second->shadingMethod() == ShadingMethod::global) {
				node.second->render(*meshShader, *_camera.get(), _lights, _method);
			}
			else {
				node.second->render(*meshShader, *_camera.get(), _lights, node.second->shadingMethod());
			}
		}
	}
	meshShader->release();

	auto vertexColorMeshShader = ResourceManager::instance().shaderProgram("KLEIN_VertexColorMesh");
	vertexColorMeshShader->bind();
	for (const auto& node : _vertexColorMeshNodes) {
		if (node.second->visible() && node.second->renderPass() & renderPass) {
			if (node.second->shadingMethod() == ShadingMethod::global) {
				node.second->render(*vertexColorMeshShader, *_camera.get(), _method);
			}
			else {
				node.second->render(*vertexColorMeshShader, *_camera.get(), node.second->shadingMethod());
			}
		}
	}
	vertexColorMeshShader->release();

	auto primitiveShader = ResourceManager::instance().shaderProgram("KLEIN_Primitive");
	primitiveShader->bind();
	for (const auto& node : _primitiveNodes) {
		if (node.second->visible() && node.second->renderPass() & renderPass) {
			node.second->render(*primitiveShader, *_camera.get());
		}
	}
	primitiveShader->release();

	vertexColorMeshShader->bind();
	for (const auto& node : _alphaVertexColorMeshNodes) {
		if (node.second->visible() && node.second->renderPass() & renderPass) {
			if (node.second->shadingMethod() == ShadingMethod::global) {
				node.second->render(*vertexColorMeshShader, *_camera.get(), _method);
			}
			else {
				node.second->render(*vertexColorMeshShader, *_camera.get(), _method);
			}
		}
	}
}
