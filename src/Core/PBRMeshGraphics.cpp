#include "Core/PBRMeshGraphics.h"
#include "Core/ResourceManager.h"
#include "Core/Logger.h"

#include <QOpenGLVertexArrayObject>

PBRMeshGraphics::PBRMeshGraphics(QOpenGLWidget& context, bool transparent, int layer)
	: GraphicsComponent(context, transparent, layer)
{
	_shader = ResourceManager::instance().shaderProgram("KLEIN_CookTorrance");
	_material = ResourceManager::instance().pbrMaterial("KLEIN_PBR_Default");
}

PBRMeshGraphics::~PBRMeshGraphics() = default;

void PBRMeshGraphics::render(const Camera& camera, const std::array<Light, KLEIN_MAX_LIGHTS>& lights)
{
	if (_posBuf == nullptr) {
		KLEIN_LOG_CRITICAL("Please set a valid position buffer before rendering");
		return;
	}
	if (_normBuf == nullptr) {
		KLEIN_LOG_CRITICAL("Please set a valid normal buffer before rendering");
		return;
	}
	
	_shader->bind();
	QMatrix4x4 projection;
	projection.perspective(camera.fov(), camera.aspect(), camera.nearPlane(), camera.farPlane());
	auto mvp = projection * camera.view() * _transform;
	auto invTransModel = _transform.inverted().transposed();
	_shader->setUniformValue("model", _transform);
	_shader->setUniformValue("mvp", mvp);
	_shader->setUniformValue("invTransModel", invTransModel);
	_shader->setUniformValue("viewPosition_w", camera.position());
	for (size_t i = 0; i < KLEIN_MAX_LIGHTS; ++i) {
		auto lightPosition = QString("lightPosition_w[%1]").arg(i);
		auto lightColor = QString("lightColor[%1]").arg(i);
		_shader->setUniformValue(lightPosition.toLatin1().data(), lights[i].position_w);
		_shader->setUniformValue(lightColor.toLatin1().data(), lights[i].color);
	}
	_shader->setUniformValue("alpha", this->transparency());

	QOpenGLVertexArrayObject vao;
	vao.create();
	vao.bind();
	_posBuf->bind();
	_shader->setAttributeBuffer(0, GL_FLOAT, 0, 3);
	_shader->enableAttributeArray(0);
	auto bufferSize = _posBuf->size();
	_posBuf->release();
	_normBuf->bind();
	_shader->setAttributeBuffer(1, GL_FLOAT, 0, 3);
	_shader->enableAttributeArray(1);
	_normBuf->release();

	if (this->shadingMethod() == ShadingMethod::shaded || this->shadingMethod() == ShadingMethod::hiddenLine) {
		_shader->setUniformValue("material.albedo", _material->albedo);
		_shader->setUniformValue("material.metallic", _material->metallic);
		_shader->setUniformValue("material.roughness", _material->roughness);
		_shader->setUniformValue("material.ao", _material->ao);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawArrays(GL_TRIANGLES, 0, bufferSize / sizeof(QVector3D));
	}
	if (this->shadingMethod() == ShadingMethod::wireframe || this->shadingMethod() == ShadingMethod::hiddenLine) {
		_shader->setUniformValue("material.albedo", QVector3D(1.0f, 1.0f, 1.0f));
		_shader->setUniformValue("material.metallic", 0.0f);
		_shader->setUniformValue("material.roughness", 1.0f);
		_shader->setUniformValue("material.ao", 1.0f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glEnable(GL_POLYGON_OFFSET_LINE);
		glPolygonOffset(-1.0f, 1.0f);
		glDrawArrays(GL_TRIANGLES, 0, bufferSize / sizeof(QVector3D));
		glDisable(GL_POLYGON_OFFSET_LINE);
	}
	vao.release();
	_shader->release();
}

bool PBRMeshGraphics::setShader(const std::string& shaderID)
{
	auto shader = ResourceManager::instance().shaderProgram(shaderID);
	if (shader != nullptr) {
		_shader = shader;
		return true;
	}
	else {
		return false;
	}
}

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
