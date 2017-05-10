#pragma once

#include <QString>
#include <QTextStream>
#include <QVector3D>
#include <vector>

enum GeomType : int
{
	GEOM_TYPE_NONE,
	GEOM_TYPE_MESH,
	GEOM_TYPE_POINTCLOUD
};

struct GeomInfo
{
	GeomType type;
	QString fileName;
	int nVertices;
	int nEdges;
	int nFaces;
	QVector3D center;
	float radius;
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
	GeomIO();
	~GeomIO();

	bool readMesh(const QString& filename, const QString& name, bool recordMesh, GeomInfo* geomInfo = nullptr);
	bool readPointCloud(const QString& filename, const QString& name, GeomInfo* geomInfo = nullptr);

private:
	virtual bool _readMesh(QTextStream& stream, const QString& name, bool recordMesh, GeomInfo* geomInfo) = 0;
	virtual bool _readPointCloud(QTextStream& stream, const QString& name, GeomInfo* geomInfo) = 0;
};
