#include <Klein/Util/RandomColorGenerator.h>

#include <QRandomGenerator>
#include <QTime>

namespace Klein
{

RandomColorGenerator::RandomColorGenerator(int n)
{
    auto t = QTime::currentTime();
    RandomColorGenerator(n, static_cast<quint32>(t.msec()));
}

RandomColorGenerator::RandomColorGenerator(int n, quint32 seed)
{
    m_palette.resize(n);
    QRandomGenerator rng(seed);
    for (int i = 0; i < m_palette.size(); ++i) {
        m_palette[i].setRed(rng.bounded(256));
        m_palette[i].setGreen(rng.bounded(256));
        m_palette[i].setBlue(rng.bounded(256));
    }
}

QColor RandomColorGenerator::getRandomColor()
{
    QColor c;
    c.setRed(QRandomGenerator::global()->bounded(256));
    c.setGreen(QRandomGenerator::global()->bounded(256));
    c.setBlue(QRandomGenerator::global()->bounded(256));
    return c;
}

} // namespace Klein
