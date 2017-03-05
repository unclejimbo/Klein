#pragma once


#ifdef _WIN32
#include <CodeAnalysis/Warnings.h>
#pragma warning(push)
#pragma warning(disable : ALL_CODE_ANALYSIS_WARNINGS)
#endif

#include <QVector3D>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Polyhedron_incremental_builder_3.h>
#include <CGAL/Polyhedron_items_with_id_3.h>
#include <CGAL/Simple_cartesian.h>
#include <Eigen/Dense>
#include <algorithm>
#include <memory>
#include <vector>

#ifdef _WIN32
#pragma warning(pop)
#endif

using Kernel = CGAL::Simple_cartesian<float>;
using CMesh = CGAL::Polyhedron_3<Kernel, CGAL::Polyhedron_items_with_id_3>;

/*
	Warning: The user is ultimately responsible for keeping raw mesh data
	and cgal mesh data in sychronization when changed by the caller
*/
class Polyhedron_3
{
public:
	Polyhedron_3(const std::vector<QVector3D>& vertices, const std::vector<unsigned>& indices);
	~Polyhedron_3();

	std::vector<Eigen::Vector3f>& vertices();
	std::vector<unsigned>& indices();
	CMesh* cMesh();

private:
	std::vector<Eigen::Vector3f> _vertices;
	std::vector<unsigned> _indices;
	std::unique_ptr<CMesh> _cMesh;
};



template<typename HDS>
class MeshBuilder : public CGAL::Modifier_base<HDS>
{
public:
	MeshBuilder(const std::vector<QVector3D>& vertices, const std::vector<unsigned>& indices);
	void operator()(HDS& hds);

private:
	const std::vector<QVector3D>& _vertices;
	const std::vector<unsigned>& _indices;
};

template<typename HDS>
inline MeshBuilder<HDS>::MeshBuilder(const std::vector<QVector3D>& vertices, const std::vector<unsigned>& indices)
	: _vertices(vertices), _indices(indices)
{
}

template<typename HDS>
inline void MeshBuilder<HDS>::operator()(HDS& hds)
{
	CGAL::Polyhedron_incremental_builder_3<HDS> builder(hds, true);
	builder.begin_surface(_vertices.size(), _indices.size() / 3);

	for (const auto& v : _vertices) {
		builder.add_vertex(HDS::Vertex::Point(v.x(), v.y(), v.z()));
	}

	for (size_t i = 0; i < _indices.size(); ++i) {
		builder.begin_facet();
		builder.add_vertex_to_facet(_indices[i++]);
		builder.add_vertex_to_facet(_indices[i++]);
		builder.add_vertex_to_facet(_indices[i]);
		builder.end_facet();
	}

	builder.end_surface();
}



// Convinient function for adding cgal mesh to glBuffers
void addGLBuffer(const std::string& name, const CMesh& cMesh);
