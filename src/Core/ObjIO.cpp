#include "Core/ObjIO.h"
#include "Core/Logger.h"
#include "Core/ResourceManager.h"

#include <QRegExp>
#include <QVector3D>
#include <vector>

ObjIO::ObjIO() = default;

ObjIO::~ObjIO() = default;

inline void readObjFile(QTextStream& stream, std::vector<QVector3D>& vertices, std::vector<QVector3D>& vertexBuffer,
	std::vector<QVector3D>* fnormals, std::vector<unsigned>* indices, std::vector<QVector3D>* normalBuffer)
{
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

		if (lineList[0] == "vn" && fnormals != nullptr) {
			auto x = lineList[1].toFloat();
			auto y = lineList[2].toFloat();
			auto z = lineList[3].toFloat();
			fnormals->emplace_back(x, y, z);
		}

		if (lineList[0] == "f") {
			if (lineList.size() > 4) { // Only support triangle mesh for now
				KLEIN_LOG_CRITICAL("Klein only supports triangle mesh");
			}

			// Record vertex indices if normals are not provided in order to compute face normal
			std::vector<unsigned> vertexIndices;
			for (auto i = 0; i < 3; ++i) {
				auto v = lineList[i + 1];
				auto numList = v.split("/");

				auto vertexIndex = numList[0].toInt();
				vertexIndex = vertexIndex > 0 ? vertexIndex - 1 : vertices.size() + vertexIndex;
				vertexBuffer.push_back(vertices[vertexIndex]);
				if (indices != nullptr) {
					indices->push_back(vertexIndex);
				}

				// Ommit texcoord

				if (normalBuffer != nullptr) {
					if (numList.length() == 3) { // Normals are provided
						auto normalIndex = numList[2].toInt();
						normalIndex = normalIndex > 0 ?
							normalIndex - 1 : fnormals->size() + normalIndex;
						normalBuffer->push_back((*fnormals)[normalIndex]);
					}
					else {
						vertexIndices.push_back(vertexIndex);
					}
				}
			}
			if (!vertexIndices.empty()) { // Compute normals
				auto v1 = vertices[vertexIndices[1]] - vertices[vertexIndices[0]];
				auto v2 = vertices[vertexIndices[2]] - vertices[vertexIndices[0]];
				auto fnormal = QVector3D::crossProduct(v1, v2);
				fnormals->push_back(fnormal);
				normalBuffer->push_back(fnormal);
				normalBuffer->push_back(fnormal);
				normalBuffer->push_back(fnormal);
			}
		}
	}
}

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

bool ObjIO::_readMesh(QTextStream& stream, const QString& name, bool recordMesh, GeomInfo* geomInfo)
{
	std::vector<QVector3D> vertices;
	std::vector<QVector3D> fnormals;
	std::vector<unsigned> indices;
	std::vector<QVector3D> vertexBuffer;
	std::vector<QVector3D> normalBuffer;

	readObjFile(stream, vertices, vertexBuffer, &fnormals, &indices, &normalBuffer);

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

bool ObjIO::_readPointCloud(QTextStream& stream, const QString& name, GeomInfo* geomInfo)
{
	std::vector<QVector3D> vertices;
	std::vector<QVector3D> vertexBuffer;

	readObjFile(stream, vertices, vertexBuffer, nullptr, nullptr, nullptr);

	auto vertexBufferName = QString(name).append("_VertexBuffer");
	ResourceManager::instance().addGLBuffer(vertexBufferName.toStdString(), vertexBuffer);

	ResourceManager::instance().addPointCloud(name.toStdString(), vertices, vertexBufferName.toStdString());

	recordGeomInfo(geomInfo, vertices, -1, -1);

	return true;
}
