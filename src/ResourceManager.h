#pragma once

#include "Material.h"
#include "Mesh.h"

#ifdef _WIN32
#include <CodeAnalysis/Warnings.h>
#pragma warning(push)
#pragma warning(disable : ALL_CODE_ANALYSIS_WARNINGS)
#endif

#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include <QVector3D>
#include <QVector4D>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

#ifdef _WIN32
#pragma warning(pop)
#endif

using MeshMap = std::unordered_map<std::string, std::pair<std::unique_ptr<Polyhedron_3>, std::string>>; // <Polyhedron_3, GLBuffer>
using GLBufferMap = std::unordered_map<std::string, QOpenGLBuffer>;
using MaterialMap = std::unordered_map<std::string, Material>;
using ShaderMap = std::unordered_map<std::string, std::unique_ptr<QOpenGLShaderProgram>>;

class ResourceManager
{
public:
	~ResourceManager();
	static ResourceManager& instance();

	void initialize(QOpenGLWidget* context);

	void addMesh(const std::string& name, const std::vector<QVector3D>& vertices,
		const std::vector<unsigned>& indices, const std::string& bufferName);
	std::pair<Polyhedron_3*, std::string> mesh(const std::string& name);
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

	void addMaterial(const std::string& name, const QVector3D& albedo,
		const float roughness, const float metallic, float ao);
	Material* material(const std::string& name);
	bool removeMaterial(const std::string& name);

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
	MaterialMap _materialMap;
	ShaderMap _shaderMap;
};
