#include "Core/XyzIO.h"
#include "Core/Logger.h"

#include <QRegExp>

bool XyzIO::_readMesh(QTextStream& stream, Geometry& geometry)
{
	KLEIN_LOG_WARNING("XYZ file format doesn't support triangle mesh data");
	return false;
}

bool XyzIO::_readPointCloud(QTextStream& stream, Geometry& geometry)
{
	auto line = stream.readLine();
	auto count = line.toInt();
	geometry.vertices.reserve(count);
	geometry.normals.reserve(count);
	for (auto i = 0; i < count; ++i) {
		auto line = stream.readLine();
		auto lineList = line.split(QRegExp("\\s+"));
		auto vx = lineList[0].toDouble();
		auto vy = lineList[1].toDouble();
		auto vz = lineList[2].toDouble();
		auto nx = lineList[3].toDouble();
		auto ny = lineList[4].toDouble();
		auto nz = lineList[5].toDouble();
		geometry.vertices.push_back(vx);
		geometry.vertices.push_back(vy);
		geometry.vertices.push_back(vz);
		geometry.normals.push_back(nx);
		geometry.normals.push_back(ny);
		geometry.normals.push_back(nz);
	}
	return true;
}

bool XyzIO::_write(QTextStream& stream, const Geometry& geometry)
{
	return false;
}
