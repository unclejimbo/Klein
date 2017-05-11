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
		const std::string& positionBuffer);
	PointCloud(const std::vector<QVector3D>& rawVertices,
		const std::string& positionBuffer);
	~PointCloud();

	void updateGLBuffer() const;
	bool attachTo(GraphicsComponent* graphics);
	GraphicsComponent* attachedGraphics();

	std::vector<Eigen::Vector3f> vertices;
	std::vector<Eigen::Vector3f> normals;

	std::string positionBufferID;
	unsigned pointCloudID;

private:
	GraphicsComponent* _graphics = nullptr;
	static unsigned _count;
};
