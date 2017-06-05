#pragma once

#include <QVector3D>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Polyhedron_items_with_id_3.h>
#include <Eigen/Dense>
#include <memory>
#include <vector>
#include <string>

using Kernel = CGAL::Simple_cartesian<float>;
using CMesh = CGAL::Polyhedron_3<Kernel, CGAL::Polyhedron_items_with_id_3>;
class GraphicsComponent;

/*
	Warning: The user is ultimately responsible for keeping raw mesh data
	and cgal mesh data in synchronization when changed by the caller
*/
class Mesh
{
public:
	Mesh(const std::vector<QVector3D>& rawVertices,
		const std::vector<QVector3D>& rawFNormals,
		const std::vector<unsigned>& rawIndices,
		unsigned positionBuffer,
		unsigned normalBuffer);
	~Mesh();

	unsigned id() const;
	void updateGLBuffer() const;

	std::vector<Eigen::Vector3f> vertices;
	std::vector<Eigen::Vector3f> fNormals;
	std::vector<unsigned> indices;
	std::unique_ptr<CMesh> cMesh;

	unsigned positionBufferID;
	unsigned normalBufferID;

private:
	unsigned _id;
	static unsigned _inc;
};
