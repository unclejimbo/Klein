#pragma once

#include "Core/GraphicsComponent.h"
#include "Core/Material.h"

#include <QOpenGLBuffer>
#include <QMatrix4x4>

class PBRMeshGraphics : public GraphicsComponent
{
public:
	PBRMeshGraphics(QOpenGLWidget& context, bool transparent = false, int layer = 0);
	~PBRMeshGraphics();

	bool setPositionBuffer(const std::string& posBufID);
	bool setNormalBuffer(const std::string& normBufID);
	bool setMaterial(const std::string& materialID);
	void setTransform(const QMatrix4x4& transform);

private:
	void _renderLit(const Camera& camera, const std::array<Light, KLEIN_MAX_LIGHTS>& lights) override;
	void _renderUnlit(const Camera& camera) override;
	
private:
	QOpenGLBuffer* _posBuf = nullptr;
	QOpenGLBuffer* _normBuf = nullptr;
	PBRMaterial* _material;
	QMatrix4x4 _transform;
};