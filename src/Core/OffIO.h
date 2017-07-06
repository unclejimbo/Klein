#pragma once

#include "Core/GeomIO.h"

class OffIO : public GeomIO
{
private:
	bool _readMesh(QTextStream& stream, unsigned& positionBufferID, unsigned& normalBufferID,
		GeomInfo* geomInfo) override;
	bool _readPointCloud(QTextStream& stream, unsigned& positionBufferID,
		GeomInfo* geomInfo) override;
};
