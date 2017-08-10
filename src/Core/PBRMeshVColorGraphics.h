#pragma once

#include "Core/GraphicsComponent.h"

class PBRMeshVColorGraphics : public GraphicsComponent
{
public:
	PBRMeshVColorGraphics(QOpenGLWidget& context, bool transparent = false, int layer = 0);

	unsigned positionBuffer() const;
	void setPositionBuffer(unsigned posBufID);
	unsigned normalBuffer() const;
	void setNormalBuffer(unsigned normBufID);
	unsigned colorBuffer() const;
	void setColorBuffer(unsigned colorBufID);
	std::string material() const;
	void setMaterial(const std::string& materialID);

private:
	void _renderLit(const Camera& camera,
		const std::array<Light, KLEIN_MAX_LIGHTS>& lights,
		float aspectRatio,
		MeshRenderMode meshRenderMode,
		PrimitiveRenderMode primitiveRenderMode) override;
	void _renderUnlit(const Camera& camera,
		float aspectRatio,
		MeshRenderMode meshRenderMode,
		PrimitiveRenderMode primitiveRenderMode) override;
	void _renderPickVertex(const Camera& camera, float aspectRatio) override;
	void _renderPickFace(const Camera& camera, float aspectRatio) override;

private:
	unsigned _posBufID = 0;
	unsigned _normBufID = 0;
	unsigned _colorBufID = 0;
	std::string _materialID;
};
