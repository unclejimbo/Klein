#pragma once

#include "Core/GraphicsComponent.h"

#include <QVector3D>
#include <QOpenGLVertexArrayObject>
#include <vector>
#include <set>

class GLBuffer;

class PrimitiveGraphics : public GraphicsComponent
{
public:
	PrimitiveGraphics(QOpenGLWidget& context, bool transparent = false, int layer = 0);

	void addPoint(const QVector3D& point);
	void addPoint(float x, float y, float z);
	void addLine(const QVector3D& p1, const QVector3D& p2);
	void addLine(float x1, float y1, float z1, float x2, float y2, float z2);
	void addBox(const QVector3D& minCorner, float xLen, float yLen, float zLen);
	void addBox(const QVector3D& lbb, const QVector3D& lbf, // Left-Right Bottom-Top Back-Front
		const QVector3D& ltb, const QVector3D& ltf, const QVector3D& rbb,
		const QVector3D& rbf, const QVector3D& rtb, const QVector3D& rtf);
	void addCircle(const QVector3D& center, const QVector3D& normal, float radius);
	void addTriangleFill(const QVector3D& p1, const QVector3D& p2, const QVector3D& p3);
	bool addPointPositionBuffer(unsigned pointPosBufID);
	bool addLinePositionBuffer(unsigned linePosBufID, unsigned specification);
	QVector3D color() const;
	void setColor(const QVector3D& color);
	void setColor(float r, float g, float b);
	short pointSize() const;
	void setPointSize(short pointSize);

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

private:
	std::vector<QVector3D> _points;
	std::vector<QVector3D> _lines;
	std::vector<std::vector<QVector3D>> _lineStrips;
	std::vector<std::vector<QVector3D>> _lineLoops;
	std::vector<QVector3D> _faces;
	std::set<unsigned> _pointBuffers;
	std::set<unsigned> _lineBuffers;
	std::set<unsigned> _lineStripBuffers;
	std::set<unsigned> _lineLoopBuffers;
	QOpenGLVertexArrayObject _vaoLit;
	QOpenGLVertexArrayObject _vaoUnlit;
	QOpenGLVertexArrayObject _vaoPick;
	QVector3D _color = QVector3D(1.0f, 1.0f, 1.0f);
	short _pointSize = 1;
};
