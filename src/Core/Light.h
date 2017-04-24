#pragma once

#ifdef _WIN32
#include <CodeAnalysis/Warnings.h>
#pragma warning(push)
#pragma warning(disable : ALL_CODE_ANALYSIS_WARNINGS)
#endif

#include <QVector3D>

#ifdef _WIN32
#pragma warning(pop)
#endif

struct Light
{
	QVector3D position_w;
	QVector3D color;
};
