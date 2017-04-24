#pragma once

#ifdef _WIN32
#include <CodeAnalysis/Warnings.h>
#pragma warning(push)
#pragma warning(disable : ALL_CODE_ANALYSIS_WARNINGS)
#endif

#include <QVector3D>
#include <CGAL/Polyhedron_3.h>
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


// Convinient function for adding cgal mesh to glBuffers
void addGLBuffer(const std::string& name, const CMesh& cMesh);
