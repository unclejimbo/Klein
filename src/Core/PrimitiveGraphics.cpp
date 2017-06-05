#include "Core/PrimitiveGraphics.h"
#include "Core/SceneNode.h"
#include "Core/ResourceManager.h"
#include "Core/Logger.h"

#include <QOpenGLVertexArrayObject>
#include <QtMath>

PrimitiveGraphics::PrimitiveGraphics(QOpenGLWidget& context, bool transparent, int layer)
	: GraphicsComponent(context, transparent, layer)
{
	this->setShaderLit("KLEIN_Unlit");
	this->setShaderUnlit("KLEIN_Unlit");
}

PrimitiveGraphics::~PrimitiveGraphics() = default;

void PrimitiveGraphics::addPoint(const QVector3D& point)
{
	_points.push_back(point);
}

void PrimitiveGraphics::addPoint(float x, float y, float z)
{
	_points.emplace_back(x, y, z);
}

void PrimitiveGraphics::addLine(const QVector3D& p1, const QVector3D& p2)
{
	_lines.push_back(p1);
	_lines.push_back(p2);
}

void PrimitiveGraphics::addLine(float x1, float y1, float z1, float x2, float y2, float z2)
{
	_lines.emplace_back(x1, y1, z1);
	_lines.emplace_back(x2, y2, z2);
}

void PrimitiveGraphics::addBox(const QVector3D& minCorner, float xLen, float yLen, float zLen)
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

void PrimitiveGraphics::addBox(const QVector3D& lbb, const QVector3D& lbf,
	const QVector3D& ltb, const QVector3D& ltf, const QVector3D& rbb, 
	const QVector3D& rbf, const QVector3D& rtb, const QVector3D& rtf)
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

void PrimitiveGraphics::addSphere(const QVector3D& center, float radius)
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

unsigned PrimitiveGraphics::pointPositionBuffer() const
{
	return _pointPosBufID;
}

void PrimitiveGraphics::setPointPositionBuffer(unsigned pointPosBufID)
{
	_pointPosBufID = pointPosBufID;
}

unsigned PrimitiveGraphics::linePositionBuffer() const
{
	return _linePosBufID;
}

void PrimitiveGraphics::setLinePositionBuffer(unsigned linePosBufID)
{
	_linePosBufID = linePosBufID;
}

QVector3D PrimitiveGraphics::color() const
{
	return _color;
}

void PrimitiveGraphics::setColor(const QVector3D& color)
{
	_color = color;
}

short PrimitiveGraphics::pointSize() const
{
	return _pointSize;
}

void PrimitiveGraphics::setPointSize(short pointSize)
{
	_pointSize = pointSize;
}

void PrimitiveGraphics::_renderLit(const Camera& camera, const std::array<Light, KLEIN_MAX_LIGHTS>& lights)
{
	_renderUnlit(camera);
}

