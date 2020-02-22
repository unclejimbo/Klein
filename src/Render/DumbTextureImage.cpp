#include <Klein/Render/DumbTextureImage.h>

#include <Qt3DRender/QTextureImageData>

namespace Klein
{

Qt3DRender::QTextureImageDataPtr DumbTextureImageDataGenerator::operator()()
{
    Qt3DRender::QTextureImageDataPtr image(new Qt3DRender::QTextureImageData);
    image->setFormat(QOpenGLTexture::RGB8_UNorm);
    image->setWidth(4);
    image->setHeight(4);
    image->setPixelFormat(QOpenGLTexture::RGB);
    image->setPixelType(QOpenGLTexture::UInt8);
    image->setTarget(QOpenGLTexture::Target2D);
    image->setLayers(1);
    image->setDepth(1);
    image->setFaces(1);
    image->setMipLevels(1);

    QByteArray bytes;
    bytes.resize(48);
    image->setData(bytes, bytes.size());
    return image;
}

bool DumbTextureImageDataGenerator::operator==(
    const Qt3DRender::QTextureImageDataGenerator& other) const
{
    const auto otherFunctor =
        functor_cast<DumbTextureImageDataGenerator>(&other);
    return otherFunctor != nullptr;
}

} // namespace Klein
