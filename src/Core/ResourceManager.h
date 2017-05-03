#pragma once

#include "Core/Material.h"
#include "Core/Mesh.h"

#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include <QVector3D>
#include <QVector4D>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

using MeshMap = std::unordered_map<std::string, std::pair<std::unique_ptr<Mesh>, std::string>>; // <Mesh, GLBuffer>
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
		const std::vector<unsigned>& indices, const std::string& bufferName);
	std::pair<Mesh*, std::string> mesh(const std::string& name);
	bool removeMesh(const std::string& name);

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
	GLBufferMap _bufferMap;
	PBRMaterialMap _pbrMaterialMap;
	ShaderMap _shaderMap;
};
