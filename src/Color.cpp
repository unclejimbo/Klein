#include "Color.h"

#ifdef _WIN32
#include <CodeAnalysis/Warnings.h>
#pragma warning(push)
#pragma warning(disable : ALL_CODE_ANALYSIS_WARNINGS)
#endif

#include <QColor>

#ifdef _WIN32
#pragma warning(pop)
#endif

QVector3D c2v(const QColor& color)
{
	return QVector3D(color.redF(), color.greenF(), color.blueF());
}

QVector3D Color::nextColor()
{
	if (Color::_current >= Color::_colors.size()) {
		Color::_current = 0;
	}
	return Color::_colors[_current++];
}

QVector3D Color::get(size_t i)
{
	return _colors[i % _colors.size()];
}

size_t Color::_current = 0;

std::vector<QVector3D> Color::_colors{
	c2v(QColor("blue")), c2v(QColor("burlywood")), c2v(QColor("cadetblue")), c2v(QColor("coral")),
	c2v(QColor("crimson")), c2v(QColor("cyan")), c2v(QColor("darkorange")), c2v(QColor("darkturquoise")),
	c2v(QColor("forestgreen")), c2v(QColor("gold")), c2v(QColor("hotpink")), c2v(QColor("khaki")),
	c2v(QColor("lawngreen")), c2v(QColor("lightgrey")), c2v(QColor("magenta")), c2v(QColor("olive")),
};
