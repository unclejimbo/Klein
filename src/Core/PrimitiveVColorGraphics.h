#pragma once

#include "Core/GraphicsComponent.h"

#include <QVector3D>
#include <vector>

class QOpenGLBuffer;

class PrimitiveVColorGraphics : public GraphicsComponent
{
public:
	PrimitiveVColorGraphics(QOpenGLWidget& context, bool transparent = false, int layer = 0);

	void addPoint(const QVector3D& point, const QVector3D& color);
	void addPoint(float x, float y, float z, float r, float g, float b);
	void addPoints(const std::vector<QVector3D>& points, const std::vector<QVector3D>& colors);
	unsigned pointPositionBuffer() const;
	void setPointPositionBuffer(unsigned bufID);
	unsigned pointColorBuffer() const;
	void setPointColorBuffer(unsigned bufID);
	short pointSize() const;
	void setPointSize(short pointSize);

private:
	void _renderLit(const Camera& camera,
		const std::array<Light, KLEIN_MAX_LIGHTS>& lights,
		float aspectRatio,
		MeshRenderMode meshRenderMode,
		PrimitiveRenderMode primitiveRendermode) override;
	void _renderUnlit(const Camera& camera,
		float aspectRatio,
		MeshRenderMode meshRenderMode,
		PrimitiveRenderMode primitiveRenderMode) override;

private:
	std::vector<QVector3D> _pointPositions;
	std::vector<QVector3D> _pointColors;
	unsigned _pointPositionBufferID = 0;
	unsigned _pointColorBufferID = 0;
	short _pointSize = 1;
};
