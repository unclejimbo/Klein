#include "Core/OffIO.h"
#include "Core/Logger.h"

#include <QRegExp>

static bool readOffFile(
	QTextStream& stream,
	std::vector<double>* vertices,
	std::vector<unsigned>* indices)
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
	vertices->reserve(vCount * 3);
	indices->reserve(fCount * 3);

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
		auto x = lineList[0].toDouble();
		auto y = lineList[1].toDouble();
		auto z = lineList[2].toDouble();
		vertices->push_back(x);
		vertices->push_back(y);
		vertices->push_back(z);
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
		indices->push_back(v1);
		indices->push_back(v2);
		indices->push_back(v3);
		++i;
	}
	return true;
}

bool OffIO::_readMesh(QTextStream& stream, Geometry& geometry)
{
	return readOffFile(stream, &geometry.vertices, &geometry.vindices);
}

bool OffIO::_readPointCloud(QTextStream& stream, Geometry& geometry)
{
	return readOffFile(stream, &geometry.vertices, nullptr);
}
