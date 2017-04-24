#pragma once

#include "Core/MeshIO.h"

class OBJMeshIO : public MeshIO
{
public:
	OBJMeshIO();
	~OBJMeshIO();

private:
	bool _readMesh(QTextStream& stream, const QString& name, bool buildMesh, MeshInfo* meshInfo) override;
};
