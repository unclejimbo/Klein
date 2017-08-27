#pragma once

#include "Core/GeomIO.h"

class OffIO : public GeomIO
{
private:
	bool _readMesh(QTextStream& stream, Geometry& geometry) override;
	bool _readPointCloud(QTextStream& stream, Geometry& geometry) override;
	bool _write(QTextStream& stream, const Geometry& geometry) override;
};
