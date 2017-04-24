#pragma once

#include "Core/Camera.h"
#include "Core/Light.h"
#include "Core/SceneNode.h"

#ifdef _WIN32
#include <CodeAnalysis/Warnings.h>
#pragma warning(push)
#pragma warning(disable : ALL_CODE_ANALYSIS_WARNINGS)
#endif

#include <QopenGLFunctions_4_3_Core>
#include <QOpenGLShaderProgram>

#ifdef _WIN32
#pragma warning(pop)
#endif

class QOpenGLWidget;

class VertexColorMeshNode : public SceneNode, protected QOpenGLFunctions_4_3_Core
{
public:
	VertexColorMeshNode(QOpenGLWidget* context, Scene* scene, SceneNode* parent, const QMatrix4x4& transform = QMatrix4x4());
	~VertexColorMeshNode();

	void attachMesh(const std::string& vertexBuffer, const std::string& colorBuffer);
	void setColor(const std::string& colorBuffer);
	void render(QOpenGLShaderProgram& shader, const Camera& camera, ShadingMethod method);

private:
	QOpenGLWidget* _context;
	std::string _vertexBuffer;
	std::string _colorBuffer;
};
