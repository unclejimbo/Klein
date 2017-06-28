#pragma once

#include "Core/GeomBase.h"

#include <QVector3D>
#include <CGAL/Point_set_3.h>
#include <vector>
#include <string>

using Point_set = CGAL::Point_set_3<Point_3>;

class PointCloud
{
public:
	PointCloud(const std::vector<QVector3D>& rawPoints,
		unsigned pointBuffer);
	PointCloud(const std::vector<QVector3D>& rawPoints,
		const std::vector<QVector3D>& rawNormals,
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
