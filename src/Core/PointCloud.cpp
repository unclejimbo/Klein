#include "Core/PointCloud.h"
#include "Core/ResourceManager.h"
#include "Core/Util.h"
#include "Core/Logger.h"

unsigned PointCloud::_inc = 1;

PointCloud::PointCloud(const std::vector<Point_3>& rawPoints,
	unsigned pointBuffer)
{
	_pointSet.reserve(rawPoints.size());
	for (const auto& p : rawPoints) {
		_pointSet.insert(p);
	}
}

PointCloud::PointCloud(const std::vector<Point_3>& rawPoints,
	const std::vector<Vector_3>& rawNormals,
	unsigned pointBuffer)
{
	_pointSet.reserve(rawPoints.size());
	_pointSet.add_normal_map();
	for (auto i = 0; i < rawPoints.size(); ++i) {
		auto p = rawPoints[i];
		auto n = rawNormals[i];
		_pointSet.insert(p, n);
	}
}

PointCloud::~PointCloud()
{
	ResourceManager::instance().removeGLBuffer(_pointBufferID);
}

unsigned PointCloud::id() const
{
	return _id;
}

Point_set& PointCloud::pointSet()
{
	return _pointSet;
}

const Point_set& PointCloud::pointSet() const
{
	return _pointSet;
}

unsigned PointCloud::pointBufferID() const
{
	return _pointBufferID;
}

void PointCloud::updateGLBuffer() const
{
	std::vector<QVector3D> points;
	points.reserve(_pointSet.number_of_points());
	for (const auto& p : _pointSet.points()) {
		points.emplace_back(p.x(), p.y(), p.z());
	}

	auto pointBuf = ResourceManager::instance().glBuffer(_pointBufferID);
	pointBuf->bind();
	pointBuf->allocate(points.data(), points.size() * sizeof(QVector3D));
	pointBuf->release();
}
