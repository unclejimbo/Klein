#include "Core/Mesh.h"
#include "Core/Logger.h"
#include "Core/ResourceManager.h"
#include "Core/Util.h"

#include <Euclid/Geometry/Polyhedron_3.h>
#include <algorithm>
#include <cmath>

unsigned Mesh::_inc = 1;

Mesh::Mesh(const std::vector<QVector3D>& rawVertices,
	const std::vector<QVector3D>& rawFNormals,
	const std::vector<unsigned>& rawIndices,
	unsigned positionBuffer,
	unsigned normalBuffer)
	: indices(rawIndices), positionBufferID(positionBuffer),
	normalBufferID(normalBuffer), _id(_inc++)
{
	vertices.resize(rawVertices.size());
	std::transform(rawVertices.begin(), rawVertices.end(), vertices.begin(),
		[](const QVector3D& v) {
			return qtToEigen<float>(v);
		}
	);

	fNormals.resize(rawFNormals.size());
	std::transform(rawFNormals.begin(), rawFNormals.end(), fNormals.begin(),
		[](const QVector3D& v) {
			return qtToEigen<float>(v);
		}
	);

	cMesh = std::make_unique<CMesh>();
	Euclid::TriMeshBuilder<CMesh> meshBuilder(vertices, indices);
	try {
		cMesh->delegate(meshBuilder);

		size_t i = 0;
		for (auto v = cMesh->vertices_begin(); v != cMesh->vertices_end(); ++v) {
			v->id() = i++;
		}
		i = 0;
		for (auto f = cMesh->facets_begin(); f != cMesh->facets_end(); ++f) {
			f->id() = i++;
		}
	}
	catch (...) {
		KLEIN_LOG_WARNING("The provided mesh is non-manifold, thus cgal-mesh will be unavailable");
		cMesh = nullptr;
	}
}

Mesh::~Mesh() = default;

unsigned Mesh::id() const
{
	return _id;
}

void Mesh::updateGLBuffer() const
{
	std::vector<QVector3D> positions(indices.size());
	std::transform(indices.begin(), indices.end(), positions.begin(),
		[&vertices = vertices](unsigned idx) {
			return eigenToQt(vertices[idx]);
		}
	);

	std::vector<QVector3D> normals;
	normals.reserve(indices.size());
	for (const auto& fn : fNormals) {
		normals.push_back(eigenToQt(fn));
		normals.push_back(eigenToQt(fn));
		normals.push_back(eigenToQt(fn));
	}

	auto posBuf = ResourceManager::instance().glBuffer(positionBufferID);
	posBuf->bind();
	posBuf->allocate(positions.data(), static_cast<int>(positions.size() * sizeof(QVector3D)));
	posBuf->release();
	auto normBuf = ResourceManager::instance().glBuffer(normalBufferID);
	normBuf->bind();
	normBuf->allocate(normals.data(), static_cast<int>(normals.size() * sizeof(QVector3D)));
	normBuf->release();
}
