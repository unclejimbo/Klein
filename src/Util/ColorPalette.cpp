#include <Klein/Util/ColorPalette.h>

#include <QRandomGenerator>

namespace Klein
{

ColorPalette::ColorPalette() noexcept
{
    m_palette = {
        "maroon", "chocolate", "olive",  "teal",    "navy",
        "red",    "orange",    "yellow", "lime",    "green",
        "cyan",   "blue",      "purple", "magenta", "grey",
        "pink",   "peachpuff", "beige",  "mint",    "lavender",
    };
}

ColorPalette::ColorPalette(int n,
                           int saturationLow,
                           int brightnessLow,
                           quint32 seed)
{
    m_palette.resize(n);
    QRandomGenerator rng(seed);
    for (int i = 0; i < m_palette.size(); ++i) {
        const auto h = rng.bounded(0, 359);
        const auto s = rng.bounded(saturationLow, 255);
        const auto v = rng.bounded(saturationLow, 255);
        m_palette[i].setHsv(h, s, v);
    }
}

} // namespace Klein
