#include "Core/XyzIO.h"
#include "Core/Logger.h"
#include "Core/ResourceManager.h"

#include <QRegExp>
#include <QVector3D>
#include <vector>

XyzIO::XyzIO() = default;

XyzIO::~XyzIO() = default;

bool XyzIO::_readMesh(QTextStream& stream, unsigned& positionBufferID, unsigned& normalBufferID,
	GeomInfo* geomInfo)
{
	KLEIN_LOG_WARNING("XYZ file format doesn't support triangle mesh data");
	return false;
}

bool XyzIO::_readPointCloud(QTextStream& stream, unsigned& positionBufferID,
	GeomInfo* geomInfo)
{
	std::vector<QVector3D> vertices;
	std::vector<QVector3D> normals;

	// Read file
	auto line = stream.readLine();
	auto count = line.toInt();
	vertices.reserve(count);
	normals.reserve(count);
	for (auto i = 0; i < count; ++i) {
		auto line = stream.readLine();
		auto lineList = line.split(QRegExp("\\s+"));
		auto vx = lineList[0].toFloat();
		auto vy = lineList[1].toFloat();
		auto vz = lineList[2].toFloat();
		auto nx = lineList[3].toFloat();
		auto ny = lineList[4].toFloat();
		auto nz = lineList[5].toFloat();
		vertices.emplace_back(vx, vy, vz);
		normals.emplace_back(nx, ny, nz);
	}

	// Construct OpenGL buffers
	positionBufferID = ResourceManager::instance().addGLBuffer(vertices, GL_POINTS);

	// Record geomInfo
	if (geomInfo != nullptr) {
		geomInfo->id = ResourceManager::instance().addPointCloud(vertices, normals, positionBufferID);
		geomInfo->nVertices = static_cast<int>(vertices.size());
		geomInfo->nEdges = -1;
		geomInfo->nFaces = -1;
		auto xmin = vertices[0].x();
		auto xmax = vertices[0].x();
		auto ymin = vertices[0].y();
		auto ymax = vertices[0].y();
		auto zmin = vertices[0].z();
		auto zmax = vertices[0].z();
		for (const auto& v : vertices) {
			if (v.x() < xmin) {
				xmin = v.x();
			}
			if (v.x() > xmax) {
				xmax = v.x();
			}
			if (v.y() < ymin) {
				ymin = v.y();
			}
			if (v.y() > ymax) {
				ymax = v.y();
			}
			if (v.z() < zmin) {
				zmin = v.z();
			}
			if (v.z() > zmax) {
				zmax = v.z();
			}
		}
		geomInfo->center = QVector3D((xmax + xmin) * 0.5f, (ymax + ymin) * 0.5f, (zmax + zmin) * 0.5f);
		geomInfo->radius = QVector3D(xmax - xmin, ymax - ymin, zmax - zmin).length() * 0.5f;
		geomInfo->minX = xmin;
		geomInfo->maxX = xmax;
		geomInfo->minY = ymin;
		geomInfo->maxY = ymax;
		geomInfo->minZ = zmin;
		geomInfo->maxZ = zmax;
	}

	return true;
}


