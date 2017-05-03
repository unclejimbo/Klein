#pragma once

#include <QVector3D>
#include <Eigen/Dense>
#include <vector>
#include <string>

class PointCloud
{
public:
	PointCloud(const std::vector<QVector3D>& rawVertices,
		const std::vector<QVector3D>& rawNormals,
		const std::string& positionBuffer);
	~PointCloud();

	void updateGLBuffer() const;

	std::vector<Eigen::Vector3f> vertices;
	std::vector<Eigen::Vector3f> normals;

	std::string positionBufferID;
};
