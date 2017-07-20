#pragma once

#include "Core/GeomBase.h"

#include <QVector3D>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Polyhedron_items_with_id_3.h>
#include <memory>
#include <vector>


using Surface_mesh = CGAL::Surface_mesh<Point_3>;
using Polyhedron_3 = CGAL::Polyhedron_3<Kernel, CGAL::Polyhedron_items_with_id_3>;

/*
	Warning: The user is ultimately responsible for keeping raw mesh data
	and cgal mesh data in synchronization when changed by the caller.
	TODO: Let Mesh handle this automatically.
	Keep in mind that the indices as well as the id property within cgal data structure
	should all be updated according to the new vertices.
*/
class Mesh
{
public:
	Mesh(const std::vector<Point_3>& rawPoints,
		const std::vector<unsigned>& rawIndices,
		unsigned PointBuffer,
		unsigned normalBuffer);
	Mesh(const Mesh& rhs) = delete; // TODO, delete for now to prevent using
	Mesh& operator=(const Mesh& rhs) = delete;
	Mesh(Mesh&& rhs) = delete;
	Mesh& operator=(Mesh&& rhs) = delete;
	~Mesh();

	unsigned id() const;
	bool isManifold() const;
	Point_3 point(unsigned idx);
	Point_3 pointAt(unsigned idx);
	unsigned pointIndex(unsigned idx);
	std::vector<Point_3>& points();
	const std::vector<Point_3>& points() const;
	std::vector<unsigned>& indices();
	const std::vector<unsigned>& indices() const;
	Surface_mesh* surfaceMesh();
	Polyhedron_3* polyhedron();
	unsigned pointBufferID() const;
	unsigned normalBufferID() const;
	void updateGLBuffer() const;

private:
	bool _buildSurfaceMesh();
	bool _buildPolyhedron();

private:
	unsigned _id;
	bool _isManifold;
	std::vector<Point_3> _points;
	std::vector<unsigned> _indices;
	std::unique_ptr<Polyhedron_3> _polyhedron;
	std::unique_ptr<Surface_mesh> _surfaceMesh;
	unsigned _pointBufferID;
	unsigned _normalBufferID;

	static unsigned _inc;
};
