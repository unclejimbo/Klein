#pragma once

#include "Core/GeomIO.h"

class OffIO : public GeomIO
{
public:
	OffIO();
	~OffIO();

private:
	bool _readMesh(QTextStream& stream, const QString& name, bool recordMesh, GeomInfo* meshInfo) override;
	bool _readPointCloud(QTextStream& stream, const QString& name, GeomInfo* geomInfo) override;
};
