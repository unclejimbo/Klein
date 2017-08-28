#include "Core/PBRMeshGraphics.h"
#include "Core/GLBuffer.h"
#include "Core/SceneNode.h"
#include "Core/ResourceManager.h"
#include "Core/Logger.h"

PBRMeshGraphics::PBRMeshGraphics(QOpenGLWidget& context, bool transparent, int layer)
	: GraphicsComponent(context, transparent, layer)
{
	this->setShaderLit("KLEIN_CookTorrance");
	this->setShaderUnlit("KLEIN_Unlit");
	_materialID = "KLEIN_PBR_Default";

	context.makeCurrent();
	_vaoLit.create();
	_vaoUnlit.create();
	_vaoPick.create();
	context.doneCurrent();
}

unsigned PBRMeshGraphics::positionBuffer() const
{
	return _posBufID;
}

bool PBRMeshGraphics::setPositionBuffer(unsigned posBufID)
{
	auto posBuf = ResourceManager::instance().glBuffer(posBufID);
	if (posBuf == nullptr) {
		KLEIN_LOG_CRITICAL("Invalid position buffer");
		return false;
	}
	else {
		_posBufID = posBufID;
		this->context()->makeCurrent();

		_shaderLit->bind();
		_vaoLit.bind();
		posBuf->bind();
		_posCount = posBuf->size() / sizeof(QVector3D);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(0);
		posBuf->release();
		_vaoLit.release();
		_shaderLit->release();

		_shaderUnlit->bind();
		_vaoUnlit.bind();
		posBuf->bind();
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(0);
		posBuf->release();
		_vaoUnlit.release();
		_shaderUnlit->release();

		auto shaderPicking = ResourceManager::instance().shaderProgram("KLEIN_Picking");
		shaderPicking->bind();
		_vaoPick.bind();
		posBuf->bind();
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(0);
		posBuf->release();
		_vaoPick.release();
		shaderPicking->release();

		this->context()->doneCurrent();
		return true;
	}
}

unsigned PBRMeshGraphics::normalBuffer() const
{
	return _normBufID;
}

bool PBRMeshGraphics::setNormalBuffer(unsigned normBufID)
{
	auto normBuf = ResourceManager::instance().glBuffer(normBufID);
	if (normBuf == nullptr) {
		KLEIN_LOG_CRITICAL("Invalid normal buffer");
		return false;
	}
	else {
		_normBufID = normBufID;
		this->context()->makeCurrent();

		_shaderLit->bind();
		_vaoLit.bind();
		normBuf->bind();
		_normCount = normBuf->size() / sizeof(QVector3D);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(1);
		normBuf->release();
		_vaoLit.release();
		_shaderLit->release();

		this->context()->doneCurrent();
		return true;
	}
}

std::string PBRMeshGraphics::material() const
{
	return _materialID;
}

bool PBRMeshGraphics::setMaterial(const std::string& materialID)
{
	auto material = ResourceManager::instance().pbrMaterial(materialID);
	if (material == nullptr) {
		KLEIN_LOG_CRITICAL("Invalid material");
		return false;
	}
	else {
		_materialID = materialID;
		this->context()->makeCurrent();

		_shaderLit->bind();
		_shaderLit->setUniformValue("material.albedo", material->albedo);
		_shaderLit->setUniformValue("material.metallic", material->metallic);
		_shaderLit->setUniformValue("material.roughness", material->roughness);
		_shaderLit->setUniformValue("material.ao", material->ao);
		_shaderLit->release();

		_shaderUnlit->bind();
		_shaderUnlit->setUniformValue("diffuseColor", material->albedo);
		_shaderUnlit->release();

		this->context()->doneCurrent();
		return true;
	}
}

void PBRMeshGraphics::_renderLit(const Camera& camera,
	const std::array<Light, KLEIN_MAX_LIGHTS>& lights,
	float aspectRatio,
	MeshRenderMode meshRenderMode,
	PrimitiveRenderMode primitiveRenderMode)
{
	if (_posCount == 0 || _normCount == 0) {
		KLEIN_LOG_CRITICAL("Uninitialized vertex buffer");
		return;
	}
	if (_posCount != _normCount) {
		KLEIN_LOG_WARNING("Vertex buffers are different in size");
	}
	auto primCount = _posCount;
	auto material = ResourceManager::instance().pbrMaterial(_materialID);

	if (meshRenderMode == MeshRenderMode::shaded || meshRenderMode == MeshRenderMode::hiddenLine) {
		_shaderLit->bind();
		QMatrix4x4 projection;
		projection.perspective(camera.fov(), aspectRatio, camera.nearPlane(), camera.farPlane());
		auto transform = this->sceneNode()->transform();
		auto mvp = projection * camera.view() * transform;
		auto invTransModel = transform.inverted().transposed();
		_shaderLit->setUniformValue("model", transform);
		_shaderLit->setUniformValue("mvp", mvp);
		_shaderLit->setUniformValue("invTransModel", invTransModel);
		_shaderLit->setUniformValue("material.albedo", material->albedo);
		_shaderLit->setUniformValue("material.metallic", material->metallic);
		_shaderLit->setUniformValue("material.roughness", material->roughness);
		_shaderLit->setUniformValue("material.ao", material->ao);
		_shaderLit->setUniformValue("viewPosition_w", camera.position());
		for (size_t i = 0; i < KLEIN_MAX_LIGHTS; ++i) {
			auto lightPosition = QString("lightPosition_w[%1]").arg(i);
			auto lightColor = QString("lightColor[%1]").arg(i);
			_shaderLit->setUniformValue(lightPosition.toLatin1().data(), lights[i].position_w);
			_shaderLit->setUniformValue(lightColor.toLatin1().data(), lights[i].color);
		}
		_shaderLit->setUniformValue("alpha", this->transparency());

		_vaoLit.bind();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDrawArrays(GL_TRIANGLES, 0, primCount);
		glDisable(GL_BLEND);
		glDisable(GL_CULL_FACE);
		_vaoLit.release();

		_shaderLit->release();
	}

	if (meshRenderMode == MeshRenderMode::wireframe || meshRenderMode == MeshRenderMode::hiddenLine) {
		_shaderUnlit->bind();
		QMatrix4x4 projection;
		projection.perspective(camera.fov(), aspectRatio, camera.nearPlane(), camera.farPlane());
		auto transform = this->sceneNode()->transform();
		auto mvp = projection * camera.view() * transform;
		_shaderUnlit->setUniformValue("mvp", mvp);
		_shaderUnlit->setUniformValue("diffuseColor", material->albedo);

		_vaoUnlit.bind();
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glEnable(GL_POLYGON_OFFSET_LINE);
		glPolygonOffset(-2.0f, 0.1f);
		glDrawArrays(GL_TRIANGLES, 0, primCount);
		glDisable(GL_POLYGON_OFFSET_LINE);
		_vaoUnlit.release();

		_shaderUnlit->release();
	}
}

