#include "Core/OFFMeshIO.h"
#include "Core/Logger.h"
#include "Core/ResourceManager.h"

#include <QRegExp>
#include <QVector3D>
#include <vector>

OFFMeshIO::OFFMeshIO() = default;

OFFMeshIO::~OFFMeshIO() = default;

bool OFFMeshIO::_readMesh(QTextStream& stream, const QString& name, bool buildMesh, MeshInfo* meshInfo)
{
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
	std::vector<QVector3D> vertices(vCount);
	std::vector<QVector3D> fnormals(fCount);
	std::vector<unsigned> indices(fCount * 3);
	std::vector<QVector3D> vertexBuffer(fCount * 3);
	std::vector<QVector3D> normalBuffer(fCount * 3);

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

	std::transform(indices.begin(), indices.end(), vertexBuffer.begin(),
		[&vertices](unsigned i) { return vertices[i]; });

	auto vertexBufferName = QString(name).append("_VertexBuffer");
	auto normalBufferName = QString(name).append("_NormalBuffer");
	ResourceManager::instance().addGLBuffer(vertexBufferName.toStdString(), vertexBuffer);
	ResourceManager::instance().addGLBuffer(normalBufferName.toStdString(), normalBuffer);

	if (buildMesh) {
		ResourceManager::instance().addMesh(name.toStdString(), vertices, fnormals, indices,
			vertexBufferName.toStdString(), normalBufferName.toStdString());
	}
	
	if (meshInfo != nullptr) {
		meshInfo->nVertices = vCount;
		meshInfo->nFaces = fCount;
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
		meshInfo->center = QVector3D(xmax + xmin, ymax + ymin, zmax + zmin) * 0.5f;
		meshInfo->radius = QVector3D(xmax -xmin, ymax - ymin, zmax - zmin).length() * 0.5f;
		meshInfo->minX = xmin;
		meshInfo->maxX = xmax;
		meshInfo->minY = ymin;
		meshInfo->maxY = ymax;
		meshInfo->minZ = zmin;
		meshInfo->maxZ = zmax;
	}
	return true;
}
