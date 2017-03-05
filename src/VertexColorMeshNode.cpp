#include "VertexColorMeshNode.h"

#include "Logger.h"
#include "ResourceManager.h"

#ifdef _WIN32
#include <CodeAnalysis/Warnings.h>
#pragma warning(push)
#pragma warning(disable : ALL_CODE_ANALYSIS_WARNINGS)
#endif

#include <QOpenGLWidget>
#include <QOpenGLVertexArrayObject>
#include <vector>

#ifdef _WIN32
#pragma warning(pop)
#endif

VertexColorMeshNode::VertexColorMeshNode(QOpenGLWidget* context, Scene* scene, SceneNode* parent, const QMatrix4x4& transform)
	: SceneNode(scene, parent, transform), _context(context)
{
	_context->makeCurrent();
	this->initializeOpenGLFunctions();
	_context->doneCurrent();
}

VertexColorMeshNode::~VertexColorMeshNode()
{
	auto name = _vertexBuffer + "White";
	ResourceManager::instance().removeGLBuffer(name);
}

void VertexColorMeshNode::attachMesh(const std::string& vertexBuffer, const std::string& colorBuffer)
{
	_vertexBuffer = vertexBuffer;
	_colorBuffer = colorBuffer;

	// Cache color buffer for drawing wireframe
	_context->makeCurrent();
	auto vBuffer = ResourceManager::instance().glBuffer(vertexBuffer);
	vBuffer->bind();
	std::vector<float> white(vBuffer->size() * 4 / 3, 1.0f);
	vBuffer->release();
	_context->makeCurrent();
	auto name = vertexBuffer + "White";
	ResourceManager::instance().addGLBuffer(name, white);
}

void VertexColorMeshNode::setColor(const std::string& colorBuffer)
{
	_colorBuffer = colorBuffer;
}

void VertexColorMeshNode::render(QOpenGLShaderProgram& shader, const Camera & camera, ShadingMethod method)
{
	QMatrix4x4 projection;
	projection.perspective(camera.fov(), camera.aspect(), 0.001f, 1000.0f);
	auto mvp = projection * camera.view() * this->transform();
	shader.setUniformValue("mvp", mvp);

	if (!_vertexBuffer.empty()) {
		auto vertexBuffer = ResourceManager::instance().glBuffer(_vertexBuffer);

		QOpenGLVertexArrayObject vao;
		vao.create();
		vao.bind();
		vertexBuffer->bind();
		shader.setAttributeBuffer(0, GL_FLOAT, 0, 3);
		shader.enableAttributeArray(0);
		auto bufferSize = vertexBuffer->size();
		vertexBuffer->release();

		if (method == ShadingMethod::shaded || method == ShadingMethod::hiddenLine) {
			auto colorBuffer = ResourceManager::instance().glBuffer(_colorBuffer);
			colorBuffer->bind();
			shader.setAttributeBuffer(1, GL_FLOAT, 0, 4);
			shader.enableAttributeArray(1);
			colorBuffer->release();
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glDrawArrays(GL_TRIANGLES, 0, bufferSize / sizeof(QVector3D));
			glDisable(GL_CULL_FACE);
			glDisable(GL_BLEND);
		}
		if (method == ShadingMethod::wireframe || method == ShadingMethod::hiddenLine) {
			auto name = _vertexBuffer + "White";
			auto colorBuffer = ResourceManager::instance().glBuffer(name);
			colorBuffer->bind();
			shader.setAttributeBuffer(1, GL_FLOAT, 0, 4);
			shader.enableAttributeArray(1);
			colorBuffer->release();
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glEnable(GL_POLYGON_OFFSET_LINE);
			glPolygonOffset(-1.0f, 1.0f);
			glDrawArrays(GL_TRIANGLES, 0, bufferSize / sizeof(QVector3D));
			glDisable(GL_POLYGON_OFFSET_LINE);
		}
		vao.release();
	}
}
