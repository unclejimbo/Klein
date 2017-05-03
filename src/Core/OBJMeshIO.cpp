#include "Core/OBJMeshIO.h"
#include "Core/Logger.h"
#include "Core/ResourceManager.h"

#include <QRegExp>
#include <QVector3D>
#include <vector>

OBJMeshIO::OBJMeshIO() = default;

OBJMeshIO::~OBJMeshIO() = default;

bool OBJMeshIO::_readMesh(QTextStream& stream, const QString& name, bool buildMesh, MeshInfo* meshInfo)
{
	std::vector<QVector3D> vertices;
	std::vector<QVector3D> fnormals;
	std::vector<unsigned> indices;
	std::vector<QVector3D> vertexBuffer;
	std::vector<QVector3D> normalBuffer;

	while (!stream.atEnd()) {
		auto line = stream.readLine();

		if (line.isEmpty()) {
			continue;
		}

		auto lineList = line.split(QRegExp("\\s+"));

		if (lineList[0] == "v") {
			auto x = lineList[1].toFloat();
			auto y = lineList[2].toFloat();
			auto z = lineList[3].toFloat();
			vertices.emplace_back(x, y, z);
		}

		if (lineList[0] == "vt") {

		}

		if (lineList[0] == "vn") {
			auto x = lineList[1].toFloat();
			auto y = lineList[2].toFloat();
			auto z = lineList[3].toFloat();
			fnormals.emplace_back(x, y, z);
		}

		if (lineList[0] == "f") {
			if (lineList.size() > 4) { // Only support triangle mesh for now
				KLEIN_LOG_CRITICAL("Klein only supports triangle mesh");
			}
			for (auto i = 0; i < 3; ++i) {
				auto v = lineList[i + 1];
				auto numList = v.split("/");
				auto vertexIndex = numList[0].toInt() - 1;
				indices.push_back(vertexIndex);
				// Ommit texcoord
				auto normalIndex = numList[2].toInt() - 1;
				vertexBuffer.push_back(vertices[vertexIndex]);
				normalBuffer.push_back(fnormals[normalIndex]);
			}
		}
	}

	auto vertexBufferName = QString(name).append("_VertexBuffer");
	auto normalBufferName = QString(name).append("_NormalBuffer");
	ResourceManager::instance().addGLBuffer(vertexBufferName.toStdString(), vertexBuffer);
	ResourceManager::instance().addGLBuffer(normalBufferName.toStdString(), normalBuffer);

	if (buildMesh) {
		ResourceManager::instance().addMesh(name.toStdString(), vertices, fnormals, indices,
			vertexBufferName.toStdString(), normalBufferName.toStdString());
	}

	if (meshInfo != nullptr) {
		meshInfo->nVertices = vertices.size();
		meshInfo->nFaces = vertexBuffer.size() / 3;
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
		meshInfo->center = QVector3D((xmax + xmin) * 0.5f, (ymax + ymin) * 0.5f, (zmax + zmin) * 0.5f);
		meshInfo->radius = QVector3D(xmax - xmin, ymax - ymin, zmax - zmin).length() * 0.5f;
		meshInfo->minX = xmin;
		meshInfo->maxX = xmax;
		meshInfo->minY = ymin;
		meshInfo->maxY = ymax;
		meshInfo->minZ = zmin;
		meshInfo->maxZ = zmax;
	}
	return true;
}
