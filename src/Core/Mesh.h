#pragma once

#include <QVector3D>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Surface_mesh.h>
#include <Eigen/Dense>
#include <memory>
#include <vector>
#include <string>

using Kernel = CGAL::Simple_cartesian<float>;
using Point_3 = Kernel::Point_3;
using Polyhedron_3 = CGAL::Polyhedron_3<Kernel>;
using Surface_mesh = CGAL::Surface_mesh<Point_3>;
class GraphicsComponent;

/*
	Warning: The user is ultimately responsible for keeping raw mesh data
	and cgal mesh data in synchronization when changed by the caller.
	TODO: Let Mesh handle this automatically.
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
	const std::vector<Eigen::Vector3f>& vertices() const;
	std::vector<Eigen::Vector3f>& vertice();
	void setVertices(const std::vector<Eigen::Vector3f>& vertices);
	const std::vector<unsigned>& indices() const;
	std::vector<unsigned>& indices();
	void setIndices(const std::vector<unsigned>& indices);
	Surface_mesh* surfaceMesh();
	void setSurfaceMesh(const Surface_mesh& surfaceMeseh);
	Polyhedron_3* polyhedron();
	void setPolyhedron(const Polyhedron_3& polyhedron);
	unsigned positionBufferID() const;
	unsigned normalBufferID() const;
	void updateGLBuffer() const;
	bool isManifold() const;

private:
	bool _buildSurfaceMesh();
	bool _buildPolyhedron();

private:
	unsigned _id;
	std::vector<Eigen::Vector3f> _vertices;
	std::vector<Eigen::Vector3f> _fNormals;
	std::vector<unsigned> _indices;
	std::unique_ptr<Polyhedron_3> _polyhedron;
	std::unique_ptr<Surface_mesh> _surfaceMesh;
	unsigned _positionBufferID;
	unsigned _normalBufferID;
	bool _isManifold;

	static unsigned _inc;
};
