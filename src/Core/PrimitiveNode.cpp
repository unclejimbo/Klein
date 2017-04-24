#include "Core/PrimitiveNode.h"
#include "Core/ResourceManager.h"
#include "Core/Scene.h"

#include <QOpenGLWidget>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QtMath>

PrimitiveNode::PrimitiveNode(QOpenGLWidget* context, Scene* scene, SceneNode* parent, const QMatrix4x4& transform)
	: SceneNode(scene, parent, transform)
{
	context->makeCurrent();
	this->initializeOpenGLFunctions();
	context->doneCurrent();
}

PrimitiveNode::~PrimitiveNode() = default;

void PrimitiveNode::addPoint(const QVector3D& p)
{
	_points.push_back(p);
}

void PrimitiveNode::addPoint(float x, float y, float z)
{
	_points.emplace_back(x, y, z);
}

void PrimitiveNode::addLine(const QVector3D& p1, const QVector3D& p2)
{
	_lines.push_back(p1);
	_lines.push_back(p2);
}

void PrimitiveNode::addLine(float x1, float y1, float z1, float x2, float y2, float z2)
{
	_lines.emplace_back(x1, y1, z1);
	_lines.emplace_back(x2, y2, z2);
}

void PrimitiveNode::addBox(const QVector3D& minCorner, float xLen, float yLen, float zLen)
{
	_lines.push_back(minCorner);
	_lines.push_back(minCorner + QVector3D(xLen, 0.0f, 0.0f));
	_lines.push_back(minCorner);
	_lines.push_back(minCorner + QVector3D(0.0f, yLen, 0.0f));
	_lines.push_back(minCorner);
	_lines.push_back(minCorner + QVector3D(0.0f, 0.0f, zLen));
	_lines.push_back(minCorner + QVector3D(xLen, 0.0f, zLen));
	_lines.push_back(minCorner + QVector3D(0.0f, 0.0f, zLen));
	_lines.push_back(minCorner + QVector3D(xLen, 0.0f, zLen));
	_lines.push_back(minCorner + QVector3D(xLen, 0.0f, 0.0f));
	_lines.push_back(minCorner + QVector3D(xLen, 0.0f, zLen));
	_lines.push_back(minCorner + QVector3D(xLen, yLen, zLen));
	_lines.push_back(minCorner + QVector3D(0.0f, yLen, zLen));
	_lines.push_back(minCorner + QVector3D(xLen, yLen, zLen));
	_lines.push_back(minCorner + QVector3D(0.0f, yLen, zLen));
	_lines.push_back(minCorner + QVector3D(0.0f, yLen, 0.0f));
	_lines.push_back(minCorner + QVector3D(0.0f, yLen, zLen));
	_lines.push_back(minCorner + QVector3D(0.0f, 0.0f, zLen));
	_lines.push_back(minCorner + QVector3D(xLen, yLen, 0.0f));
	_lines.push_back(minCorner + QVector3D(0.0f, yLen, 0.0f));
	_lines.push_back(minCorner + QVector3D(xLen, yLen, 0.0f));
	_lines.push_back(minCorner + QVector3D(xLen, 0.0f, 0.0f));
	_lines.push_back(minCorner + QVector3D(xLen, yLen, 0.0f));
	_lines.push_back(minCorner + QVector3D(xLen, yLen, zLen));
}

void PrimitiveNode::addBox(const QVector3D& lbb, const QVector3D& lbf, const QVector3D& ltb, const QVector3D& ltf,
	const QVector3D& rbb, const QVector3D& rbf, const QVector3D& rtb, const QVector3D& rtf)
{
	_lines.push_back(lbb);
	_lines.push_back(lbf);
	_lines.push_back(lbb);
	_lines.push_back(ltb);
	_lines.push_back(lbb);
	_lines.push_back(rbb);
	_lines.push_back(rbf);
	_lines.push_back(lbf);
	_lines.push_back(rbf);
	_lines.push_back(rtf);
	_lines.push_back(rbf);
	_lines.push_back(rbb);
	_lines.push_back(ltf);
	_lines.push_back(rtf);
	_lines.push_back(ltf);
	_lines.push_back(lbf);
	_lines.push_back(ltf);
	_lines.push_back(ltb);
	_lines.push_back(rtb);
	_lines.push_back(ltb);
	_lines.push_back(rtb);
	_lines.push_back(rbb);
	_lines.push_back(rtb);
	_lines.push_back(rtf);
}

