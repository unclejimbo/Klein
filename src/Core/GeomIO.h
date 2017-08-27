#pragma once

#include <QString>
#include <QTextStream>
#include <QVector3D>
#include <vector>

struct Geometry
{
	std::vector<double> vertices;
	std::vector<double> texcoords;
	std::vector<double> normals;
	std::vector<unsigned> vindices;
	std::vector<unsigned> tindices;
	std::vector<unsigned> nindices;
};

class GeomIO
{
public:
	bool readMesh(const QString& filename, Geometry& geometry);
	bool readPointCloud(const QString& filename, Geometry& geometry);
	bool write(const QString& filename, const Geometry& geometry);

private:
	virtual bool _readMesh(QTextStream& stream, Geometry& geometry) = 0;
	virtual bool _readPointCloud(QTextStream& stream, Geometry& geometry) = 0;
	virtual bool _write(QTextStream& stream, const Geometry& geoemtry) = 0;
};
