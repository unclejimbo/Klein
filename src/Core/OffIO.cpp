#include "Core/OffIO.h"
#include "Core/Logger.h"
#include "Core/ResourceManager.h"

#include <QRegExp>
#include <QVector3D>
#include <vector>

OffIO::OffIO() = default;

OffIO::~OffIO() = default;

inline void recordGeomInfo(GeomInfo* geomInfo, const std::vector<QVector3D>& vertices, int nEdges, int nFaces)
{
	if (geomInfo != nullptr) {
		geomInfo->nVertices = static_cast<int>(vertices.size());
		geomInfo->nEdges = nEdges;
		geomInfo->nFaces = nFaces;
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
}

bool OffIO::_readMesh(QTextStream& stream, const QString& name, bool recordMesh, GeomInfo* geomInfo)
{
	std::vector<QVector3D> vertices;
	std::vector<QVector3D> fnormals;
	std::vector<unsigned> indices;
	std::vector<QVector3D> vertexBuffer;
	std::vector<QVector3D> normalBuffer;

	// OFF file header
	auto line = stream.readLine();
	if (line != "OFF") {
		KLEIN_LOG_CRITICAL("Off mesh file is broken");
		return false;
	}

	// Read vertex count and face count
	size_t vCount = 0;
	size_t fCount = 0;
	while (!stream.atEnd()) {
		line = stream.readLine();
		if (line.isEmpty()) {
			continue;
		}
		auto lineList = line.split(QRegExp("\\s+"));
		if (lineList[0] == "#") {
			continue;
		}
		vCount = lineList[0].toInt();
		fCount = lineList[1].toInt();
		// Ommit edge count
		break;
	}

	// Read mesh data
	vertices.resize(vCount);
	fnormals.resize(fCount);
	indices.resize(fCount * 3);
	normalBuffer.resize(fCount * 3);

	size_t i = 0;
	while (i != vCount) {
		line = stream.readLine();
		if (line.isEmpty()) {
			continue;
		}
		auto lineList = line.split(QRegExp("\\s+"));
		if (lineList[0] == "#") {
			continue;
		}
		auto x = lineList[0].toFloat();
		auto y = lineList[1].toFloat();
		auto z = lineList[2].toFloat();
		vertices[i] = QVector3D(x, y, z);
		++i;
	}

	i = 0;
	while (i != fCount) {
		line = stream.readLine();
		if (line.isEmpty()) {
			continue;
		}
		auto lineList = line.split(QRegExp("\\s+"));
		if (lineList[0] == "#") {
			continue;
		}
		auto n = lineList[0].toUInt();
		auto v1 = lineList[1].toUInt();
		auto v2 = lineList[2].toUInt();
		auto v3 = lineList[3].toUInt();
		if (n != 3) { // Only support triangle mesh for the moment
			KLEIN_LOG_CRITICAL("Klein only supports triangle mesh");
			return false;
		}

		indices[i * 3] = v1;
		indices[i * 3 + 1] = v2;
		indices[i * 3 + 2] = v3;

		// Compute normal
		auto norm = QVector3D::crossProduct(vertices[v2] - vertices[v1], vertices[v3] - vertices[v2]).normalized();
		fnormals[i] = norm;
		normalBuffer[i * 3] = norm;
		normalBuffer[i * 3 + 1] = norm;
		normalBuffer[i * 3 + 2] = norm;
		++i;
	}

	vertexBuffer.resize(fCount * 3);
	std::transform(indices.begin(), indices.end(), vertexBuffer.begin(),
		[&vertices](unsigned i) { return vertices[i]; });

	auto vertexBufferName = QString(name).append("_VertexBuffer");
	auto normalBufferName = QString(name).append("_NormalBuffer");
	ResourceManager::instance().addGLBuffer(vertexBufferName.toStdString(), vertexBuffer);
	ResourceManager::instance().addGLBuffer(normalBufferName.toStdString(), normalBuffer);

	if (recordMesh) {
		ResourceManager::instance().addMesh(name.toStdString(), vertices, fnormals, indices,
			vertexBufferName.toStdString(), normalBufferName.toStdString());
	}

	recordGeomInfo(geomInfo, vertices, -1, static_cast<int>(fnormals.size()));

	return true;
}

bool OffIO::_readPointCloud(QTextStream& stream, const QString& name, GeomInfo* geomInfo)
{
	std::vector<QVector3D> vertices;

	// OFF file header
	auto line = stream.readLine();
	if (line != "OFF") {
		KLEIN_LOG_CRITICAL("Off mesh file is broken");
		return false;
	}

	// Read vertex count and face count
	size_t vCount = 0;
	size_t fCount = 0;
	while (!stream.atEnd()) {
		line = stream.readLine();
		if (line.isEmpty()) {
			continue;
		}
		auto lineList = line.split(QRegExp("\\s+"));
		if (lineList[0] == "#") {
			continue;
		}
		vCount = lineList[0].toInt();
		fCount = lineList[1].toInt();
		// Ommit edge count
		break;
	}

	// Read mesh data
	vertices.resize(vCount);
	size_t i = 0;
	while (i != vCount) {
		line = stream.readLine();
		if (line.isEmpty()) {
			continue;
		}
		auto lineList = line.split(QRegExp("\\s+"));
		if (lineList[0] == "#") {
			continue;
		}
		auto x = lineList[0].toFloat();
		auto y = lineList[1].toFloat();
		auto z = lineList[2].toFloat();
		vertices[i] = QVector3D(x, y, z);
		++i;
	}

	auto vertexBufferName = QString(name).append("_VertexBuffer");
	ResourceManager::instance().addGLBuffer(vertexBufferName.toStdString(), vertices);

	ResourceManager::instance().addPointCloud(name.toStdString(), vertices, vertexBufferName.toStdString());

	recordGeomInfo(geomInfo, vertices, -1, -1);

	return true;
}
