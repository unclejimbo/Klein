#pragma once

#include "Core/Material.h"
#include "Core/Mesh.h"
#include "Core/PointCloud.h"

#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include <QVector3D>
#include <QVector4D>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

using MeshMap = std::unordered_map<std::string, std::unique_ptr<Mesh>>;
using PointCloudMap = std::unordered_map<std::string, std::unique_ptr<PointCloud>>;
using GLBufferMap = std::unordered_map<std::string, QOpenGLBuffer>;
using PBRMaterialMap = std::unordered_map<std::string, PBRMaterial>;
using ShaderMap = std::unordered_map<std::string, std::unique_ptr<QOpenGLShaderProgram>>;

class ResourceManager
{
public:
	~ResourceManager();
	static ResourceManager& instance();

	void initialize(QOpenGLWidget* context);

	void addMesh(const std::string& name, const std::vector<QVector3D>& vertices,
		const std::vector<QVector3D>& normals, const std::vector<unsigned>& indices, 
		const std::string& vertexBufferID, const std::string& normalBufferID);
	void addMesh(const std::string& name, const std::vector<QVector3D>& vertices,
		const std::vector<unsigned>& indices, 
		const std::string& vertexBufferID, const std::string& normalBufferID);
	Mesh* mesh(const std::string& name);
	Mesh* mesh(unsigned id);
	bool removeMesh(const std::string& name);

	void addPointCloud(const std::string& name, const std::vector<QVector3D>& vertices,
		const std::vector<QVector3D>& normals, const std::string& vertexBufferID);
	void addPointCloud(const std::string& name, const std::vector<QVector3D>& vertices,
		const std::string& vertexBufferID);
	PointCloud* pointCloud(const std::string& name);
	bool removePointCloud(const std::string& name);

	void addGLBuffer(const std::string& name, const std::vector<float>& data,
		QOpenGLBuffer::Type type = QOpenGLBuffer::Type::VertexBuffer,
		QOpenGLBuffer::UsagePattern usage = QOpenGLBuffer::UsagePattern::StaticDraw);
	void addGLBuffer(const std::string& name, const std::vector<QVector3D>& data,
		QOpenGLBuffer::Type type = QOpenGLBuffer::Type::VertexBuffer,
		QOpenGLBuffer::UsagePattern usage = QOpenGLBuffer::UsagePattern::StaticDraw);
	void addGLBuffer(const std::string& name, const std::vector<QVector4D>& data,
		QOpenGLBuffer::Type type = QOpenGLBuffer::Type::VertexBuffer,
		QOpenGLBuffer::UsagePattern usage = QOpenGLBuffer::UsagePattern::StaticDraw);
	void addGLBuffer(const std::string& name, const std::vector<typename Kernel::Point_3>& data,
		QOpenGLBuffer::Type type = QOpenGLBuffer::Type::VertexBuffer,
		QOpenGLBuffer::UsagePattern usage = QOpenGLBuffer::UsagePattern::StaticDraw);
	QOpenGLBuffer* glBuffer(const std::string& name);
	bool removeGLBuffer(const std::string& name);

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
