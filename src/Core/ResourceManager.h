#pragma once

#include "Core/Material.h"
#include "Core/Mesh.h"
#include "Core/PointCloud.h"
#include "Core/GLBuffer.h"
#include "Core/Logger.h"

#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include <QVector3D>
#include <QVector4D>
#include <memory>
#include <unordered_map>
#include <vector>

using MeshMap = std::unordered_map<unsigned, std::unique_ptr<Mesh>>;
using PointCloudMap = std::unordered_map<unsigned, std::unique_ptr<PointCloud>>;
using GLBufferMap = std::unordered_map<unsigned, std::unique_ptr<GLBuffer>>;
using PBRMaterialMap = std::unordered_map<std::string, PBRMaterial>;
using ShaderMap = std::unordered_map<std::string, std::unique_ptr<QOpenGLShaderProgram>>;

class ResourceManager
{
public:
	static ResourceManager& instance();

	void initialize(QOpenGLWidget* context);

	unsigned addMesh(const std::vector<Point_3>& vertices,
		const std::vector<unsigned>& indices,
		unsigned vertexBufferID, unsigned normalBufferID);
	Mesh* mesh(unsigned id);
	bool removeMesh(unsigned id);

	unsigned addPointCloud(const std::vector<Point_3>& vertices, unsigned vertexBufferID);
	unsigned addPointCloud(const std::vector<Point_3>& vertices,
		const std::vector<Vector_3>& normals, unsigned vertexBufferID);
	PointCloud* pointCloud(unsigned id);
	bool removePointCloud(unsigned id);

	template<typename T>
	unsigned addGLBuffer(const std::vector<T>& data, unsigned bufferSpec,
		QOpenGLBuffer::Type type = QOpenGLBuffer::Type::VertexBuffer,
		QOpenGLBuffer::UsagePattern usage = QOpenGLBuffer::UsagePattern::StaticDraw);
	GLBuffer* glBuffer(unsigned bufferID);
	bool removeGLBuffer(unsigned bufferID);

	void addPBRMaterial(const std::string& name, const QVector3D& albedo,
		const float roughness, const float metallic, float ao);
	PBRMaterial* pbrMaterial(const std::string& name);
	bool removePBRMaterial(const std::string& name);

	void addShaderProgram(const std::string& name,
		const std::string& vertexShader, const std::string& fragmentShader);
	QOpenGLShaderProgram* shaderProgram(const std::string& name);
	bool removeShaderProgram(const std::string& name);

private:
	ResourceManager() {};

private:
	QOpenGLWidget* _context = nullptr;
	MeshMap _meshMap;
	PointCloudMap _pointCloudMap;
	GLBufferMap _bufferMap;
	PBRMaterialMap _pbrMaterialMap;
	ShaderMap _shaderMap;
};

template<typename T>
inline unsigned ResourceManager::addGLBuffer(const std::vector<T>& data, unsigned bufferSpec,
	QOpenGLBuffer::Type type, QOpenGLBuffer::UsagePattern usage)
{
	if (_context != nullptr) {
		_context->makeCurrent();
		auto buffer = std::make_unique<GLBuffer>(bufferSpec, type);
		buffer->create();
		buffer->setUsagePattern(usage);
		buffer->bind();
		buffer->allocate(data.data(), static_cast<int>(data.size() * sizeof(T)));
		buffer->release();
		_context->doneCurrent();

		auto id = buffer->bufferId();
		if (!_bufferMap.insert_or_assign(id, std::move(buffer)).second) {
			KLEIN_LOG_WARNING(QString("OpenGL Buffer Object %1 already exists and is replaced").arg(id));
		}
		return id;
	}
	else {
		KLEIN_LOG_CRITICAL("Invalid OpenGL context");
		return 0;
	}
}
