#pragma once

#include "Core/GeomIO.h"

class OffIO : public GeomIO
{
public:
	OffIO();
	~OffIO();

private:
	bool _readMesh(QTextStream& stream, unsigned& positionBufferID, unsigned& normalBufferID,
		GeomInfo* geomInfo) override;
	bool _readPointCloud(QTextStream& stream, unsigned& positionBufferID,
		GeomInfo* geomInfo) override;
};
