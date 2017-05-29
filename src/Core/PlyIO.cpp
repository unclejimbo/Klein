#include "Core/PlyIO.h"
#include "Core/Logger.h"
#include "Core/ResourceManager.h"

#include <QRegExp>
#include <QVector3D>
#include <vector>

PlyIO::PlyIO() = default;

PlyIO::~PlyIO() = default;

enum class PlyFormat
{
	ascii,
	binary_little_endian,
	binary_big_endians
};

inline bool readPlyHeader(QTextStream& stream, PlyFormat& format,
	int& nVertices, int& nFaces, bool& hasNormal)
{
	auto line = stream.readLine();
	if (line != "ply") {
		KLEIN_LOG_CRITICAL("Bad ply file");
		return false;
	}

	line = stream.readLine();
	auto lineList = line.split(QRegExp("\\s+"));
	if (lineList[0] != "format") {
		KLEIN_LOG_CRITICAL("Bad ply file");
		return false;
	}
	else {
		if (lineList[1] == "ascii") {
			format = PlyFormat::ascii;
		}
		else if (lineList[1] == "binary_little_endian") {
			format = PlyFormat::binary_little_endian;
		}
		else if (lineList[1] == "binary_big_endian") {
			format = PlyFormat::binary_big_endians;
		}
		else {
			KLEIN_LOG_CRITICAL("Bad ply file");
			return false;
		}
	}

	while (!stream.atEnd()) {
		line = stream.readLine();
		lineList = line.split(QRegExp("\\s+"));
		if (lineList[0] == "element" && lineList[1] == "vertex") {
			nVertices = lineList[2].toInt();
		}
		else if (lineList[0] == "property" && lineList[2] == "nx") {
			hasNormal = true;
		}
		else if (lineList[0] == "element" && lineList[1] == "face") {
			nFaces = lineList[2].toInt();
			hasNormal = false;
		}
		else if (lineList[0] == "end_header") {
			return true;
		}
		else {
			// Empty
		}
	}

	KLEIN_LOG_CRITICAL("Bad ply file");
	return false;
}

// Without normals
inline bool readPlyAscii(QTextStream& stream, std::vector<QVector3D>& vertices, std::vector<unsigned>& vIndices)
{
	auto nVertices = vertices.size();
	auto nFaces = vIndices.size() / 3;

	for (auto i = 0; i < nVertices; ++i) {
		auto line = stream.readLine();
		auto lineList = line.split(QRegExp("\\s+"));
		auto x = lineList[0].toFloat();
		auto y = lineList[1].toFloat();
		auto z = lineList[2].toFloat();
		vertices[i] = QVector3D(x, y, z);
	}

	for (auto i = 0; i < nFaces; ++i) {
		auto line = stream.readLine();
		auto lineList = line.split(QRegExp("\\s+"));
		if (lineList[0].toInt() != 3) {
			KLEIN_LOG_CRITICAL("Klein only supports triangle mesh");
			return false;
		}
		auto v1 = lineList[1].toUInt();
		auto v2 = lineList[2].toUInt();
		auto v3 = lineList[3].toUInt();
		vIndices[i * 3 + 0] = v1;
		vIndices[i * 3 + 1] = v2;
		vIndices[i * 3 + 2] = v3;
	}

	return true;
}

