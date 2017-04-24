#pragma once

#include "Core/Camera.h"
#include "Core/Light.h"
#include "Core/SceneNode.h"

#ifdef _WIN32
#include <CodeAnalysis/Warnings.h>
#pragma warning(push)
#pragma warning(disable : ALL_CODE_ANALYSIS_WARNINGS)
#endif

#include <QOpenGLFunctions_4_3_Core>
#include <QOpenGLShaderProgram>
#include <array>

#ifdef _WIN32
#pragma warning(pop)
#endif

class QOpenGLWidget;

class RenderMeshNode : public SceneNode, protected QOpenGLFunctions_4_3_Core
{
public:
	RenderMeshNode(QOpenGLWidget* context, Scene* scene, SceneNode* parent, const QMatrix4x4& transform = QMatrix4x4());
	~RenderMeshNode();

	void attachMesh(const std::string& vertexBuffer,
		const std::string& normalBuffer, const std::string& material = "KLEIN_Red");
	void setMaterial(const std::string& material);
	void render(QOpenGLShaderProgram& shader, const Camera& camera,
		const std::array<Light, KLEIN_MAX_LIGHTS>& lights, ShadingMethod method);

private:
	std::string _vertexBuffer;
	std::string _normalBuffer;
	std::string _material = "KLEIN_Red";
};
