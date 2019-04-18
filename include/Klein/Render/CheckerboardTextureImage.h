#ifndef KLEIN_CHECKERBOARD_H
#define KLEIN_CHECKERBOARD_H

#include <Klein/Core/Export.h>

#include <QColor>
#include <QSharedPointer>
#include <QVarLengthArray>
#include <Qt3DRender/QAbstractTextureImage>
#include <Qt3DRender/QTextureImageDataGenerator>

namespace Klein
{

class CheckerboardTextureImageDataGenerator
    : public Qt3DRender::QTextureImageDataGenerator
{
public:
    QT3D_FUNCTOR(CheckerboardTextureImageDataGenerator)

    CheckerboardTextureImageDataGenerator(int size,
                                          int count,
                                          const QColor& color1,
                                          const QColor& color2)
        : m_size(size), m_count(count), m_colors({ color1, color2 })
    {}

    virtual ~CheckerboardTextureImageDataGenerator() {}

    Qt3DRender::QTextureImageDataPtr operator()() override;

    bool operator==(
        const Qt3DRender::QTextureImageDataGenerator& other) const override;

private:
    int m_size;
    int m_count;
    QVarLengthArray<QColor, 2> m_colors;
};

class KLEIN_API CheckerboardTextureImage
    : public Qt3DRender::QAbstractTextureImage
{
    Q_OBJECT

public:
    CheckerboardTextureImage(Qt3DCore::QNode* parent = nullptr)
        : Qt3DRender::QAbstractTextureImage(parent)
    {}

    virtual ~CheckerboardTextureImage() {}

public slots:
    void setSize(int size)
    {
        m_size = size;
        this->notifyDataGeneratorChanged();
    }

    void setCount(int count)
    {
        m_count = count;
        this->notifyDataGeneratorChanged();
    };

    void setColors(const QColor& color1, const QColor& color2)
    {
        m_colors = { color1, color2 };
        this->notifyDataGeneratorChanged();
    }

protected:
    Qt3DRender::QTextureImageDataGeneratorPtr dataGenerator() const override
    {
        QSharedPointer<CheckerboardTextureImageDataGenerator> generator(
            new CheckerboardTextureImageDataGenerator(
                m_size, m_count, m_colors[0], m_colors[1]));
        return generator;
    }

private:
    int m_size = 1000;
    int m_count = 10;
    QVarLengthArray<QColor, 2> m_colors{ Qt::white, Qt::black };
};

} // namespace Klein

#endif
