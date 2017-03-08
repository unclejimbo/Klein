#include "Mesh.h"

#include "Logger.h"
#include "ResourceManager.h"
#include "Util.h"
#include <euclid/Polyhedron_3.h>

#ifdef _WIN32
#include <CodeAnalysis/Warnings.h>
#pragma warning(push)
#pragma warning(disable : ALL_CODE_ANALYSIS_WARNINGS)
#endif

#include <algorithm>
#include <cmath>

#ifdef _WIN32
#pragma warning(pop)
#endif

Polyhedron_3::Polyhedron_3(const std::vector<QVector3D>& vertices, const std::vector<unsigned>& indices)
	: _indices(indices)
{
	_vertices.resize(vertices.size());
	std::transform(vertices.begin(), vertices.end(), _vertices.begin(),
		[](const QVector3D& v) {
			return qtToEigen<float>(v);
		}
	);

	_cMesh = std::make_unique<CMesh>();
	euclid::TriMeshBuilder<CMesh> meshBuilder(_vertices, indices);
	try {
		_cMesh->delegate(meshBuilder);

		size_t i = 0;
		for (auto v = _cMesh->vertices_begin(); v != _cMesh->vertices_end(); ++v) {
			v->id() = i++;
		}
		i = 0;
		for (auto f = _cMesh->facets_begin(); f != _cMesh->facets_end(); ++f) {
			f->id() = i++;
		}
	}
	catch (...) {
		KLEIN_LOG_WARNING("The provided mesh is non-manifold, thus cgal-mesh will be unavailable");
		_cMesh = nullptr;
	}
}

Polyhedron_3::~Polyhedron_3() = default;

std::vector<Eigen::Vector3f>& Polyhedron_3::vertices()
{
	return _vertices;
}

std::vector<unsigned>& Polyhedron_3::indices()
{
	return _indices;
}

CMesh* Polyhedron_3::cMesh()
{
	return _cMesh.get();
}

void addGLBuffer(const std::string& name, const CMesh& cMesh)
{
	std::vector<typename Kernel::Point_3> positions;
	positions.reserve((cMesh.size_of_facets() * 3));
	for (auto f = cMesh.facets_begin(); f != cMesh.facets_end(); ++f) {
		auto v = f->facet_begin();
		do {
			positions.push_back(v->vertex()->point());
		} while (++v != f->facet_begin());
	}
	auto vertexBuffer = name + "_VertexBuffer";
	ResourceManager::instance().addGLBuffer(vertexBuffer, positions);

	std::vector<typename Kernel::Point_3> normals(cMesh.size_of_facets() * 3);
	std::transform(cMesh.facets_begin(), cMesh.facets_end(), normals.begin(),
		[](decltype(cMesh.facets_begin())::value_type f) {
			auto h = f.halfedge();
			auto n = CGAL::normal(h->vertex()->point(),
				h->next()->vertex()->point(),
				h->next()->next()->vertex()->point());
			auto invLen = 1.0 / std::sqrt(n.squared_length());
			return typename Kernel::Point_3(n.x() * invLen, n.y() * invLen, n.z() * invLen);
		}
	);
	auto normalBuffer = name + "_NormalBuffer";
	ResourceManager::instance().addGLBuffer(normalBuffer, normals);
}
