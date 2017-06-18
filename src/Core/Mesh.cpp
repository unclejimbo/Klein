#include "Core/Mesh.h"
#include "Core/Logger.h"
#include "Core/ResourceManager.h"
#include "Core/Util.h"

#include <Euclid/Geometry/Polyhedron_3.h>
#include <algorithm>
#include <cmath>
#include <cassert>

unsigned Mesh::_inc = 1;

Mesh::Mesh(const std::vector<QVector3D>& rawVertices,
	const std::vector<QVector3D>& rawFNormals,
	const std::vector<unsigned>& rawIndices,
	unsigned positionBuffer,
	unsigned normalBuffer)
	: _indices(rawIndices), _positionBufferID(positionBuffer),
	_normalBufferID(normalBuffer), _id(_inc++)
{
	_vertices.resize(rawVertices.size());
	std::transform(rawVertices.begin(), rawVertices.end(), _vertices.begin(),
		[](const QVector3D& v) {
			return qtToEigen<float>(v);
		}
	);

	_fNormals.resize(rawFNormals.size());
	std::transform(rawFNormals.begin(), rawFNormals.end(), _fNormals.begin(),
		[](const QVector3D& v) {
			return qtToEigen<float>(v);
		}
	);

	_isManifold = _buildSurfaceMesh();
}

Mesh::~Mesh() = default;

unsigned Mesh::id() const
{
	return _id;
}

const std::vector<Eigen::Vector3f>& Mesh::vertices() const
{
	return _vertices;
}

std::vector<Eigen::Vector3f>& Mesh::vertice()
{
	return _vertices;
}

void Mesh::setVertices(const std::vector<Eigen::Vector3f>& vertices)
{
	_vertices = vertices;
}

const std::vector<unsigned>& Mesh::indices() const
{
	return _indices;
}

std::vector<unsigned>& Mesh::indices()
{
	return _indices;
}

void Mesh::setIndices(const std::vector<unsigned>& indices)
{
	_indices = indices;
}

Surface_mesh* Mesh::surfaceMesh()
{
	if (!_isManifold) {
		return nullptr;
	}
	else {
		return _surfaceMesh.get();
	}
}

void Mesh::setSurfaceMesh(const Surface_mesh& surfaceMesh)
{
	_surfaceMesh = std::make_unique<Surface_mesh>(surfaceMesh);
}

Polyhedron_3* Mesh::polyhedron()
{
	if (!_isManifold) {
		return nullptr;
	}
	else {
		if (_polyhedron == nullptr) {
			auto result = _buildPolyhedron();
			assert(result == _isManifold);
		}
		return _polyhedron.get();
	}
}

void Mesh::setPolyhedron(const Polyhedron_3& polyhedron)
{
	_polyhedron = std::make_unique<Polyhedron_3>(polyhedron);
}

unsigned Mesh::positionBufferID() const
{
	return _positionBufferID;
}

unsigned Mesh::normalBufferID() const
{
	return _normalBufferID;
}

void Mesh::updateGLBuffer() const
{
	std::vector<QVector3D> positions(_indices.size());
	std::transform(_indices.begin(), _indices.end(), positions.begin(),
		[&vertices = _vertices](unsigned idx) {
		return eigenToQt(vertices[idx]);
	}
	);

	std::vector<QVector3D> normals;
	normals.reserve(_indices.size());
	for (const auto& fn : _fNormals) {
		normals.push_back(eigenToQt(fn));
		normals.push_back(eigenToQt(fn));
		normals.push_back(eigenToQt(fn));
	}

	auto posBuf = ResourceManager::instance().glBuffer(_positionBufferID);
	posBuf->bind();
	posBuf->allocate(positions.data(), static_cast<int>(positions.size() * sizeof(QVector3D)));
	posBuf->release();
	auto normBuf = ResourceManager::instance().glBuffer(_normalBufferID);
	normBuf->bind();
	normBuf->allocate(normals.data(), static_cast<int>(normals.size() * sizeof(QVector3D)));
	normBuf->release();
}

bool Mesh::isManifold() const
{
	return _isManifold;
}

bool Mesh::_buildSurfaceMesh()
{
	_surfaceMesh = std::make_unique<Surface_mesh>();
	for (auto i = 0; i < _indices.size(); i += 3) {
		auto v1 = _surfaceMesh->add_vertex(
			eigenToCgal<Kernel>(_vertices[_indices[i + 0]]));
		auto v2 = _surfaceMesh->add_vertex(
			eigenToCgal<Kernel>(_vertices[_indices[i + 1]]));
		auto v3 = _surfaceMesh->add_vertex(
			eigenToCgal<Kernel>(_vertices[_indices[i + 2]]));
		_surfaceMesh->add_face(v1, v2, v3);
	}
	if (_surfaceMesh->is_valid()) {
		return true;
	}
	else {
		return false;
	}
}

bool Mesh::_buildPolyhedron()
{

	_polyhedron = std::make_unique<Polyhedron_3>();
	Euclid::TriMeshBuilder<Polyhedron_3> meshBuilder(_vertices, _indices);
	try {
		_polyhedron->delegate(meshBuilder);
		return true;
	}
	catch (...) {
		KLEIN_LOG_WARNING("The provided mesh is non-manifold, thus cgal-mesh will be unavailable");
		_polyhedron = nullptr;
		return false;
	}
}
