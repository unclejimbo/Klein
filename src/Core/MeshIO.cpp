#include "Core/MeshIO.h"
#include "Core/Logger.h"

#ifdef _WIN32
#include <CodeAnalysis/Warnings.h>
#pragma warning(push)
#pragma warning(disable : ALL_CODE_ANALYSIS_WARNINGS)
#endif

#include <QFile>

#ifdef _WIN32
#pragma warning(pop)
#endif

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
