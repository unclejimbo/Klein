#pragma once

#include "Core/GeomIO.h"

class PlyIO : public GeomIO
{
public:
	PlyIO();
	~PlyIO();

private:
	bool _readMesh(QTextStream& stream, const QString& name, bool recordMesh, GeomInfo* geomInfo) override;
	bool _readPointCloud(QTextStream& stream, const QString& name, GeomInfo* geomInfo) override;
};
