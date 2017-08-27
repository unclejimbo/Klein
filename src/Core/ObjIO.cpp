#include "Core/ObjIO.h"
#include "Core/Logger.h"

#include <QRegExp>

static bool readOBJFile(
	QTextStream& stream,
	std::vector<double>* vertices,
	std::vector<double>* texcoords,
	std::vector<double>* normals,
	std::vector<unsigned>* vIndices,
	std::vector<unsigned>* tIndices,
	std::vector<unsigned>* nIndices)
{
	while (!stream.atEnd()) {
		auto line = stream.readLine();

		if (line.isEmpty()) {
			continue;
		}

		auto lineList = line.split(QRegExp("\\s+"));

		if (lineList[0] == "v" && vertices != nullptr) {
			auto x = lineList[1].toDouble();
			auto y = lineList[2].toDouble();
			auto z = lineList[3].toDouble();
			vertices->push_back(x);
			vertices->push_back(y);
			vertices->push_back(z);
		}

		if (lineList[0] == "vt" && texcoords != nullptr) {
			auto x = lineList[1].toDouble();
			auto y = lineList[2].toDouble();
			texcoords->push_back(x);
			texcoords->push_back(y);
		}

		if (lineList[0] == "vn" && normals != nullptr) {
			auto x = lineList[1].toDouble();
			auto y = lineList[2].toDouble();
			auto z = lineList[3].toDouble();
			normals->push_back(x);
			normals->push_back(y);
			normals->push_back(z);
		}

		if (lineList[0] == "f") {
			if (lineList.size() > 4) { // Only support triangle mesh for now
				KLEIN_LOG_CRITICAL("Klein only supports triangle mesh");
				return false;
			}

			for (auto i = 0; i < 3; ++i) {
				auto v = lineList[i + 1];
				auto numList = v.split("/");

				// Vertex index
				if (vIndices != nullptr) {
					auto vIndex = numList[0].toInt();
					vIndex = vIndex > 0 ? // Index might be negative, which means counting backwards
						vIndex - 1 : static_cast<int>(vertices->size()) + vIndex;
					vIndices->push_back(vIndex);
				}

				// Texcoord index (optional)
				if (numList.size() > 1 && tIndices != nullptr) {
					auto tIndex = numList[1].toInt();
					tIndex = tIndex > 0 ?
						tIndex - 1 : static_cast<int>(texcoords->size()) + tIndex;
					tIndices->push_back(tIndex);
				}

				// Normal index (optional)
				if (numList.size() > 2 && nIndices != nullptr) {
					auto nIndex = numList[2].toInt();
					nIndex = nIndex > 0 ?
						nIndex - 1 : static_cast<int>(normals->size()) + nIndex;
					nIndices->push_back(nIndex);
				}
			}
		}
	}
	return true;
}

bool ObjIO::_readMesh(QTextStream& stream, Geometry& geometry)
{
	return readOBJFile(stream,
		&geometry.vertices, &geometry.texcoords, &geometry.normals,
		&geometry.vindices, &geometry.tindices, &geometry.nindices);
}

bool ObjIO::_readPointCloud(QTextStream& stream, Geometry& geometry)
{
	return readOBJFile(stream,
		&geometry.vertices, nullptr, &geometry.normals,
		nullptr, nullptr, nullptr);
}

bool ObjIO::_write(QTextStream& stream, const Geometry& geometry)
{
	return false;
}
