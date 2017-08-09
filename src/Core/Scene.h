#pragma once

#include "Core/Common.h"
#include "Core/Light.h"
#include "Core/GraphicsComponent.h"
#include "Core/SceneNode.h"

#include <unordered_map>
#include <memory>
#include <array>

class Camera;

class Scene
{
	friend class SceneNode;

public:
	Scene();

	SceneNode* node(const std::string& name);
	SceneNode* node(unsigned id);
	SceneNode* rootNode();
	SceneNode* addNode(const std::string& parentName, const std::string& name, const QMatrix4x4& transform = QMatrix4x4());
	SceneNode* addNode(SceneNode* parent, const std::string& name, const QMatrix4x4& transform = QMatrix4x4());
	bool removeNode(const std::string& name);
	Camera* camera();
	void setCamera(const QVector3D& eye_w, const QVector3D& center_w, const QVector3D& up_w, float fov);
	bool setLight(int lightID, const QVector3D& position_w, const QVector3D& color = QVector3D(1.0f, 1.0f, 1.0f));
	void setShadingMethod(MeshShadingMethod shading);
	void setPickingPrimitive(PickingPrimitive picking);
	void setUnlit(bool unlit);
	void render(RenderPass renderPass, float aspectRatio);

private:
	std::unordered_map<std::string, std::unique_ptr<SceneNode>> _nodes;
	SceneNode* _root = nullptr;
	std::unordered_map<std::string, SceneNode*> _graphicsNodes;
	std::unordered_map<std::string, SceneNode*> _transparentGraphicsNodes;
	std::unique_ptr<Camera> _camera = nullptr;
	std::array<Light, KLEIN_MAX_LIGHTS> _lights;
	PickingPrimitive _picking = PICKING_PRIMITIVE_NONE;
};
