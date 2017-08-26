#pragma once

#include <QString>

enum GeomType : unsigned
{
	GEOM_TYPE_NONE = 0,
	GEOM_TYPE_MESH,
	GEOM_TYPE_POINTCLOUD
};

struct GeomInfo
{
	QString filename;
	GeomType type;
	unsigned id;
};
