#pragma once

#include "Camera.h"
#include "SceneNode.h"

#ifdef _WIN32
#include <CodeAnalysis/Warnings.h>
#pragma warning(push)
#pragma warning(disable : ALL_CODE_ANALYSIS_WARNINGS)
#endif

#include <QOpenGLFunctions_4_3_Core>
#include <QOpenGLShaderProgram>
#include <QVector3D>
#include <vector>

#ifdef _WIN32
#pragma warning(pop)
#endif

class QOpenGLWidget;

class PrimitiveNode : public SceneNode, protected QOpenGLFunctions_4_3_Core
{
public:
	PrimitiveNode(QOpenGLWidget* context, Scene* scene, SceneNode* parent, const QMatrix4x4& transform = QMatrix4x4());
	~PrimitiveNode();

	void addPoint(const QVector3D& p);
	void addPoint(float x, float y, float z);
	void addLine(const QVector3D& p1, const QVector3D& p2);
	void addLine(float x1, float y1, float z1, float x2, float y2, float z2);
	void addBox(const QVector3D& minCorner, float xLen, float yLen, float zLen);
	void addBox(const QVector3D& lbb, const QVector3D& lbf, // Left-Right Bottom-Top Back-Front
		const QVector3D& ltb, const QVector3D& ltf, const QVector3D& rbb,
		const QVector3D& rbf, const QVector3D& rtb, const QVector3D& rtf);
	void addSphere(const QVector3D& center, float radius);
	void setPointColor(const QVector3D& color);
	void setPointColor(float r, float g, float b);
	void setLineColor(const QVector3D& color);
	void setLineColor(float r, float g, float b);
	void render(QOpenGLShaderProgram& shader, const Camera& camera);

private:
	std::vector<QVector3D> _points;
	std::vector<QVector3D> _lines;
	std::vector<std::vector<QVector3D>> _lineLoops;
	QVector3D _pointColor;
	QVector3D _lineColor;
};
