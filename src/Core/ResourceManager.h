#pragma once

#include "Core/Material.h"
#include "Core/Mesh.h"
#include "Core/PointCloud.h"
#include "Core/GLBuffer.h"
#include "Core/Logger.h"

#include <Euclid/Geometry/MeshProperties.h>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include <QVector3D>
#include <QVector4D>
#include <memory>
#include <unordered_map>
#include <vector>
#include <utility>

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

	unsigned addMesh(const std::vector<double>& points,
		const std::vector<unsigned>& indices,
		unsigned pointBufferID, unsigned normalBufferID);
	unsigned addMesh(const std::vector<Point_3>& points,
		const std::vector<unsigned>& indices,
		unsigned pointBufferID, unsigned normalBufferID);
	Mesh* mesh(unsigned id);
	bool removeMesh(unsigned id);

	unsigned addPointCloud(const std::vector<double>& points, unsigned pointBufferID);
	unsigned addPointCloud(const std::vector<double>& points,
		const std::vector<double>& normals, unsigned pointBufferID);
	unsigned addPointCloud(const std::vector<Point_3>& points, unsigned pointBufferID);
	unsigned addPointCloud(const std::vector<Point_3>& points,
		const std::vector<Vector_3>& normals, unsigned pointBufferID);
	PointCloud* pointCloud(unsigned id);
	bool removePointCloud(unsigned id);

	unsigned addGLBuffer(const std::vector<float>& data, unsigned bufferSpec,
		QOpenGLBuffer::Type type = QOpenGLBuffer::Type::VertexBuffer,
		QOpenGLBuffer::UsagePattern usage = QOpenGLBuffer::UsagePattern::StaticDraw);
	unsigned addGLBuffer(const std::vector<QVector3D>& data, unsigned bufferSpec,
		QOpenGLBuffer::Type type = QOpenGLBuffer::Type::VertexBuffer,
		QOpenGLBuffer::UsagePattern usage = QOpenGLBuffer::UsagePattern::StaticDraw);
	template<typename Mesh>
	std::pair<unsigned, unsigned> addGLBuffer(const Mesh& mesh,
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

template<typename Mesh>
inline std::pair<unsigned, unsigned>
ResourceManager::addGLBuffer(const Mesh& mesh, QOpenGLBuffer::UsagePattern usage)
{
	if (_context != nullptr) {
		auto vpmap = get(boost::vertex_point, mesh);
		std::vector<float> points;
		std::vector<float> normals;
		points.reserve(num_faces(mesh) * 3);
		normals.reserve(num_faces(mesh) * 3);
		for (const auto& f : faces(mesh)) {
			for (const auto& v : points_around_face(halfedge(f, mesh), mesh)) {
				points.push_back(vpmap[v].x());
				points.push_back(vpmap[v].y());
				points.push_back(vpmap[v].z());
			}
			auto n = Euclid::face_normal(f, mesh);
			normals.push_back(n.x());
			normals.push_back(n.y());
			normals.push_back(n.z());
			normals.push_back(n.x());
			normals.push_back(n.y());
			normals.push_back(n.z());
			normals.push_back(n.x());
			normals.push_back(n.y());
			normals.push_back(n.z());
		}

		_context->makeCurrent();
		auto posBuf = std::make_unique<GLBuffer>(GL_TRIANGLES, QOpenGLBuffer::VertexBuffer);
		posBuf->create();
		posBuf->setUsagePattern(usage);
		posBuf->bind();
		posBuf->allocate(points.data(), static_cast<int>(points.size() * sizeof(float)));
		posBuf->release();
		auto normBuf = std::make_unique<GLBuffer>(GL_TRIANGLES, QOpenGLBuffer::VertexBuffer);
		normBuf->create();
		normBuf->setUsagePattern(usage);
		normBuf->bind();
		normBuf->allocate(normals.data(), static_cast<int>(normals.size() * sizeof(float)));
		normBuf->release();
		_context->doneCurrent();

		auto posID = posBuf->bufferId();
		if (!_bufferMap.insert_or_assign(posID, std::move(posBuf)).second) {
			KLEIN_LOG_WARNING(QString("OpenGL Buffer Object %1 already exists and is replaced").arg(posID));
		}
		auto normID = normBuf->bufferId();
		if (!_bufferMap.insert_or_assign(normID, std::move(normBuf)).second) {
			KLEIN_LOG_WARNING(QString("OpenGL Buffer Object %1 already exists and is replaced").arg(normID));
		}
		return { posID, normID };
	}
	else {
		KLEIN_LOG_CRITICAL("Invalid OpenGL context");
		return { 0, 0 };
	}
}
