#include "Core/RenderMeshNode.h"
#include "Core/ResourceManager.h"
#include "Core/Scene.h"

#ifdef _WIN32
#include <CodeAnalysis/Warnings.h>
#pragma warning(push)
#pragma warning(disable : ALL_CODE_ANALYSIS_WARNINGS)
#endif

#include "QOpenGLWidget"
#include <QOpenGLVertexArrayObject>
#include <QString>

#ifdef _WIN32
#pragma warning(pop)
#endif

RenderMeshNode::RenderMeshNode(QOpenGLWidget* context, Scene* scene, SceneNode* parent, const QMatrix4x4& transform)
	: SceneNode(scene, parent, transform)
{
	context->makeCurrent();
	this->initializeOpenGLFunctions();
	context->doneCurrent();
}

RenderMeshNode::~RenderMeshNode() = default;

void RenderMeshNode::attachMesh(const std::string& vertexBuffer,
	const std::string& normalBuffer, const std::string& material)
{
	_vertexBuffer = vertexBuffer;
	_normalBuffer = normalBuffer;
	_material = material;
}

void RenderMeshNode::setMaterial(const std::string& material)
{
	_material = material;
}

void RenderMeshNode::render(QOpenGLShaderProgram& shader, const Camera& camera,
	const std::array<Light, KLEIN_MAX_LIGHTS>& lights, ShadingMethod method)
{
	QMatrix4x4 projection;
	projection.perspective(camera.fov(), camera.aspect(), 0.001f, 1000.0f);
	auto mvp = projection * camera.view() * this->transform();
	auto invTransModel = this->transform().inverted().transposed();
	shader.setUniformValue("model", this->transform());
	shader.setUniformValue("mvp", mvp);
	shader.setUniformValue("invTransModel", invTransModel);
	shader.setUniformValue("viewPosition_w", camera.position());
	for (auto i = 0; i < KLEIN_MAX_LIGHTS; ++i) {
		auto lightPosition = QString("lightPosition_w[%1]").arg(i);
		auto lightColor = QString("lightColor[%1]").arg(i);
		shader.setUniformValue(lightPosition.toLatin1().data(), lights[i].position_w);
		shader.setUniformValue(lightColor.toLatin1().data(), lights[i].color);
	}
	shader.setUniformValue("alpha", 1.0f);

	if (!_vertexBuffer.empty()) {
		auto vertexBuffer = ResourceManager::instance().glBuffer(_vertexBuffer);
		auto normalBuffer = ResourceManager::instance().glBuffer(_normalBuffer);
		auto material = ResourceManager::instance().material(_material);

		QOpenGLVertexArrayObject vao;
		vao.create();
		vao.bind();
		vertexBuffer->bind();
		shader.setAttributeBuffer(0, GL_FLOAT, 0, 3);
		shader.enableAttributeArray(0);
		auto bufferSize = vertexBuffer->size();
		vertexBuffer->release();
		normalBuffer->bind();
		shader.setAttributeBuffer(1, GL_FLOAT, 0, 3);
		shader.enableAttributeArray(1);
		normalBuffer->release();

		if (method == ShadingMethod::shaded || method == ShadingMethod::hiddenLine) {
			shader.setUniformValue("material.albedo", material->albedo);
			shader.setUniformValue("material.metallic", material->metallic);
			shader.setUniformValue("material.roughness", material->roughness);
			shader.setUniformValue("material.ao", material->ao);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glDrawArrays(GL_TRIANGLES, 0, bufferSize / sizeof(QVector3D));
		}
		if (method == ShadingMethod::wireframe || method == ShadingMethod::hiddenLine) {
			shader.setUniformValue("material.albedo", QVector3D(1.0f, 1.0f, 1.0f));
			shader.setUniformValue("material.metallic", 0.0f);
			shader.setUniformValue("material.roughness", 1.0f);
			shader.setUniformValue("material.ao", 1.0f);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glEnable(GL_POLYGON_OFFSET_LINE);
			glPolygonOffset(-1.0f, 1.0f);
			glDrawArrays(GL_TRIANGLES, 0, bufferSize / sizeof(QVector3D));
			glDisable(GL_POLYGON_OFFSET_LINE);
		}
		vao.release();
	}
}
