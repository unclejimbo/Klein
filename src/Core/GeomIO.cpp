#include "Core/GeomIO.h"
#include "Core/Logger.h"

#include <QFile>

GeomIO::GeomIO() = default;

GeomIO::~GeomIO() = default;

bool GeomIO::readMesh(const QString& filename, unsigned& positionBufferID, unsigned& normalBufferID,
	GeomInfo* geomInfo)
{
	QFile file(filename);
	if (file.open(QIODevice::ReadOnly)) {
		if (geomInfo != nullptr) {
			geomInfo->type = GEOM_TYPE_MESH;
			geomInfo->fileName = filename;
		}
		QTextStream stream(&file);
		return _readMesh(stream, positionBufferID, normalBufferID, geomInfo);
	}
	else {
		KLEIN_LOG_CRITICAL(QString("Can't open file %1").arg(filename));
		return false;
	}
}

bool GeomIO::readPointCloud(const QString& filename, unsigned& positionBufferID,
	GeomInfo* geomInfo)
{
	QFile file(filename);
	if (file.open(QIODevice::ReadOnly)) {
		if (geomInfo != nullptr) {
			geomInfo->type = GEOM_TYPE_POINTCLOUD;
			geomInfo->fileName = filename;
		}
		QTextStream stream(&file);
		return _readPointCloud(stream, positionBufferID, geomInfo);
	}
	else {
		KLEIN_LOG_CRITICAL(QString("Can't open file %1").arg(filename));
		return false;
	}
}
