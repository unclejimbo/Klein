#pragma once

#include "Core/GeomIO.h"

class XyzIO : public GeomIO
{
private:
	bool _readMesh(QTextStream& stream, Geometry& geometry) override;
	bool _readPointCloud(QTextStream& stream, Geometry& geometry) override;
};
