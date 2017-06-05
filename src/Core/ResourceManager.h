#pragma once

#include "Core/Material.h"
#include "Core/Mesh.h"
#include "Core/PointCloud.h"
#include "Core/GLBuffer.h"

#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include <QVector3D>
#include <QVector4D>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

using MeshMap = std::unordered_map<unsigned, std::unique_ptr<Mesh>>;
using PointCloudMap = std::unordered_map<unsigned, std::unique_ptr<PointCloud>>;
using GLBufferMap = std::unordered_map<unsigned, std::unique_ptr<GLBuffer>>;
using PBRMaterialMap = std::unordered_map<std::string, PBRMaterial>;
using ShaderMap = std::unordered_map<std::string, std::unique_ptr<QOpenGLShaderProgram>>;

class ResourceManager
{
public:
	~ResourceManager();
	static ResourceManager& instance();

	void initialize(QOpenGLWidget* context);

	unsigned addMesh(const std::vector<QVector3D>& vertices,
		const std::vector<QVector3D>& normals, const std::vector<unsigned>& indices, 
		unsigned vertexBufferID, unsigned normalBufferID);
	Mesh* mesh(unsigned id);
	bool removeMesh(unsigned id);

	unsigned addPointCloud(const std::vector<QVector3D>& vertices,
		const std::vector<QVector3D>& normals, unsigned vertexBufferID);
	unsigned addPointCloud(const std::vector<QVector3D>& vertices, unsigned vertexBufferID);
	PointCloud* pointCloud(unsigned id);
	bool removePointCloud(unsigned id);

	unsigned addGLBuffer(const std::vector<float>& data, unsigned bufferSpec,
		QOpenGLBuffer::Type type = QOpenGLBuffer::Type::VertexBuffer,
		QOpenGLBuffer::UsagePattern usage = QOpenGLBuffer::UsagePattern::StaticDraw);
	unsigned addGLBuffer(const std::vector<QVector3D>& data, unsigned bufferSpec,
		QOpenGLBuffer::Type type = QOpenGLBuffer::Type::VertexBuffer,
		QOpenGLBuffer::UsagePattern usage = QOpenGLBuffer::UsagePattern::StaticDraw);
	unsigned addGLBuffer(const std::vector<QVector4D>& data, unsigned bufferSpec,
		QOpenGLBuffer::Type type = QOpenGLBuffer::Type::VertexBuffer,
		QOpenGLBuffer::UsagePattern usage = QOpenGLBuffer::UsagePattern::StaticDraw);
	unsigned addGLBuffer(const std::vector<typename Kernel::Point_3>& data, unsigned bufferSpec,
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
