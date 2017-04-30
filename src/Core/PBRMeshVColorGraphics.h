#pragma once

#include "Core/GraphicsComponent.h"
#include "Core/Material.h"

class QOpenGLBuffer;

class PBRMeshVColorGraphics : public GraphicsComponent
{
public:
	PBRMeshVColorGraphics(QOpenGLWidget& context, bool transparent = false, int layer = 0);
	PBRMeshVColorGraphics(SceneNode* node, QOpenGLWidget& context, bool transparent = false, int layer = 0);
	~PBRMeshVColorGraphics();

	bool setPositionBuffer(const std::string& posBufID);
	bool setNormalBuffer(const std::string& normBufID);
	bool setColorBuffer(const std::string& colorBufID);
	bool setMaterial(const std::string& materialID);

private:
	void _renderLit(const Camera& camera, const std::array<Light, KLEIN_MAX_LIGHTS>& lights) override;
	void _renderUnlit(const Camera& camera) override;

private:
	QOpenGLBuffer* _posBuf = nullptr;
	QOpenGLBuffer* _normBuf = nullptr;
	QOpenGLBuffer* _colorBuf = nullptr;
	PBRMaterial* _material;
};
