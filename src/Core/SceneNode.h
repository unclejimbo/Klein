#pragma once

#include "Core/Scene.h"

#include <QMatrix4x4>
#include <string>
#include <unordered_map>
#include <memory>

class GraphicsComponent;

class SceneNode
{
	friend class Scene;

public:
	SceneNode(const std::string& name, Scene& scene, SceneNode* parent, const QMatrix4x4& transform = QMatrix4x4());

	unsigned id() const;
	void setTransDirty(bool dirty);
	QMatrix4x4 transform() const;
	void setTransform(const QMatrix4x4& transform); // Cascading, lazy
	GraphicsComponent* graphicsComponent();
	void addGraphicsComponent(std::unique_ptr<GraphicsComponent> graphics);

private:
	std::string _name;
	Scene& _scene;
	SceneNode* _parent;
	std::unordered_map<std::string, SceneNode*> _children;
	QMatrix4x4 _transform;
	mutable QMatrix4x4 _toWorld;
	mutable bool _transDirty = false;
	std::unique_ptr<GraphicsComponent> _graphics = nullptr;
	unsigned _id;
	static unsigned _inc;
};
