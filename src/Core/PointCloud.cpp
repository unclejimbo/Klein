#include "Core/PointCloud.h"
#include "Core/ResourceManager.h"
#include "Core/Util.h"
#include "Core/Logger.h"

unsigned PointCloud::_inc = 1;

PointCloud::PointCloud(
	const std::vector<double>& points,
	unsigned pointBuffer)
	: _pointBufferID(pointBuffer)
{
	_pointSet.reserve(points.size() / 3);
	for (auto i = 0; i < points.size(); i += 3) {
		_pointSet.insert(Point_3(points[i], points[i + 1], points[i + 2]));
	}
}

PointCloud::PointCloud(
	const std::vector<double>& points,
	const std::vector<double>& normals,
	unsigned pointBuffer)
	: _pointBufferID(pointBuffer)
{
	_pointSet.reserve(points.size() / 3);
	_pointSet.add_normal_map();
	for (auto i = 0; i < points.size(); i += 3) {
		_pointSet.insert(
			Point_3(points[i], points[i + 1], points[i + 2]),
			Vector_3(normals[i], normals[i + 1], normals[i + 2]));
	}
}

PointCloud::PointCloud(
	const std::vector<Point_3>& points,
	unsigned pointBuffer)
	: _pointBufferID(pointBuffer)
{
	_pointSet.reserve(points.size());
	for (const auto& p : points) {
		_pointSet.insert(p);
	}
}

PointCloud::PointCloud(
	const std::vector<Point_3>& points,
	const std::vector<Vector_3>& normals,
	unsigned pointBuffer)
	: _pointBufferID(pointBuffer)
{
	_pointSet.reserve(points.size());
	_pointSet.add_normal_map();
	for (auto i = 0; i < points.size(); ++i) {
		auto p = points[i];
		auto n = normals[i];
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
		points.emplace_back(static_cast<float>(p.x()),
			static_cast<float>(p.y()), static_cast<float>(p.z()));
	}

	auto pointBuf = ResourceManager::instance().glBuffer(_pointBufferID);
	pointBuf->bind();
	pointBuf->allocate(points.data(), static_cast<int>(points.size() * sizeof(QVector3D)));
	pointBuf->release();
}
