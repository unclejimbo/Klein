#ifndef KLEIN_COLORPALETTE_H
#define KLEIN_COLORPALETTE_H

#include <Klein/Core/Export.h>

#include <QColor>
#include <QVector>
#include <QtDebug>

namespace Klein
{

class KLEIN_API ColorPalette
{
public:
    ColorPalette() noexcept;

    ColorPalette(int n,
                 int saturationLow = 200,
                 int brightnessLow = 200,
                 quint32 seed = 0);

    QColor color(int i) const { return m_palette[i % m_palette.size()]; }

    QVector<QColor> palette() const { return m_palette; }

    int size() const { return m_palette.size(); }

private:
    QVector<QColor> m_palette;
};

} // namespace Klein

#endif
