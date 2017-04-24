#pragma once

#include <QString>
#include <QTextStream>
#include <QVector3D>
#include <vector>

struct MeshInfo
{
	QString fileName;
	size_t nVertices;
	size_t nEdges;
	size_t nFaces;
	QVector3D center;
	float radius;
	float minX;
	float maxX;
	float minY;
	float maxY;
	float minZ;
	float maxZ;
};

class MeshIO
{
public:
	MeshIO();
	~MeshIO();

	bool readMesh(const QString& filename, const QString& name, bool buildMesh, MeshInfo* meshInfo);

private:
	virtual bool _readMesh(QTextStream& stream, const QString& name, bool buildMesh, MeshInfo* meshInfo) = 0;
};
