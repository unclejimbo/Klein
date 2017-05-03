#pragma once

#include "Core/GeomIO.h"

class ObjIO : public GeomIO
{
public:
	ObjIO();
	~ObjIO();

private:
	bool _readMesh(QTextStream& stream, const QString& name, bool recordMesh, GeomInfo* meshInfo) override;
	bool _readPointCloud(QTextStream& stream, const QString& name, GeomInfo* geomInfo) override;
};
