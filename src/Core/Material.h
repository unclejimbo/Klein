#pragma once

#include <QVector3D>

struct PBRMaterial
{
	QVector3D albedo;
	float roughness;
	float metallic;
	float ao;
};
