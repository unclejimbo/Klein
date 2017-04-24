#pragma once

#include "Core/Camera.h"
#include "Core/Common.h"
#include "Core/Light.h"

#ifdef _WIN32
#include <CodeAnalysis/Warnings.h>
#pragma warning(push)
#pragma warning(disable : ALL_CODE_ANALYSIS_WARNINGS)
#endif

#include <array>
#include <memory>
#include <unordered_map>

#ifdef _WIN32
#pragma warning(pop)
#endif

class QOpenGLWidget;
class SceneNode;
class RenderMeshNode;
class VertexColorMeshNode;
class PrimitiveNode;

enum class ShadingMethod
{
	global,
	shaded,
	wireframe,
	hiddenLine
};

enum class SceneNodeType
{
	sceneNode,
	renderMeshNode,
	vertexColorMeshNode,
	alphaVertexColorMeshNode,
	primitiveNode
};

enum RenderPass
{
	none,
	shading,
	offscreen
};

class Scene
{
public:
	explicit Scene(QOpenGLWidget* context);
	~Scene();

	void resize(int width, int height);
	SceneNode* addNode(SceneNode* parent, SceneNodeType type, const std::string& name, const QMatrix4x4& transform = QMatrix4x4());
	SceneNode* node(const std::string& name);
	bool removeNode(const std::string& name);
	void addLight(const QVector3D& position_w, const QVector3D& color = QVector3D(1.0f, 1.0f, 1.0f));
	void setCamera(const QVector3D& eye_w, const QVector3D& center_w, const QVector3D& up_w, float fov);
	Camera* camera();
	SceneNode* rootNode();
	void setShadingMethod(ShadingMethod method);
	void render(RenderPass renderPass);

private:
	float _screenAspect;
	QOpenGLWidget* _context;
	std::unordered_map<std::string, std::unique_ptr<SceneNode>> _nodes;
	SceneNode* _root = nullptr;
	std::unordered_map<std::string, RenderMeshNode*> _meshNodes;
	std::unordered_map<std::string, VertexColorMeshNode*> _vertexColorMeshNodes;
	std::unordered_map<std::string, VertexColorMeshNode*> _alphaVertexColorMeshNodes; // TODO: Transparet objects should at least be sorted
	std::unordered_map<std::string, PrimitiveNode*> _primitiveNodes;
	std::unique_ptr<Camera> _camera = nullptr;
	std::array<Light, KLEIN_MAX_LIGHTS> _lights;
	short _lightCount = 0;
	ShadingMethod _method = ShadingMethod::shaded;
};