void PrimitiveNode::addSphere(const QVector3D& center, float radius)
{
	const int len = 200;
	std::vector<QVector3D> circle1(len);
	std::vector<QVector3D> circle2(len);
	std::vector<QVector3D> circle3(len);
	auto pi = qDegreesToRadians(360.0f);
	for (auto i = 0; i < len; ++i) {
		auto theta = i * 2 * pi / len;
		circle1[i] = center + QVector3D(static_cast<float>(qCos(theta)), static_cast<float>(qSin(theta)), 0.0f) * radius;
		circle2[i] = center + QVector3D(static_cast<float>(qCos(theta)), 0.0f, static_cast<float>(qSin(theta))) * radius;
		circle3[i] = center + QVector3D(0.0f, static_cast<float>(qCos(theta)), static_cast<float>(qSin(theta))) * radius;
	}
	_lineLoops.push_back(circle1);
	_lineLoops.push_back(circle2);
	_lineLoops.push_back(circle3);
}

void PrimitiveNode::setPointColor(const QVector3D& color)
{
	_pointColor = color;
}

void PrimitiveNode::setPointColor(float r, float g, float b)
{
	_pointColor = QVector3D(r, g, b);
}

void PrimitiveNode::setLineColor(const QVector3D& color)
{
	_lineColor = color;
}

void PrimitiveNode::setLineColor(float r, float g, float b)
{
	_lineColor = QVector3D(r, g, b);
}

void PrimitiveNode::render(QOpenGLShaderProgram& shader, const Camera& camera)
{
	QMatrix4x4 projection;
	projection.perspective(camera.fov(), camera.aspect(), 0.001f, 1000.0f);
	auto mvp = projection * camera.view() * this->transform();
	shader.setUniformValue("mvp", mvp);

	QOpenGLVertexArrayObject vao;
	vao.create();
	vao.bind();

	// Draw points
	QOpenGLBuffer vboPoint(QOpenGLBuffer::VertexBuffer);
	vboPoint.setUsagePattern(QOpenGLBuffer::DynamicDraw);
	vboPoint.create();
	vboPoint.bind();
	vboPoint.allocate(_points.data(), static_cast<int>(_points.size() * sizeof(QVector3D)));
	shader.setAttributeBuffer(0, GL_FLOAT, 0, 3);
	shader.enableAttributeArray(0);
	vboPoint.release();
	shader.setUniformValue("primitiveColor", _pointColor);
	glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	glDrawArrays(GL_POINTS, 0, static_cast<int>(_points.size()));

	// Draw lines
	QOpenGLBuffer vboLine(QOpenGLBuffer::VertexBuffer);
	vboLine.setUsagePattern(QOpenGLBuffer::DynamicDraw);
	vboLine.create();
	vboLine.bind();
	vboLine.allocate(_lines.data(), static_cast<int>(_lines.size() * sizeof(QVector3D)));
	shader.setAttributeBuffer(0, GL_FLOAT, 0, 3);
	shader.enableAttributeArray(0);
	vboLine.release();
	shader.setUniformValue("primitiveColor", _lineColor);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawArrays(GL_LINES, 0, static_cast<int>(_lines.size()));

	// Draw line loops
	for (const auto& loop : _lineLoops) {
		QOpenGLBuffer vboLoop(QOpenGLBuffer::VertexBuffer);
		vboLoop.setUsagePattern(QOpenGLBuffer::DynamicDraw);
		vboLoop.create();
		vboLoop.bind();
		vboLoop.allocate(loop.data(), static_cast<int>(loop.size() * sizeof(QVector3D)));
		shader.setAttributeBuffer(0, GL_FLOAT, 0, 3);
		shader.enableAttributeArray(0);
		vboLoop.release();
		glDrawArrays(GL_LINE_LOOP, 0, static_cast<int>(loop.size()));
	}

	vao.release();
}
