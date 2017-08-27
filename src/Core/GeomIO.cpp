#include "Core/GeomIO.h"
#include "Core/Logger.h"

#include <QFile>

bool GeomIO::readMesh(const QString& filename, Geometry& geometry)
{
	QFile file(filename);
	if (file.open(QIODevice::ReadOnly)) {
		QTextStream stream(&file);
		return _readMesh(stream, geometry);
	}
	else {
		KLEIN_LOG_CRITICAL(QString("Can't open file %1").arg(filename));
		return false;
	}
}

bool GeomIO::readPointCloud(const QString& filename, Geometry& geometry)
{
	QFile file(filename);
	if (file.open(QIODevice::ReadOnly)) {
		QTextStream stream(&file);
		return _readPointCloud(stream, geometry);
	}
	else {
		KLEIN_LOG_CRITICAL(QString("Can't open file %1").arg(filename));
		return false;
	}
}

bool GeomIO::write(const QString& filename, const Geometry& geometry)
{
	QFile file(filename);
	if (file.open(QIODevice::WriteOnly)) {
		QTextStream stream(&file);
		return _write(stream, geometry);
	}
	else {
		KLEIN_LOG_CRITICAL(QString("Can't open file %1").arg(filename));
		return false;
	}
}
