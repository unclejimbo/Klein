#ifndef KLEIN_DUMBTEXTUREIMAGE_H
#define KLEIN_DUMBTEXTUREIMAGE_H

#include <Klein/Core/Export.h>

#include <QSharedPointer>
#include <Qt3DRender/QAbstractTextureImage>
#include <Qt3DRender/QTextureImageDataGenerator>

namespace Klein
{

class DumbTextureImageDataGenerator
    : public Qt3DRender::QTextureImageDataGenerator
{
public:
    QT3D_FUNCTOR(DumbTextureImageDataGenerator)

    DumbTextureImageDataGenerator() {}

    Qt3DRender::QTextureImageDataPtr operator()() override;

    bool operator==(
        const Qt3DRender::QTextureImageDataGenerator& other) const override;
};

class KLEIN_API DumbTextureImage : public Qt3DRender::QAbstractTextureImage
{
    Q_OBJECT

public:
    explicit DumbTextureImage(Qt3DCore::QNode* parent = nullptr)
        : Qt3DRender::QAbstractTextureImage(parent)
    {}

protected:
    Qt3DRender::QTextureImageDataGeneratorPtr dataGenerator() const override
    {
        QSharedPointer<DumbTextureImageDataGenerator> generator(
            new DumbTextureImageDataGenerator());
        return generator;
    }
};

} // namespace Klein

#endif
