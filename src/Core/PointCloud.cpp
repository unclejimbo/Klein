#include "Core/PointCloud.h"
#include "Core/GraphicsComponent.h"
#include "Core/ResourceManager.h"
#include "Core/Util.h"
#include "Core/Logger.h"

#include <algorithm>

unsigned PointCloud::_inc = 1;

PointCloud::PointCloud(const std::vector<QVector3D>& rawVertices, 
	const std::vector<QVector3D>& rawNormals, 
	unsigned positionBuffer)
	: positionBufferID(positionBuffer), _id(_inc++)
{
	vertices.resize(rawVertices.size());
	std::transform(rawVertices.begin(), rawVertices.end(), vertices.begin(),
		[](const QVector3D& v) {
			return qtToEigen<float>(v);
		}
	);

	normals.resize(rawNormals.size());
	std::transform(rawNormals.begin(), rawNormals.end(), normals.begin(),
		[](const QVector3D& v) {
			return qtToEigen<float>(v);
		}
	);
}

PointCloud::PointCloud(const std::vector<QVector3D>& rawVertices, unsigned positionBuffer)
	: positionBufferID(positionBuffer), _id(_inc++)
{
	vertices.resize(rawVertices.size());
	std::transform(rawVertices.begin(), rawVertices.end(), vertices.begin(),
		[](const QVector3D& v) {
			return qtToEigen<float>(v);
		}
	);
}

PointCloud::~PointCloud() = default;

unsigned PointCloud::id() const
{
	return _id;
}

void PointCloud::updateGLBuffer() const
{
	std::vector<QVector3D> positions(vertices.size());
	std::transform(vertices.begin(), vertices.end(), positions.begin(),
		[&vertices = vertices](const Eigen::Vector3f& v) {
			return eigenToQt(v);
		}
	);

	auto posBuf = ResourceManager::instance().glBuffer(positionBufferID);
	posBuf->bind();
	posBuf->allocate(positions.data(), positions.size() * sizeof(QVector3D));
	posBuf->release();
}
