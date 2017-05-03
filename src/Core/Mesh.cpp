#include "Core/Mesh.h"
#include "Core/Logger.h"
#include "Core/ResourceManager.h"
#include "Core/Util.h"
#include <Euclid/Geometry/Polyhedron_3.h>

#include <algorithm>
#include <cmath>

Mesh::Mesh(const std::vector<QVector3D>& rawVertices, const std::vector<unsigned>& rawIndices)
	: indices(rawIndices)
{
	vertices.resize(rawVertices.size());
	std::transform(rawVertices.begin(), rawVertices.end(), vertices.begin(),
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
