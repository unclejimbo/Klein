#pragma once

#include "Core/Scene.h"

#include <QMatrix4x4>
#include <string>
#include <unordered_map>

class SceneNode
{
	friend class Scene;

public:
	SceneNode(Scene* scene, SceneNode* parent, const QMatrix4x4& transform = QMatrix4x4());
	virtual ~SceneNode();

	void setTransDirty(bool dirty);
	QMatrix4x4 transform() const;
	void setTransform(const QMatrix4x4& transform); // Cascading, lazy
	bool visible() const;
	void setVisible(bool visible); // Cascading, aggresive
	ShadingMethod shadingMethod() const;
	void setShadingMethod(ShadingMethod method); // Local
	int renderPass() const;
	void addRenderPass(int renderPass);
	void removeRenderPass(int renderPass);
	void setRenderPass(int renderPass); // Local

protected:
	Scene* _scene;

private:
	SceneNode* _parent;
	std::unordered_map<std::string, SceneNode*> _children;
	QMatrix4x4 _transform;
	mutable QMatrix4x4 _toWorld;
	bool _visible = true;
	mutable bool _dirty = false;
	ShadingMethod _method = ShadingMethod::global;
	int _renderPass = RenderPass::shading;
};
