#include "Core/PBRMeshGraphics.h"
#include "Core/ResourceManager.h"
#include "Core/Logger.h"

#include <QOpenGLVertexArrayObject>

PBRMeshGraphics::PBRMeshGraphics(QOpenGLWidget& context, bool transparent, int layer)
	: GraphicsComponent(context, transparent, layer)
{
	this->setShaderLit("KLEIN_CookTorrance");
	this->setShaderUnlit("KLEIN_Unlit");
	_material = ResourceManager::instance().pbrMaterial("KLEIN_PBR_Default");
}

PBRMeshGraphics::~PBRMeshGraphics() = default;

bool PBRMeshGraphics::setPositionBuffer(const std::string& posBufID)
{
	auto buf = ResourceManager::instance().glBuffer(posBufID);
	if (buf != nullptr) {
		_posBuf = buf;
		return true;
	}
	else {
		return false;
	}
}

bool PBRMeshGraphics::setNormalBuffer(const std::string& normBufID)
{
	auto buf = ResourceManager::instance().glBuffer(normBufID);
	if (buf != nullptr) {
		_normBuf = buf;
		return true;
	}
	else {
		return false;
	}
}

bool PBRMeshGraphics::setMaterial(const std::string& materialID)
{
	auto material = ResourceManager::instance().pbrMaterial(materialID);
	if (material != nullptr) {
		_material = material;
		return true;
	}
	else {
		return false;
	}
}

void PBRMeshGraphics::setTransform(const QMatrix4x4& transform)
{
	_transform = transform;
}

void PBRMeshGraphics::_renderLit(const Camera& camera, const std::array<Light, KLEIN_MAX_LIGHTS>& lights)
{
	if (_posBuf == nullptr) {
		KLEIN_LOG_CRITICAL("Please set a valid position buffer before rendering");
		return;
	}
	if (_normBuf == nullptr) {
		KLEIN_LOG_CRITICAL("Please set a valid normal buffer before rendering");
		return;
	}

	if (this->shadingMethod() == ShadingMethod::shaded || this->shadingMethod() == ShadingMethod::hiddenLine) {
		_shaderLit->bind();
		QMatrix4x4 projection;
		projection.perspective(camera.fov(), camera.aspect(), camera.nearPlane(), camera.farPlane());
		auto mvp = projection * camera.view() * _transform;
		auto invTransModel = _transform.inverted().transposed();
		_shaderLit->setUniformValue("model", _transform);
		_shaderLit->setUniformValue("mvp", mvp);
		_shaderLit->setUniformValue("invTransModel", invTransModel);
		_shaderLit->setUniformValue("viewPosition_w", camera.position());
		for (size_t i = 0; i < KLEIN_MAX_LIGHTS; ++i) {
			auto lightPosition = QString("lightPosition_w[%1]").arg(i);
			auto lightColor = QString("lightColor[%1]").arg(i);
			_shaderLit->setUniformValue(lightPosition.toLatin1().data(), lights[i].position_w);
			_shaderLit->setUniformValue(lightColor.toLatin1().data(), lights[i].color);
		}
		_shaderLit->setUniformValue("alpha", this->transparency());
		_shaderLit->setUniformValue("material.albedo", _material->albedo);
		_shaderLit->setUniformValue("material.metallic", _material->metallic);
		_shaderLit->setUniformValue("material.roughness", _material->roughness);
		_shaderLit->setUniformValue("material.ao", _material->ao);

		QOpenGLVertexArrayObject vao;
		vao.create();
		vao.bind();
		_posBuf->bind();
		_shaderLit->setAttributeBuffer(0, GL_FLOAT, 0, 3);
		_shaderLit->enableAttributeArray(0);
		auto bufferSize = _posBuf->size();
		_posBuf->release();
		_normBuf->bind();
		_shaderLit->setAttributeBuffer(1, GL_FLOAT, 0, 3);
		_shaderLit->enableAttributeArray(1);
		_normBuf->release();

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawArrays(GL_TRIANGLES, 0, bufferSize / sizeof(QVector3D));

		vao.release();
		_shaderLit->release();
	}

	if (this->shadingMethod() == ShadingMethod::wireframe || this->shadingMethod() == ShadingMethod::hiddenLine) {
		_shaderUnlit->bind();
		QMatrix4x4 projection;
		projection.perspective(camera.fov(), camera.aspect(), camera.nearPlane(), camera.farPlane());
		auto mvp = projection * camera.view() * _transform;
		_shaderUnlit->setUniformValue("mvp", mvp);
		_shaderUnlit->setUniformValue("diffuseColor", QVector3D(1.0f, 1.0f, 1.0f));

		QOpenGLVertexArrayObject vao;
		vao.create();
		vao.bind();
		_posBuf->bind();
		_shaderUnlit->setAttributeBuffer(0, GL_FLOAT, 0, 3);
		_shaderUnlit->enableAttributeArray(0);
		auto bufferSize = _posBuf->size();
		_posBuf->release();

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glEnable(GL_POLYGON_OFFSET_LINE);
		glPolygonOffset(-1.0f, 1.0f);
		glDrawArrays(GL_TRIANGLES, 0, bufferSize / sizeof(QVector3D));
		glDisable(GL_POLYGON_OFFSET_LINE);

		vao.release();
		_shaderUnlit->release();
	}
}

void PBRMeshGraphics::_renderUnlit(const Camera & camera)
{
	if (_posBuf == nullptr) {
		KLEIN_LOG_CRITICAL("Please set a valid position buffer before rendering");
		return;
	}

	_shaderUnlit->bind();
	QMatrix4x4 projection;
	projection.perspective(camera.fov(), camera.aspect(), camera.nearPlane(), camera.farPlane());
	auto mvp = projection * camera.view() * _transform;
	_shaderUnlit->setUniformValue("mvp", mvp);

	QOpenGLVertexArrayObject vao;
	vao.create();
	vao.bind();
	_posBuf->bind();
	_shaderUnlit->setAttributeBuffer(0, GL_FLOAT, 0, 3);
	_shaderUnlit->enableAttributeArray(0);
	auto bufferSize = _posBuf->size();
	_posBuf->release();

	if (this->shadingMethod() == ShadingMethod::shaded || this->shadingMethod() == ShadingMethod::hiddenLine) {
		_shaderUnlit->setUniformValue("diffuseColor", _material->albedo);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawArrays(GL_TRIANGLES, 0, bufferSize / sizeof(QVector3D));
	}
	if (this->shadingMethod() == ShadingMethod::wireframe || this->shadingMethod() == ShadingMethod::hiddenLine) {
		_shaderUnlit->setUniformValue("diffuseColor", QVector3D(1.0f, 1.0f, 1.0f));
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glEnable(GL_POLYGON_OFFSET_LINE);
		glPolygonOffset(-1.0f, 1.0f);
		glDrawArrays(GL_TRIANGLES, 0, bufferSize / sizeof(QVector3D));
		glDisable(GL_POLYGON_OFFSET_LINE);
	}

	vao.release();
	_shaderUnlit->release();
}
