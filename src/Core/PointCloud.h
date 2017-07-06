#pragma once

#include "Core/GeomBase.h"

#include <CGAL/Point_set_3.h>
#include <vector>

using Point_set = CGAL::Point_set_3<Point_3>;

// Note:
// Point_set.collect_garbage() should be called everytime
// before indexing into the points in order to update the indices
class PointCloud
{
public:
	PointCloud(const std::vector<Point_3>& rawPoints,
		unsigned pointBuffer);
	PointCloud(const std::vector<Point_3>& rawPoints,
		const std::vector<Vector_3>& rawNormals,
		unsigned pointBuffer);
	~PointCloud();

	unsigned id() const;
	unsigned pointBufferID() const;
	Point_set& pointSet();
	const Point_set& pointSet() const;
	void updateGLBuffer() const;

private:
	unsigned _id;
	unsigned _pointBufferID;
	Point_set _pointSet;

	static unsigned _inc;
};
