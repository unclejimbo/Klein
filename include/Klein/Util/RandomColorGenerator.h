#ifndef KLEIN_RANDOMCOLORGENERATOR_H
#define KLEIN_RANDOMCOLORGENERATOR_H

#include <Klein/Core/Export.h>

#include <QColor>
#include <QVector>

namespace Klein
{

class KLEIN_API RandomColorGenerator
{
public:
    RandomColorGenerator(int n);

    RandomColorGenerator(int n, quint32 seed);

    QColor getColor(int i) const { return m_palette[i]; }

    QVector<QColor> getPalette() const { return m_palette; }

public:
    static QColor getRandomColor();

private:
    QVector<QColor> m_palette;
};

} // namespace Klein

#endif
