#include "Core/PBRMeshGraphics.h"
#include "Core/GLBuffer.h"
#include "Core/SceneNode.h"
#include "Core/ResourceManager.h"
#include "Core/Logger.h"

#include <QOpenGLVertexArrayObject>

PBRMeshGraphics::PBRMeshGraphics(QOpenGLWidget& context, bool transparent, int layer)
	: GraphicsComponent(context, transparent, layer)
{
	this->setShaderLit("KLEIN_CookTorrance");
	this->setShaderUnlit("KLEIN_Unlit");
	_materialID = "KLEIN_PBR_Default";
}

unsigned PBRMeshGraphics::positionBuffer() const
{
	return _posBufID;
}

void PBRMeshGraphics::setPositionBuffer(unsigned posBufID)
{
	_posBufID = posBufID;
}

unsigned PBRMeshGraphics::normalBuffer() const
{
	return _normBufID;
}

void PBRMeshGraphics::setNormalBuffer(unsigned normBufID)
{
	_normBufID = normBufID;
}

std::string PBRMeshGraphics::material() const
{
	return _materialID;
}

void PBRMeshGraphics::setMaterial(const std::string& materialID)
{
	_materialID = materialID;
}

void PBRMeshGraphics::_renderLit(const Camera& camera, const std::array<Light, KLEIN_MAX_LIGHTS>& lights)
{
	auto posBuf = ResourceManager::instance().glBuffer(_posBufID);
	auto normBuf = ResourceManager::instance().glBuffer(_normBufID);
	auto material = ResourceManager::instance().pbrMaterial(_materialID);
	if (posBuf == nullptr) {
		KLEIN_LOG_CRITICAL("Please set a valid position buffer before rendering");
		return;
	}
	if (normBuf == nullptr) {
		KLEIN_LOG_CRITICAL("Please set a valid normal buffer before rendering");
		return;
	}
	if (material == nullptr) {
		KLEIN_LOG_CRITICAL("Please set a valid pbr material before rendering");
		return;
	}

	if (this->shadingMethod() == ShadingMethod::shaded || this->shadingMethod() == ShadingMethod::hiddenLine) {
		_shaderLit->bind();
		QMatrix4x4 projection;
		projection.perspective(camera.fov(), camera.aspect(), camera.nearPlane(), camera.farPlane());
		auto transform = this->sceneNode()->transform();
		auto mvp = projection * camera.view() * transform;
		auto invTransModel = transform.inverted().transposed();
		_shaderLit->setUniformValue("model", transform);
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
		_shaderLit->setUniformValue("material.albedo", material->albedo);
		_shaderLit->setUniformValue("material.metallic", material->metallic);
		_shaderLit->setUniformValue("material.roughness", material->roughness);
		_shaderLit->setUniformValue("material.ao", material->ao);

		QOpenGLVertexArrayObject vao;
		vao.create();
		vao.bind();
		posBuf->bind();
		_shaderLit->setAttributeBuffer(0, GL_FLOAT, 0, 3);
		_shaderLit->enableAttributeArray(0);
		auto bufferSize = posBuf->size();
		posBuf->release();
		normBuf->bind();
		_shaderLit->setAttributeBuffer(1, GL_FLOAT, 0, 3);
		_shaderLit->enableAttributeArray(1);
		normBuf->release();

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawArrays(GL_TRIANGLES, 0, bufferSize / sizeof(QVector3D));

		vao.release();
		_shaderLit->release();
	}

	if (this->shadingMethod() == ShadingMethod::wireframe || this->shadingMethod() == ShadingMethod::hiddenLine) {
		_shaderUnlit->bind();
		QMatrix4x4 projection;
		projection.perspective(camera.fov(), camera.aspect(), camera.nearPlane(), camera.farPlane());
		auto transform = this->sceneNode()->transform();
		auto mvp = projection * camera.view() * transform;
		_shaderUnlit->setUniformValue("mvp", mvp);
		_shaderUnlit->setUniformValue("diffuseColor", material->albedo);

		QOpenGLVertexArrayObject vao;
		vao.create();
		vao.bind();
		posBuf->bind();
		_shaderUnlit->setAttributeBuffer(0, GL_FLOAT, 0, 3);
		_shaderUnlit->enableAttributeArray(0);
		auto bufferSize = posBuf->size();
		posBuf->release();

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glEnable(GL_POLYGON_OFFSET_LINE);
		glPolygonOffset(-1.0f, 1.0f);
		glDrawArrays(GL_TRIANGLES, 0, bufferSize / sizeof(QVector3D));
		glDisable(GL_POLYGON_OFFSET_LINE);

		vao.release();
		_shaderUnlit->release();
	}
}

