#include "Core/GeomIO.h"
#include "Core/Logger.h"

#include <QFile>

GeomIO::GeomIO() = default;

GeomIO::~GeomIO() = default;

bool GeomIO::readMesh(const QString& filename, const QString& name, bool recordMesh, GeomInfo* geomInfo)
{
	QFile file(filename);
	if (file.open(QIODevice::ReadOnly)) {
		if (geomInfo != nullptr) {
			geomInfo->fileName = filename;
		}
		QTextStream stream(&file);
		return _readMesh(stream, name, recordMesh, geomInfo);
	}
	else {
		KLEIN_LOG_CRITICAL(QString("Can't open file %1").arg(filename));
		return false;
	}
}

bool GeomIO::readPointCloud(const QString& filename, const QString& name, GeomInfo* geomInfo)
{
	QFile file(filename);
	if (file.open(QIODevice::ReadOnly)) {
		if (geomInfo != nullptr) {
			geomInfo->fileName = filename;
		}
		QTextStream stream(&file);
		return _readPointCloud(stream, name, geomInfo);
	}
	else {
		KLEIN_LOG_CRITICAL(QString("Can't open file %1").arg(filename));
		return false;
	}
}