void PrimitiveGraphics::_renderUnlit(const Camera& camera)
{

	_shaderUnlit->bind();
	QMatrix4x4 projection;
	projection.perspective(camera.fov(), camera.aspect(), camera.nearPlane(), camera.farPlane());
	auto transform = this->sceneNode()->transform();
	auto mvp = projection * camera.view() * transform;
	_shaderUnlit->setUniformValue("mvp", mvp);
	_shaderUnlit->setUniformValue("diffuseColor", _color);

	QOpenGLVertexArrayObject vao;
	vao.create();
	vao.bind();
	
	// Draw points
	if (!_points.empty()) {
		QOpenGLBuffer vboPoint(QOpenGLBuffer::VertexBuffer);
		vboPoint.setUsagePattern(QOpenGLBuffer::DynamicDraw);
		vboPoint.create();
		vboPoint.bind();
		vboPoint.allocate(_points.data(), static_cast<int>(_points.size() * sizeof(QVector3D)));
		_shaderUnlit->setAttributeBuffer(0, GL_FLOAT, 0, 3);
		_shaderUnlit->enableAttributeArray(0);
		vboPoint.release();
		glEnable(GL_PROGRAM_POINT_SIZE);
		glPointSize(_pointSize);
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		glDrawArrays(GL_POINTS, 0, static_cast<int>(_points.size()));
		glPointSize(1);
		glDisable(GL_PROGRAM_POINT_SIZE);
	}

	// Draw lines
	if (!_lines.empty()) {
		QOpenGLBuffer vboLine(QOpenGLBuffer::VertexBuffer);
		vboLine.setUsagePattern(QOpenGLBuffer::DynamicDraw);
		vboLine.create();
		vboLine.bind();
		vboLine.allocate(_lines.data(), static_cast<int>(_lines.size() * sizeof(QVector3D)));
		_shaderUnlit->setAttributeBuffer(0, GL_FLOAT, 0, 3);
		_shaderUnlit->enableAttributeArray(0);
		vboLine.release();
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawArrays(GL_LINES, 0, static_cast<int>(_lines.size()));
	}

	// Draw line loops
	if (!_lineLoops.empty()) {
		for (const auto& loop : _lineLoops) {
			QOpenGLBuffer vboLoop(QOpenGLBuffer::VertexBuffer);
			vboLoop.setUsagePattern(QOpenGLBuffer::DynamicDraw);
			vboLoop.create();
			vboLoop.bind();
			vboLoop.allocate(loop.data(), static_cast<int>(loop.size() * sizeof(QVector3D)));
			_shaderUnlit->setAttributeBuffer(0, GL_FLOAT, 0, 3);
			_shaderUnlit->enableAttributeArray(0);
			vboLoop.release();
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDrawArrays(GL_LINE_LOOP, 0, static_cast<int>(loop.size()));
		}
	}

	// Draw point buffer
	if (_pointPosBufID != 0) {
		auto pointPosBuf = ResourceManager::instance().glBuffer(_pointPosBufID);
		pointPosBuf->bind();
		_shaderUnlit->setAttributeBuffer(0, GL_FLOAT, 0, 3);
		_shaderUnlit->enableAttributeArray(0);
		auto bufsize = static_cast<int>(pointPosBuf->size());
		pointPosBuf->release();
		glEnable(GL_PROGRAM_POINT_SIZE);
		glPointSize(_pointSize);
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		glDrawArrays(GL_POINTS, 0, bufsize / sizeof(QVector3D));
		glPointSize(1);
		glDisable(GL_PROGRAM_POINT_SIZE);
	}

	// Draw line buffer
	if (_linePosBufID != 0) {
		auto linePosBuf = ResourceManager::instance().glBuffer(_linePosBufID);
		linePosBuf->bind();
		_shaderUnlit->setAttributeBuffer(0, GL_FLOAT, 0, 3);
		_shaderUnlit->enableAttributeArray(0);
		auto bufsize = static_cast<int>(linePosBuf->size());
		linePosBuf->release();
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawArrays(GL_LINES, 0, bufsize / sizeof(QVector3D));
	}

	vao.release();
	_shaderUnlit->release();
}

void PrimitiveGraphics::_renderPickVertex(const Camera& camera)
{
	auto pointPosBuf = ResourceManager::instance().glBuffer(_pointPosBufID);
	if (pointPosBuf == nullptr) {
		KLEIN_LOG_CRITICAL("There is no PointCloud associated with this component,\
			so it is impossible to do picking");
		return;
	}

	auto shaderPicking = ResourceManager::instance().shaderProgram("KLEIN_Picking");
	shaderPicking->bind();
	QMatrix4x4 projection;
	projection.perspective(camera.fov(), camera.aspect(), camera.nearPlane(), camera.farPlane());
	auto transform = this->sceneNode()->transform();
	auto mvp = projection * camera.view() * transform;
	shaderPicking->setUniformValue("mvp", mvp);
	auto bsLocation = shaderPicking->uniformLocation("bufferSpec");
	auto ptLocation = shaderPicking->uniformLocation("primitiveType");
	auto nidLocation = shaderPicking->uniformLocation("nodeID");
	auto bidLocation = shaderPicking->uniformLocation("bufferID");
	glUniform1ui(bsLocation, pointPosBuf->bufferSpec()); // Use native calls since Qt doesn't support uniform1ui
	glUniform1ui(ptLocation, PICKING_PRIMITIVE_VERTEX);
	glUniform1ui(nidLocation, this->sceneNode()->id());
	glUniform1ui(bidLocation, pointPosBuf->bufferId());

	QOpenGLVertexArrayObject vao;
	vao.create();
	vao.bind();
	pointPosBuf->bind();
	shaderPicking->setAttributeBuffer(0, GL_FLOAT, 0, 3);
	shaderPicking->enableAttributeArray(0);
	auto bufferSize = pointPosBuf->size();
	pointPosBuf->release();

	glEnable(GL_PROGRAM_POINT_SIZE);
	glPointSize(10);
	glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	glDrawArrays(GL_POINTS, 0, bufferSize / sizeof(QVector3D));
	glPointSize(1);
	glDisable(GL_PROGRAM_POINT_SIZE);

	vao.release();
	shaderPicking->release();
}
