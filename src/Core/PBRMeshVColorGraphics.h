#pragma once

#include "Core/GraphicsComponent.h"

#include <QOpenGLVertexArrayObject>

class PBRMeshVColorGraphics : public GraphicsComponent
{
public:
	PBRMeshVColorGraphics(QOpenGLWidget& context, bool transparent = false, int layer = 0);

	unsigned positionBuffer() const;
	bool setPositionBuffer(unsigned bufferID);
	unsigned normalBuffer() const;
	bool setNormalBuffer(unsigned bufferID);
	unsigned colorBuffer() const;
	bool setColorBuffer(unsigned bufferID);
	std::string material() const;
	bool setMaterial(const std::string& materialID);

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
	unsigned _posCount = 0;
	unsigned _normCount = 0;
	unsigned _colorCount = 0;
	std::string _materialID;
	QOpenGLVertexArrayObject _vaoLit;
	QOpenGLVertexArrayObject _vaoUnlit;
	QOpenGLVertexArrayObject _vaoPick;
};
