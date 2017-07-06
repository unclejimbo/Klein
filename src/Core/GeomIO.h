#pragma once

#include <QString>
#include <QTextStream>
#include <QVector3D>

enum GeomType : unsigned
{
	GEOM_TYPE_NONE = 0,
	GEOM_TYPE_MESH,
	GEOM_TYPE_POINTCLOUD
};

struct GeomInfo
{
	GeomType type;
	unsigned id;
	QString fileName;
	int nVertices;
	int nEdges;
	int nFaces;
	QVector3D center;
	float minX;
	float maxX;
	float minY;
	float maxY;
	float minZ;
	float maxZ;
};

class GeomIO
{
public:
	bool readMesh(const QString& filename, unsigned& positionBufferID, unsigned& normalBufferID,
		GeomInfo* geomInfo = nullptr);
	bool readPointCloud(const QString& filename, unsigned& positionBufferID,
		GeomInfo* geomInfo = nullptr);

private:
	virtual bool _readMesh(QTextStream& stream, unsigned& positionBufferID, unsigned& normalBufferID,
		GeomInfo* geomInfo) = 0;
	virtual bool _readPointCloud(QTextStream& stream, unsigned& positionBufferID,
		GeomInfo* geomInfo) = 0;
};
