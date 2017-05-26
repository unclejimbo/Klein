#pragma once

#include "Core/GraphicsComponent.h"

#include <QVector3D>
#include <vector>

class QOpenGLBuffer;

class PrimitiveVColorGraphics : public GraphicsComponent
{
public:
	PrimitiveVColorGraphics(QOpenGLWidget& context, bool transparent = false, int layer = 0);
	PrimitiveVColorGraphics(QOpenGLWidget& context, GeomType geomType,
		unsigned geomID, bool transparent = false, int layer = 0);
	~PrimitiveVColorGraphics();

	void addPoint(const QVector3D& point, const QVector3D& color);
	void addPoint(float x, float y, float z, float r, float g, float b);
	void addPoints(const std::vector<QVector3D>& points, const std::vector<QVector3D>& colors);
	bool setPointPositionBuffer(const std::string& bufID);
	bool setPointColorBuffer(const std::string& bufID);
	void setPointSize(short pointSize);

private:
	void _renderLit(const Camera& camera, const std::array<Light, KLEIN_MAX_LIGHTS>& lights) override;
	void _renderUnlit(const Camera& camera) override;

private:
	std::vector<QVector3D> _pointPositions;
	std::vector<QVector3D> _pointColors;
	QOpenGLBuffer* _pointPositionBuffer = nullptr;
	QOpenGLBuffer* _pointColorBuffer = nullptr;
	short _pointSize = 1;
};
