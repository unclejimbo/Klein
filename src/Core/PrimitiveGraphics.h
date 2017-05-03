#pragma once

#include "Core/GraphicsComponent.h"

#include <QVector3D>
#include <vector>

class QOpenGLBuffer;

class PrimitiveGraphics : public GraphicsComponent
{
public:
	PrimitiveGraphics(QOpenGLWidget& context, bool transparent = false, int layer = 0);
	~PrimitiveGraphics();

	void addPoint(const QVector3D& point);
	void addPoint(float x, float y, float z);
	void addLine(const QVector3D& p1, const QVector3D& p2);
	void addLine(float x1, float y1, float z1, float x2, float y2, float z2);
	void addBox(const QVector3D& minCorner, float xLen, float yLen, float zLen);
	void addBox(const QVector3D& lbb, const QVector3D& lbf, // Left-Right Bottom-Top Back-Front
		const QVector3D& ltb, const QVector3D& ltf, const QVector3D& rbb,
		const QVector3D& rbf, const QVector3D& rtb, const QVector3D& rtf);
	void addSphere(const QVector3D& center, float radius);
	bool setPointPositionBuffer(const std::string& pointPosBufID);
	bool setLinePositionBuffer(const std::string& linePosBufID);
	void setColor(const QVector3D& color);

private:
	void _renderLit(const Camera& camera, const std::array<Light, KLEIN_MAX_LIGHTS>& lights) override;
	void _renderUnlit(const Camera& camera) override;

private:
	std::vector<QVector3D> _points;
	std::vector<QVector3D> _lines;
	std::vector<std::vector<QVector3D>> _lineLoops;
	QOpenGLBuffer* _pointPosBuf = nullptr;
	QOpenGLBuffer* _linePosBuf = nullptr;
	QVector3D _color = QVector3D(1.0f, 1.0f, 1.0f);
};
