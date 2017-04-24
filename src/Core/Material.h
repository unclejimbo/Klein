#pragma once

#include <QVector3D>

struct Material
{
	QVector3D albedo;
	float roughness;
	float metallic;
	float ao;
};
