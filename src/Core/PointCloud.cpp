#include "Core/PointCloud.h"
#include "Core/ResourceManager.h"
#include "Core/Util.h"
#include "Core/Logger.h"

unsigned PointCloud::_inc = 1;

PointCloud::PointCloud(const std::vector<QVector3D>& rawPoints, unsigned pointBuffer)
	: _pointBufferID(pointBuffer), _id(_inc++)
{
	_pointSet.reserve(rawPoints.size());
	for (const auto& p : rawPoints) {
		_pointSet.insert(qtToCgal<Point_3>(p));
	}
}

PointCloud::PointCloud(const std::vector<QVector3D>& rawPoints, 
	const std::vector<QVector3D>& rawNormals, 
	unsigned pointBuffer)
	: _pointBufferID(pointBuffer), _id(_inc++)
{
	_pointSet.reserve(rawPoints.size());
	for (auto i = 0; i < rawPoints.size(); ++i) {
		auto p = rawPoints[i];
		auto n = rawNormals[i];
		_pointSet.insert(qtToCgal<Point_3>(p), qtToCgal<Vector_3>(n));
	}
}

PointCloud::~PointCloud() = default;

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
	return 0;
}

void PointCloud::updateGLBuffer() const
{
	std::vector<QVector3D> points(_pointSet.number_of_points());
	for (auto p : _pointSet.points()) {
		points.emplace_back(p.x(), p.y(), p.z());
	}

	auto pointBuf = ResourceManager::instance().glBuffer(_pointBufferID);
	pointBuf->bind();
	pointBuf->allocate(points.data(), points.size() * sizeof(QVector3D));
	pointBuf->release();
}
