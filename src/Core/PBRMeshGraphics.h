#pragma once

#include "Core/GraphicsComponent.h"
#include "Core/Material.h"

#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QMatrix4x4>

class PBRMeshGraphics : public GraphicsComponent
{
public:
	PBRMeshGraphics(QOpenGLWidget& context, bool transparent = false, int layer = 0);
	~PBRMeshGraphics();

	void render(const Camera& camera, const std::array<Light, KLEIN_MAX_LIGHTS>& lights);

	bool setShader(const std::string& shaderID);
	bool setPositionBuffer(const std::string& posBufID);
	bool setNormalBuffer(const std::string& normBufID);
	bool setMaterial(const std::string& materialID);
	void setTransform(const QMatrix4x4& transform);
	
private:
	QOpenGLShaderProgram* _shader;
	QOpenGLBuffer* _posBuf = nullptr;
	QOpenGLBuffer* _normBuf = nullptr;
	PBRMaterial* _material;
	QMatrix4x4 _transform;
};