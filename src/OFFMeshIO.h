#pragma once

#include "MeshIO.h"

class OFFMeshIO : public MeshIO
{
public:
	OFFMeshIO();
	~OFFMeshIO();

private:
	bool _readMesh(QTextStream& stream, const QString& name, bool buildMesh, MeshInfo* meshInfo) override;
};