// With vertex normals
inline bool readPlyAscii(QTextStream& stream, std::vector<QVector3D>& vertices,
	std::vector<QVector3D>& normals, std::vector<unsigned>& vIndices)
{
	auto nVertices = vertices.size();
	auto nFaces = vIndices.size() / 3;

	for (auto i = 0; i < nVertices; ++i) {
		auto line = stream.readLine();
		auto lineList = line.split(QRegExp("\\s+"));
		auto x = lineList[0].toFloat();
		auto y = lineList[1].toFloat();
		auto z = lineList[2].toFloat();
		vertices[i] = QVector3D(x, y, z);
		auto nx = lineList[3].toFloat();
		auto ny = lineList[4].toFloat();
		auto nz = lineList[5].toFloat();
		normals[i] = QVector3D(nx, ny, nz);
	}

	for (auto i = 0; i < nFaces; ++i) {
		auto line = stream.readLine();
		auto lineList = line.split(QRegExp("\\s+"));
		if (lineList[0].toInt() != 3) {
			KLEIN_LOG_CRITICAL("Klein only supports triangle mesh");
			return false;
		}
		auto v1 = lineList[1].toUInt();
		auto v2 = lineList[2].toUInt();
		auto v3 = lineList[3].toUInt();
		vIndices[i * 3 + 0] = v1;
		vIndices[i * 3 + 1] = v2;
		vIndices[i * 3 + 2] = v3;
	}

	return true;
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

bool PlyIO::_readMesh(QTextStream& stream, const QString& name, bool recordMesh, GeomInfo* geomInfo)
{
	PlyFormat format;
	int nVertices = 0;
	int nFaces = 0;
	bool hasNormal = false;
	if (!readPlyHeader(stream, format, nVertices, nFaces, hasNormal)) {
		return false;
	}

	std::vector<QVector3D> vertices(nVertices);
	std::vector<QVector3D> normals(nVertices);
	std::vector<unsigned> vIndices(nFaces * 3);
	std::vector<QVector3D> vertexBuffer(vIndices.size());
	std::vector<QVector3D> normalBuffer(vIndices.size());
	if (format == PlyFormat::ascii) {
		if (hasNormal) {
			if (!readPlyAscii(stream, vertices, normals, vIndices)) {
				return false;
			}
			else {
				std::transform(vIndices.begin(), vIndices.end(), vertexBuffer.begin(),
					[&vertices](unsigned i) { return vertices[i]; });
				std::transform(vIndices.begin(), vIndices.end(), normalBuffer.begin(),
					[&normals](unsigned i) { return normals[i]; });
			}
		}
		else {
			if (!readPlyAscii(stream, vertices, vIndices)) {
				return false;
			}
			else {
				std::transform(vIndices.begin(), vIndices.end(), vertexBuffer.begin(),
					[&vertices](unsigned i) { return vertices[i]; });
				for (auto i = 0; i < nFaces; ++i) {
					auto v1 = vertices[vIndices[i * 3 + 0]];
					auto v2 = vertices[vIndices[i * 3 + 1]];
					auto v3 = vertices[vIndices[i * 3 + 2]];
					auto n = QVector3D::crossProduct(v2 - v1, v3 - v1).normalized();
					normalBuffer[i * 3 + 0] = n;
					normalBuffer[i * 3 + 1] = n;
					normalBuffer[i * 3 + 2] = n;
				}
			}
		}
	}
	else if (format == PlyFormat::binary_little_endian) {
		// TODO
		KLEIN_LOG_CRITICAL("Klein doesn't support binary ply format");
		return false;
	}
	else { // big_endian
		// TODO
		KLEIN_LOG_CRITICAL("Klein doesn't support binary ply format");
		return false;
	}

	// Construct OpenGL buffers
	auto vertexBufferName = QString(name).append("_VertexBuffer");
	auto normalBufferName = QString(name).append("_NormalBuffer");
	ResourceManager::instance().addGLBuffer(vertexBufferName.toStdString(), vertexBuffer);
	ResourceManager::instance().addGLBuffer(normalBufferName.toStdString(), normalBuffer);

	if (recordMesh) {
		std::vector<QVector3D> fNormals(nFaces);
		if (!hasNormal) {
			for (auto i = 0; i < nFaces; ++i) {
				fNormals[i] = normalBuffer[i * 3];
			}
		}
		else {
			for (auto i = 0; i < nFaces; ++i) {
				auto v1 = vertices[vIndices[i * 3 + 0]];
				auto v2 = vertices[vIndices[i * 3 + 1]];
				auto v3 = vertices[vIndices[i * 3 + 2]];
				auto n = QVector3D::crossProduct(v2 - v1, v3 - v1).normalized();
				fNormals[i] = n;
			}
		}
		ResourceManager::instance().addMesh(name.toStdString(), vertices, normals, vIndices,
			vertexBufferName.toStdString(), normalBufferName.toStdString());
	}

	recordGeomInfo(geomInfo, vertices, nVertices, nFaces);

	return true;
}

bool PlyIO::_readPointCloud(QTextStream& stream, const QString& name, GeomInfo* geomInfo)
{
	PlyFormat format;
	int nVertices = 0;
	int nFaces = 0;
	bool hasNormal = false;
	if (!readPlyHeader(stream, format, nVertices, nFaces, hasNormal)) {
		return false;
	}

	std::vector<QVector3D> vertices(nVertices);
	std::vector<QVector3D> normals(nVertices);
	std::vector<unsigned> vIndices(nFaces * 3);
	if (format == PlyFormat::ascii) {
		if (hasNormal) {
			if (!readPlyAscii(stream, vertices, normals, vIndices)) {
				return false;
			}
		}
		else {
			if (!readPlyAscii(stream, vertices, vIndices)) {
				return false;
			}
		}
	}
	else if (format == PlyFormat::binary_little_endian) {
		// TODO
		return false;
	}
	else { // big_endian
		// TODO
		return false;
	}

	// Construct OpenGL buffers
	auto vertexBufferName = QString(name).append("_VertexBuffer");
	ResourceManager::instance().addGLBuffer(vertexBufferName.toStdString(), vertices);

	if (hasNormal) {
		ResourceManager::instance().addPointCloud(name.toStdString(), vertices, normals, vertexBufferName.toStdString());
	}
	else {
		ResourceManager::instance().addPointCloud(name.toStdString(), vertices, vertexBufferName.toStdString());
	}

	recordGeomInfo(geomInfo, vertices, nVertices, -1);

	return true;
}
