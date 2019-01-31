#include <Klein/Render/CheckerboardTextureImage.h>

#include <Qt3DRender/QTextureImageData>

namespace Klein
{

Qt3DRender::QTextureImageDataPtr CheckerboardTextureImageDataGenerator::
operator()()
{
    Qt3DRender::QTextureImageDataPtr image(new Qt3DRender::QTextureImageData);
    image->setFormat(QOpenGLTexture::RGB8_UNorm);
    image->setWidth(m_size);
    image->setHeight(m_size);
    image->setPixelFormat(QOpenGLTexture::RGB);
    image->setPixelType(QOpenGLTexture::UInt8);
    image->setTarget(QOpenGLTexture::Target2D);
    image->setLayers(1);
    image->setDepth(1);
    image->setFaces(1);
    image->setMipLevels(1);

    QByteArray bytes;
    auto size = m_size * m_size * 3;
    bytes.resize(size * sizeof(uint8_t));
    auto data = reinterpret_cast<uint8_t*>(bytes.data());
    auto gridSize = m_size / m_count;
    int offset = gridSize / 2; // symmetry on border
    for (size_t i = 0; i < m_size; ++i) {
        for (size_t j = 0; j < m_size; ++j) {
            int s = i - offset;
            int t = j - offset;
            s = s < 0 ? m_size + s : s;
            t = t < 0 ? m_size + t : t;
            auto c = (s / gridSize + t / gridSize) % 2;
            auto idx = 3 * (i * m_size + j);
            data[idx + 0] = static_cast<uint8_t>(m_colors[c].red());
            data[idx + 1] = static_cast<uint8_t>(m_colors[c].green());
            data[idx + 2] = static_cast<uint8_t>(m_colors[c].blue());
        }
    }
    image->setData(bytes, bytes.size());
    return image;
}

bool CheckerboardTextureImageDataGenerator::operator==(
    const Qt3DRender::QTextureImageDataGenerator& other) const
{
    const auto otherFunctor =
        functor_cast<CheckerboardTextureImageDataGenerator>(&other);
    if (otherFunctor != nullptr) {
        return (otherFunctor->m_size == m_size &&
                otherFunctor->m_count == m_count &&
                otherFunctor->m_colors == m_colors);
    }
    return false;
}

} // namespace Klein
