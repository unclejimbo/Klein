#include "Core/PlyIO.h"
#include "Core/Logger.h"

#include <QRegExp>

enum class PlyFormat
{
	ascii,
	binary_little_endian,
	binary_big_endian
};

static bool readPlyHeader(QTextStream& stream, PlyFormat& format,
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

	if (lineList[1] == "ascii") {
		format = PlyFormat::ascii;
	}
	else if (lineList[1] == "binary_little_endian") {
		format = PlyFormat::binary_little_endian;
	}
	else if (lineList[1] == "binary_big_endian") {
		format = PlyFormat::binary_big_endian;
	}
	else {
		KLEIN_LOG_CRITICAL("Bad ply file");
		return false;
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
			nFaces = lineList[2].toInt();
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

static bool readPlyAscii(
	QTextStream& stream,
	unsigned nVertices,
	unsigned nFaces,
	std::vector<double>* vertices,
	std::vector<double>* normals,
	std::vector<unsigned>* vIndices)
{
	if (vertices != nullptr) {
		vertices->reserve(nVertices * 3);
	}
	if (normals != nullptr) {
		normals->reserve(nVertices * 3);
	}
	if (vIndices != nullptr) {
		vIndices->reserve(nFaces * 3);
	}

	for (unsigned i = 0; i < nVertices; ++i) {
		auto line = stream.readLine();
		auto lineList = line.split(QRegExp("\\s+"));
		if (vertices != nullptr) {
			auto x = lineList[0].toDouble();
			auto y = lineList[1].toDouble();
			auto z = lineList[2].toDouble();
			vertices->push_back(x);
			vertices->push_back(y);
			vertices->push_back(z);
		}
		if (normals != nullptr) {
			auto nx = lineList[3].toDouble();
			auto ny = lineList[4].toDouble();
			auto nz = lineList[5].toDouble();
			normals->push_back(nx);
			normals->push_back(ny);
			normals->push_back(nz);
		}
	}

	for (unsigned i = 0; i < nFaces; ++i) {
		auto line = stream.readLine();
		auto lineList = line.split(QRegExp("\\s+"));
		if (lineList[0].toInt() != 3) {
			KLEIN_LOG_CRITICAL("Klein only supports triangle mesh");
			return false;
		}
		if (vIndices != nullptr) {
			auto v1 = lineList[1].toUInt();
			auto v2 = lineList[2].toUInt();
			auto v3 = lineList[3].toUInt();
			vIndices->push_back(v1);
			vIndices->push_back(v2);
			vIndices->push_back(v3);
		}
	}

	return true;
}

static bool writePlyHeader(QTextStream& stream, const PlyFormat& format,
	int nVertices, int nFaces, bool hasNormal)
{
	stream << "ply" << endl << "format ";
	if (format == PlyFormat::ascii) {
		stream << "ascii";
	}
	else if (format == PlyFormat::binary_big_endian) {
		stream << "binary_big_endian";
	}
	else if (format == PlyFormat::binary_little_endian) {
		stream << "binary_little_endian";
	}
	stream << " 1.0" << endl
		<< "comment generated by Klein" << endl
		<< "element vertex " << nVertices << endl
		<< "property float x" << endl
		<< "property float y" << endl
		<< "property float z" << endl;
	if (hasNormal) {
		stream << "property float nx" << endl
			<< "property float ny" << endl
			<< "property float nz" << endl;
	}
	stream << "element face " << nFaces << endl
		<< "property list uchar int vertex_indices" << endl
		<< "end_header" << endl;
	return true;
}

static bool writePlyAscii(QTextStream& stream, const Geometry& geometry, bool hasNormal)
{
	for (auto i = 0; i < geometry.vertices.size(); i += 3) {
		stream << geometry.vertices[i] << " "
			<< geometry.vertices[i + 1] << " "
			<< geometry.vertices[i + 2];
		if (hasNormal) {
			stream << " " << geometry.normals[i] << " "
				<< geometry.normals[i + 1] << " "
				<< geometry.normals[i + 2];
		}
		stream << endl;
	}
	for (auto i = 0; i < geometry.vindices.size(); i += 3) {
		stream << "3 " << geometry.vindices[i] << " "
			<< geometry.vindices[i + 1] << " "
			<< geometry.vindices[i + 2] << endl;
	}
	return true;
}

bool PlyIO::_readMesh(QTextStream& stream, Geometry& geometry)
{
	PlyFormat format;
	int nVertices = 0;
	int nFaces = 0;
	bool hasNormal = false;
	if (!readPlyHeader(stream, format, nVertices, nFaces, hasNormal)) {
		return false;
	}

	if (format == PlyFormat::ascii) {
		if (hasNormal) {
			if (!readPlyAscii(stream, nVertices, nFaces,
				&geometry.vertices, &geometry.normals, &geometry.vindices)) {
				return false;
			}
		}
		else {
			if (!readPlyAscii(stream, nVertices, nFaces,
				&geometry.vertices, nullptr, &geometry.vindices)) {
				return false;
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

	return true;
}

bool PlyIO::_readPointCloud(QTextStream& stream, Geometry& geometry)
{
	PlyFormat format;
	int nVertices = 0;
	int nFaces = 0;
	bool hasNormal = false;
	if (!readPlyHeader(stream, format, nVertices, nFaces, hasNormal)) {
		return false;
	}

	if (format == PlyFormat::ascii) {
		if (hasNormal) {
			if (!readPlyAscii(stream, nVertices, nFaces,
				&geometry.vertices, &geometry.normals, nullptr)) {
				return false;
			}
		}
		else {
			if (!readPlyAscii(stream, nVertices, nFaces,
				&geometry.vertices, nullptr, nullptr)) {
				return false;
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

	return true;
}

bool PlyIO::_write(QTextStream& stream, const Geometry& geometry)
{
	bool hasNormal = !geometry.normals.empty();
	if (hasNormal && !geometry.nindices.empty()) {
		KLEIN_LOG_WARNING("Ply format requires per-vertex normal, normal indexing is not supported");
		hasNormal = false;
	}
	int nVertices = static_cast<int>(geometry.vertices.size()) / 3;
	int nFaces = static_cast<int>(geometry.vindices.size());

	// TODO: write any type of format
	writePlyHeader(stream, PlyFormat::ascii, nVertices, nFaces, hasNormal);
	writePlyAscii(stream, geometry, hasNormal);

	return true;
}
