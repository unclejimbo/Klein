#include "Core/Mesh.h"
#include "Core/Logger.h"
#include "Core/ResourceManager.h"
#include "Core/Util.h"

#include <Euclid/Geometry/Surface_mesh.h>
#include <Euclid/Geometry/Polyhedron_3.h>
#include <algorithm>
#include <cmath>
#include <cassert>

unsigned Mesh::_inc = 1;

Mesh::Mesh(const std::vector<QVector3D>& rawPoints,
	const std::vector<unsigned>& rawIndices,
	unsigned pointBuffer,
	unsigned normalBuffer)
	: _indices(rawIndices), _pointBufferID(pointBuffer),
	_normalBufferID(normalBuffer), _id(_inc++)
{
	_points.resize(rawPoints.size());
	std::transform(rawPoints.begin(), rawPoints.end(), _points.begin(),
		[](const QVector3D& v) {
			return qtToCgal<Point_3>(v);
		}
	);

	_isManifold = _buildSurfaceMesh();
}

Mesh::~Mesh() = default;

unsigned Mesh::id() const
{
	return _id;
}

bool Mesh::isManifold() const
{
	return _isManifold;
}

std::vector<Point_3>& Mesh::points()
{
	return _points;
}

const std::vector<Point_3>& Mesh::points() const
{
	return _points;
}

std::vector<unsigned>& Mesh::indices()
{
	return _indices;
}

const std::vector<unsigned>& Mesh::indices() const
{
	return _indices;
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

unsigned Mesh::pointBufferID() const
{
	return _pointBufferID;
}

unsigned Mesh::normalBufferID() const
{
	return _normalBufferID;
}

void Mesh::updateGLBuffer() const
{
	std::vector<QVector3D> positions(_indices.size());
	std::transform(_indices.begin(), _indices.end(), positions.begin(),
		[&vertices = _points](unsigned idx) {
		return cgalToQt(vertices[idx]);
	}
	);

	std::vector<QVector3D> normals;
	normals.reserve(_indices.size());
	for (auto i = 0; i < _indices.size(); i += 3) {
		auto n = CGAL::unit_normal(_points[_indices[i]],
			_points[_indices[i + 1]], _points[_indices[i + 2]]);
		auto normal = cgalToQt(n);
		normals.push_back(normal);
		normals.push_back(normal);
		normals.push_back(normal);
	}

	auto posBuf = ResourceManager::instance().glBuffer(_pointBufferID);
	posBuf->bind();
	posBuf->allocate(positions.data(), static_cast<int>(positions.size() * sizeof(QVector3D)));
	posBuf->release();
	auto normBuf = ResourceManager::instance().glBuffer(_normalBufferID);
	normBuf->bind();
	normBuf->allocate(normals.data(), static_cast<int>(normals.size() * sizeof(QVector3D)));
	normBuf->release();
}

bool Mesh::_buildSurfaceMesh()
{
	_surfaceMesh = std::make_unique<Surface_mesh>();
	return Euclid::build_surface_mesh(_points, _indices, *_surfaceMesh);
}

bool Mesh::_buildPolyhedron()
{

	_polyhedron = std::make_unique<Polyhedron_3>();
	Euclid::TriMeshBuilder<Polyhedron_3> meshBuilder(_points, _indices);
	try {
		_polyhedron->delegate(meshBuilder);
		CGAL::set_halfedgeds_items_id(*_polyhedron.get());
		return true;
	}
	catch (...) {
		KLEIN_LOG_WARNING("The provided mesh is non-manifold, thus cgal-mesh will be unavailable");
		_polyhedron = nullptr;
		return false;
	}
}
