#include "Core/PointCloud.h"
#include "Core/ResourceManager.h"
#include "Core/Util.h"

#include <algorithm>

PointCloud::PointCloud(const std::vector<QVector3D>& rawVertices, 
	const std::vector<QVector3D>& rawNormals, 
	const std::string & positionBuffer)
	: positionBufferID(positionBuffer)
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

PointCloud::~PointCloud() = default;

void PointCloud::updateGLBuffer() const
{
	std::vector<QVector3D> positions(vertices.size());
	std::transform(vertices.begin(), vertices.end(), positions.begin(),
		[&vertices = vertices](const Eigen::Vector3f& v) {
			return eigenToQt(v);
		}
	);

	ResourceManager::instance().addGLBuffer(positionBufferID, positions);
}
