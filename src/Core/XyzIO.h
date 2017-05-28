#pragma once

#include "Core/GeomIO.h"

class XyzIO : public GeomIO
{
public:
	XyzIO();
	~XyzIO();

private:
	bool _readMesh(QTextStream& stream, const QString& name, bool recordMesh, GeomInfo* meshInfo) override;
	bool _readPointCloud(QTextStream& stream, const QString& name, GeomInfo* geomInfo) override;
};
