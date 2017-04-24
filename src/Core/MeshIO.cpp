#include "Core/MeshIO.h"
#include "Core/Logger.h"

#include <QFile>

MeshIO::MeshIO() = default;

MeshIO::~MeshIO() = default;

bool MeshIO::readMesh(const QString& filename, const QString& name, bool buildMesh, MeshInfo* meshInfo)
{
	QFile file(filename);
	if (file.open(QIODevice::ReadOnly)) {
		if (meshInfo != nullptr) {
			meshInfo->fileName = filename;
		}
		QTextStream stream(&file);
		return _readMesh(stream, name, buildMesh, meshInfo);
	}
	else {
		KLEIN_LOG_CRITICAL(QString("Can't open file %1").arg(filename));
		return false;
	}
}
