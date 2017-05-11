#include "Core/PointCloud.h"
#include "Core/GraphicsComponent.h"
#include "Core/ResourceManager.h"
#include "Core/Util.h"
#include "Core/Logger.h"

#include <algorithm>

PointCloud::PointCloud(const std::vector<QVector3D>& rawVertices, 
	const std::vector<QVector3D>& rawNormals, 
	const std::string & positionBuffer)
	: positionBufferID(positionBuffer), pointCloudID(_count++)
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

PointCloud::PointCloud(const std::vector<QVector3D>& rawVertices, const std::string& positionBuffer)
	: positionBufferID(positionBuffer)
{
	vertices.resize(rawVertices.size());
	std::transform(rawVertices.begin(), rawVertices.end(), vertices.begin(),
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

bool PointCloud::attachTo(GraphicsComponent* graphics)
{
	if (_graphics == nullptr) {
		_graphics = graphics;
		return true;
	}
	else {
		KLEIN_LOG_CRITICAL("This mesh has already been attached to another GraphicsComponent");
		return false;
	}
}

GraphicsComponent * PointCloud::attachedGraphics()
{
	return _graphics;
}

unsigned PointCloud::_count = 0;
