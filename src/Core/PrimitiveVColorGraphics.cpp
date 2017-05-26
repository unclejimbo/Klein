#include "Core/PrimitiveVColorGraphics.h"
#include "Core/SceneNode.h"
#include "Core/ResourceManager.h"

#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>

PrimitiveVColorGraphics::PrimitiveVColorGraphics(QOpenGLWidget& context, bool transparent, int layer)
	: GraphicsComponent(context, transparent, layer)
{
	this->setShaderLit("KLEIN_Unlit_VColor");
	this->setShaderUnlit("KLEIN_Unlit_VColor");
}

PrimitiveVColorGraphics::PrimitiveVColorGraphics(QOpenGLWidget& context,
	GeomType geomType, unsigned geomID, bool transparent, int layer)
	: GraphicsComponent(context, geomType, geomID, transparent, layer)
{
	this->setShaderLit("KLEIN_Unlit_VColor");
	this->setShaderUnlit("KLEIN_Unlit_VColor");
}

PrimitiveVColorGraphics::~PrimitiveVColorGraphics() = default;

void PrimitiveVColorGraphics::addPoint(const QVector3D& point, const QVector3D& color)
{
	_pointPositions.push_back(point);
	_pointColors.push_back(color);
}

void PrimitiveVColorGraphics::addPoint(float x, float y, float z, float r, float g, float b)
{
	_pointPositions.emplace_back(x, y, z);
	_pointColors.emplace_back(r, g, b);
}

void PrimitiveVColorGraphics::addPoints(const std::vector<QVector3D>& points, const std::vector<QVector3D>& colors)
{
	_pointPositions.insert(_pointPositions.end(), points.begin(), points.end());
	_pointColors.insert(_pointColors.end(), colors.begin(), colors.end());
}

bool PrimitiveVColorGraphics::setPointPositionBuffer(const std::string& bufID)
{
	auto buf = ResourceManager::instance().glBuffer(bufID);
	if (buf != nullptr) {
		_pointPositionBuffer = buf;
		return true;
	}
	else {
		return false;
	}
}

bool PrimitiveVColorGraphics::setPointColorBuffer(const std::string& bufID)
{
	auto buf = ResourceManager::instance().glBuffer(bufID);
	if (buf != nullptr) {
		_pointColorBuffer = buf;
		return true;
	}
	else {
		return false;
	}
}

void PrimitiveVColorGraphics::setPointSize(short pointSize)
{
	_pointSize = pointSize;
}

void PrimitiveVColorGraphics::_renderLit(const Camera& camera, const std::array<Light, KLEIN_MAX_LIGHTS>& lights)
{
	_renderUnlit(camera);
}

void PrimitiveVColorGraphics::_renderUnlit(const Camera& camera)
{
	_shaderUnlit->bind();
	QMatrix4x4 projection;
	projection.perspective(camera.fov(), camera.aspect(), camera.nearPlane(), camera.farPlane());
	auto transform = this->sceneNode()->transform();
	auto mvp = projection * camera.view() * transform;
	_shaderUnlit->setUniformValue("mvp", mvp);

	QOpenGLVertexArrayObject vao;
	vao.create();
	vao.bind();

	// Draw points
	if (!_pointPositions.empty() && !_pointColors.empty() && _pointPositions.size() == _pointColors.size()) {
		QOpenGLBuffer vboPointPosition(QOpenGLBuffer::VertexBuffer);
		vboPointPosition.setUsagePattern(QOpenGLBuffer::DynamicDraw);
		vboPointPosition.create();
		vboPointPosition.bind();
		vboPointPosition.allocate(_pointPositions.data(), static_cast<int>(_pointPositions.size() * sizeof(QVector3D)));
		_shaderUnlit->enableAttributeArray(0);
		_shaderUnlit->setAttributeBuffer(0, GL_FLOAT, 0, 3);
		vboPointPosition.release();

		QOpenGLBuffer vboPointColor(QOpenGLBuffer::VertexBuffer);
		vboPointColor.setUsagePattern(QOpenGLBuffer::DynamicDraw);
		vboPointColor.create();
		vboPointColor.bind();
		vboPointColor.allocate(_pointColors.data(), static_cast<int>(_pointColors.size() * sizeof(QVector3D)));
		_shaderUnlit->enableAttributeArray(1);
		_shaderUnlit->setAttributeBuffer(1, GL_FLOAT, 0, 3);
		vboPointColor.release();

		glEnable(GL_PROGRAM_POINT_SIZE);
		glPointSize(_pointSize);
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		glDrawArrays(GL_POINTS, 0, static_cast<int>(_pointPositions.size()));
		glDisable(GL_PROGRAM_POINT_SIZE);
	}

	// Draw point buffer
	if (_pointPositionBuffer != nullptr && _pointColorBuffer != nullptr) {
		_pointPositionBuffer->bind();
		_shaderUnlit->enableAttributeArray(0);
		_shaderUnlit->setAttributeBuffer(0, GL_FLOAT, 0, 3);
		auto pbufsize = static_cast<int>(_pointPositionBuffer->size());
		_pointPositionBuffer->release();

		_pointColorBuffer->bind();
		_shaderUnlit->enableAttributeArray(1);
		_shaderUnlit->setAttributeBuffer(1, GL_FLOAT, 0, 3);
		auto cbufsize = static_cast<int>(_pointColorBuffer->size());
		_pointColorBuffer->release();

		if (pbufsize == cbufsize) {
			glEnable(GL_PROGRAM_POINT_SIZE);
			glPointSize(_pointSize);
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
			glDrawArrays(GL_POINTS, 0, pbufsize / sizeof(QVector3D));
			glDisable(GL_PROGRAM_POINT_SIZE);
		}
	}

	vao.release();
	_shaderUnlit->release();
}
