#pragma once

#include "Core/Camera.h"
#include "Core/Light.h"
#include "Core/SceneNode.h"

#include <QopenGLFunctions_4_3_Core>
#include <QOpenGLShaderProgram>

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