void PBRMeshGraphics::_renderUnlit(const Camera& camera)
{
	auto posBuf = ResourceManager::instance().glBuffer(_posBufID);
	auto material = ResourceManager::instance().pbrMaterial(_materialID);
	if (posBuf == nullptr) {
		KLEIN_LOG_CRITICAL("Please set a valid position buffer before rendering");
		return;
	}
	if (material == nullptr) {
		KLEIN_LOG_CRITICAL("Please set a valid pbr material before rendering");
		return;
	}

	_shaderUnlit->bind();
	QMatrix4x4 projection;
	projection.perspective(camera.fov(), camera.aspect(), camera.nearPlane(), camera.farPlane());
	auto transform = this->sceneNode()->transform();
	auto mvp = projection * camera.view() * transform;
	_shaderUnlit->setUniformValue("mvp", mvp);
	_shaderUnlit->setUniformValue("diffuseColor", material->albedo);

	QOpenGLVertexArrayObject vao;
	vao.create();
	vao.bind();
	posBuf->bind();
	_shaderUnlit->setAttributeBuffer(0, GL_FLOAT, 0, 3);
	_shaderUnlit->enableAttributeArray(0);
	auto bufferSize = posBuf->size();
	posBuf->release();

	if (this->shadingMethod() == ShadingMethod::shaded || this->shadingMethod() == ShadingMethod::hiddenLine) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawArrays(GL_TRIANGLES, 0, bufferSize / sizeof(QVector3D));
	}
	if (this->shadingMethod() == ShadingMethod::wireframe || this->shadingMethod() == ShadingMethod::hiddenLine) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glEnable(GL_POLYGON_OFFSET_LINE);
		glPolygonOffset(-1.0f, 1.0f);
		glDrawArrays(GL_TRIANGLES, 0, bufferSize / sizeof(QVector3D));
		glDisable(GL_POLYGON_OFFSET_LINE);
	}

	vao.release();
	_shaderUnlit->release();
}

void PBRMeshGraphics::_renderPickVertex(const Camera& camera)
{
	auto posBuf = ResourceManager::instance().glBuffer(_posBufID);
	if (posBuf == nullptr) {
		KLEIN_LOG_CRITICAL("Please set a valid position buffer before rendering");
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

	QOpenGLVertexArrayObject vao;
	vao.create();
	vao.bind();
	posBuf->bind();
	shaderPicking->setAttributeBuffer(0, GL_FLOAT, 0, 3);
	shaderPicking->enableAttributeArray(0);
	auto bufferSize = posBuf->size();
	posBuf->release();

	// Draw triangles first in order to cull points on the back faces
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glUniform1ui(bsLocation, 0); // Use native calls since Qt doesn't support uniform1ui
	glUniform1ui(ptLocation, PICKING_PRIMITIVE_NONE);
	glUniform1ui(nidLocation, 0);
	glUniform1ui(bidLocation, 0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0f, 1.0f);
	glDrawArrays(GL_TRIANGLES, 0, bufferSize / sizeof(QVector3D));
	glDisable(GL_POLYGON_OFFSET_FILL);
	glDisable(GL_CULL_FACE);

	// Then draw points
	glUniform1ui(bsLocation, posBuf->bufferSpec());
	glUniform1ui(ptLocation, PICKING_PRIMITIVE_VERTEX);
	glUniform1ui(nidLocation, this->sceneNode()->id());
	glUniform1ui(bidLocation, posBuf->bufferId());
	glEnable(GL_PROGRAM_POINT_SIZE);
	glPointSize(10);
	glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	glDrawArrays(GL_POINTS, 0, bufferSize / sizeof(QVector3D));
	glPointSize(1);
	glDisable(GL_PROGRAM_POINT_SIZE);
	
	vao.release();
	shaderPicking->release();
}

void PBRMeshGraphics::_renderPickFace(const Camera& camera)
{
	auto posBuf = ResourceManager::instance().glBuffer(_posBufID);
	if (posBuf == nullptr) {
		KLEIN_LOG_CRITICAL("Please set a valid position buffer before rendering");
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
	glUniform1ui(bsLocation, posBuf->bufferSpec()); // Use native calls since Qt doesn't support uniform1ui
	glUniform1ui(ptLocation, PICKING_PRIMITIVE_FACE);
	glUniform1ui(nidLocation, this->sceneNode()->id());
	glUniform1ui(bidLocation, posBuf->bufferId());

	QOpenGLVertexArrayObject vao;
	vao.create();
	vao.bind();
	posBuf->bind();
	shaderPicking->setAttributeBuffer(0, GL_FLOAT, 0, 3);
	shaderPicking->enableAttributeArray(0);
	auto bufferSize = posBuf->size();
	posBuf->release();

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, bufferSize / sizeof(QVector3D));
	glDisable(GL_CULL_FACE);

	vao.release();
	shaderPicking->release();
}