void PBRMeshGraphics::_renderUnlit(const Camera& camera, float aspectRatio,
	MeshRenderMode meshRenderMode, PrimitiveRenderMode primitiveRenderMode)
{
	if (_posCount == 0 || _normCount == 0) {
		KLEIN_LOG_CRITICAL("Uninitialized vertex buffer");
		return;
	}
	if (_posCount != _normCount) {
		KLEIN_LOG_WARNING("Vertex buffers are different in size");
	}
	auto primCount = _posCount;
	auto material = ResourceManager::instance().pbrMaterial(_materialID);

	_shaderUnlit->bind();
	QMatrix4x4 projection;
	projection.perspective(camera.fov(), aspectRatio, camera.nearPlane(), camera.farPlane());
	auto transform = this->sceneNode()->transform();
	auto mvp = projection * camera.view() * transform;
	_shaderUnlit->setUniformValue("mvp", mvp);
	_shaderUnlit->setUniformValue("diffuseColor", material->albedo);
	_vaoUnlit.bind();
	if (meshRenderMode == MeshRenderMode::shaded || meshRenderMode == MeshRenderMode::hiddenLine) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDrawArrays(GL_TRIANGLES, 0, primCount);
		glDisable(GL_BLEND);
		glDisable(GL_CULL_FACE);
	}
	if (meshRenderMode == MeshRenderMode::wireframe || meshRenderMode == MeshRenderMode::hiddenLine) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glEnable(GL_POLYGON_OFFSET_LINE);
		glPolygonOffset(-2.0f, 0.1f);
		glDrawArrays(GL_TRIANGLES, 0, primCount);
		glDisable(GL_POLYGON_OFFSET_LINE);
	}
	_vaoUnlit.release();
	_shaderUnlit->release();
}

void PBRMeshGraphics::_renderPickVertex(const Camera& camera, float aspectRatio)
{
	if (_posCount == 0) {
		KLEIN_LOG_CRITICAL("Uninitialized vertex buffer");
		return;
	}
	auto primCount = _posCount;
	auto posBuf = ResourceManager::instance().glBuffer(_posBufID);

	auto shaderPicking = ResourceManager::instance().shaderProgram("KLEIN_Picking");
	shaderPicking->bind();
	QMatrix4x4 projection;
	projection.perspective(camera.fov(), aspectRatio, camera.nearPlane(), camera.farPlane());
	auto transform = this->sceneNode()->transform();
	auto mvp = projection * camera.view() * transform;
	shaderPicking->setUniformValue("mvp", mvp);
	auto bsLocation = shaderPicking->uniformLocation("bufferSpec");
	auto ptLocation = shaderPicking->uniformLocation("primitiveType");
	auto nidLocation = shaderPicking->uniformLocation("nodeID");
	auto bidLocation = shaderPicking->uniformLocation("bufferID");
	_vaoPick.bind();

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
	glDrawArrays(GL_TRIANGLES, 0, primCount);
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
	glDrawArrays(GL_POINTS, 0, primCount);
	glPointSize(1);
	glDisable(GL_PROGRAM_POINT_SIZE);

	_vaoPick.release();
	shaderPicking->release();
}

void PBRMeshGraphics::_renderPickFace(const Camera& camera, float aspectRatio)
{
	if (_posCount == 0) {
		KLEIN_LOG_CRITICAL("Uninitialized vertex buffer");
		return;
	}
	auto primCount = _posCount;
	auto posBuf = ResourceManager::instance().glBuffer(_posBufID);

	auto shaderPicking = ResourceManager::instance().shaderProgram("KLEIN_Picking");
	shaderPicking->bind();
	QMatrix4x4 projection;
	projection.perspective(camera.fov(), aspectRatio, camera.nearPlane(), camera.farPlane());
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
	_vaoPick.bind();

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, primCount);
	glDisable(GL_CULL_FACE);

	_vaoPick.release();
	shaderPicking->release();
}
