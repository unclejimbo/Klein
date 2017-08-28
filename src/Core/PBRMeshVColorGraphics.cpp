#include "Core/PBRMeshVColorGraphics.h"
#include "Core/GLBuffer.h"
#include "Core/SceneNode.h"
#include "Core/ResourceManager.h"
#include "Core/Logger.h"


PBRMeshVColorGraphics::PBRMeshVColorGraphics(QOpenGLWidget& context, bool transparent, int layer)
	: GraphicsComponent(context, transparent, layer)
{
	this->setShaderLit("KLEIN_CookTorrance_VColor");
	this->setShaderUnlit("KLEIN_Unlit_VColor");
	_materialID = "KLEIN_PBR_Default";

	context.makeCurrent();
	_vaoLit.create();
	_vaoUnlit.create();
	_vaoPick.create();
	context.doneCurrent();
}

unsigned PBRMeshVColorGraphics::positionBuffer() const
{
	return _posBufID;
}

bool PBRMeshVColorGraphics::setPositionBuffer(unsigned bufferID)
{
	auto buffer = ResourceManager::instance().glBuffer(bufferID);
	if (buffer == nullptr) {
		KLEIN_LOG_CRITICAL("Invalid position buffer");
		return false;
	}
	else {
		_posBufID = bufferID;
		this->context()->makeCurrent();

		_shaderLit->bind();
		_vaoLit.bind();
		buffer->bind();
		_posCount = buffer->size() / sizeof(QVector3D);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(0);
		buffer->release();
		_vaoLit.release();
		_shaderLit->release();

		_shaderUnlit->bind();
		_vaoUnlit.bind();
		buffer->bind();
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(0);
		buffer->release();
		_vaoUnlit.release();
		_shaderUnlit->release();

		auto shaderPicking = ResourceManager::instance().shaderProgram("KLEIN_Picking");
		shaderPicking->bind();
		_vaoPick.bind();
		buffer->bind();
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(0);
		buffer->release();
		_vaoPick.release();
		shaderPicking->release();

		this->context()->doneCurrent();
		return true;
	}
}

unsigned PBRMeshVColorGraphics::normalBuffer() const
{
	return _normBufID;
}

bool PBRMeshVColorGraphics::setNormalBuffer(unsigned bufferID)
{
	auto buffer = ResourceManager::instance().glBuffer(bufferID);
	if (buffer == nullptr) {
		KLEIN_LOG_CRITICAL("Invalid normal buffer");
		return false;
	}
	else {
		_normBufID = bufferID;
		this->context()->makeCurrent();

		_shaderLit->bind();
		_vaoLit.bind();
		buffer->bind();
		_normCount = buffer->size() / sizeof(QVector3D);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(1);
		buffer->release();
		_vaoLit.release();
		_shaderLit->release();

		_shaderUnlit->bind();
		_vaoUnlit.bind();
		buffer->bind();
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(1);
		buffer->release();
		_vaoUnlit.release();
		_shaderUnlit->release();

		this->context()->doneCurrent();
		return true;
	}
}

unsigned PBRMeshVColorGraphics::colorBuffer() const
{
	return _colorBufID;
}

bool PBRMeshVColorGraphics::setColorBuffer(unsigned bufferID)
{
	auto buffer = ResourceManager::instance().glBuffer(bufferID);
	if (buffer == nullptr) {
		KLEIN_LOG_CRITICAL("Invalid color buffer");
		return false;
	}
	else {
		_colorBufID = bufferID;
		this->context()->makeCurrent();

		_shaderLit->bind();
		_vaoLit.bind();
		buffer->bind();
		_colorCount = buffer->size() / sizeof(QVector3D);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(2);
		buffer->release();
		_vaoLit.release();
		_shaderLit->release();

		_shaderUnlit->bind();
		_vaoUnlit.bind();
		buffer->bind();
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(2);
		buffer->release();
		_vaoUnlit.release();
		_shaderUnlit->release();

		this->context()->doneCurrent();
		return true;
	}
}

std::string PBRMeshVColorGraphics::material() const
{
	return _materialID;
}

bool PBRMeshVColorGraphics::setMaterial(const std::string& materialID)
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
		_shaderLit->setUniformValue("material.metallic", material->metallic);
		_shaderLit->setUniformValue("material.roughness", material->roughness);
		_shaderLit->setUniformValue("material.ao", material->ao);
		_shaderLit->release();

		this->context()->doneCurrent();
		return true;
	}
}

void PBRMeshVColorGraphics::_renderLit(const Camera& camera,
	const std::array<Light, KLEIN_MAX_LIGHTS>& lights,
	float aspectRatio,
	MeshRenderMode meshRenderMode,
	PrimitiveRenderMode primitiveRenderMode)
{
	if (_posCount == 0 || _normCount == 0 || _colorCount == 0) {
		KLEIN_LOG_CRITICAL("Uninitialized vertex buffer");
		return;
	}
	if (_posCount != _normCount || _posCount != _colorCount) {
		KLEIN_LOG_WARNING("Vertex buffers are different in size");
	}
	auto  primCount = _posCount;
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
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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

void PBRMeshVColorGraphics::_renderUnlit(const Camera& camera, float aspectRatio,
	MeshRenderMode meshRenderMode, PrimitiveRenderMode primitiveRenderMode)
{
	if (_posCount == 0 || _normCount == 0 || _colorCount == 0) {
		KLEIN_LOG_CRITICAL("Uninitialized vertex buffer");
		return;
	}
	if (_posCount != _normCount || _posCount != _colorCount) {
		KLEIN_LOG_WARNING("Vertex buffers are different in size");
	}
	auto  primCount = _posCount;

	_shaderUnlit->bind();
	QMatrix4x4 projection;
	projection.perspective(camera.fov(), aspectRatio, camera.nearPlane(), camera.farPlane());
	auto transform = this->sceneNode()->transform();
	auto mvp = projection * camera.view() * transform;
	_shaderUnlit->setUniformValue("mvp", mvp);
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

void PBRMeshVColorGraphics::_renderPickVertex(const Camera& camera, float aspectRatio)
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

void PBRMeshVColorGraphics::_renderPickFace(const Camera& camera, float aspectRatio)
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
