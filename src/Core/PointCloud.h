#pragma once

#include <QVector3D>
#include <Eigen/Dense>
#include <vector>
#include <string>

class GraphicsComponent;

class PointCloud
{
public:
	PointCloud(const std::vector<QVector3D>& rawVertices,
		const std::vector<QVector3D>& rawNormals,
		unsigned positionBuffer);
	PointCloud(const std::vector<QVector3D>& rawVertices,
		unsigned positionBuffer);
	~PointCloud();

	unsigned id() const;
	void updateGLBuffer() const;

	std::vector<Eigen::Vector3f> vertices;
	std::vector<Eigen::Vector3f> normals;

	unsigned positionBufferID;

private:
	GraphicsComponent* _graphics = nullptr;
	unsigned _id;
	static unsigned _inc;
};
