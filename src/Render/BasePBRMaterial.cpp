#include <Klein/Render/BasePBRMaterial.h>

#include <QString>

namespace Klein
{

BasePBRMaterial::BasePBRMaterial(Qt3DCore::QNode* parent)
    : BasePBRMaterial(BASECOLOR_MODE, parent)
{}

BasePBRMaterial::BasePBRMaterial(ColorMode mode, Qt3DCore::QNode* parent)
    : Qt3DRender::QMaterial(parent)
{
    m_baseColor = new Qt3DRender::QParameter(
        QStringLiteral("baseColor"), QColor("white"), this);
    m_colorMode =
        new Qt3DRender::QParameter(QStringLiteral("colorMode"), mode, this);
    m_metalness =
        new Qt3DRender::QParameter(QStringLiteral("metalness"), 0.0f, this);
    m_roughness =
        new Qt3DRender::QParameter(QStringLiteral("roughness"), 0.0f, this);
    m_shift = new Qt3DRender::QParameter(QStringLiteral("shift"), 0.0f, this);
    m_texCoordOffset = new Qt3DRender::QParameter(
        QStringLiteral("texCoordOffset"), 0.0f, this);
    m_texCoordScale =
        new Qt3DRender::QParameter(QStringLiteral("texCoordScale"), 1.0f, this);
    this->addParameter(m_baseColor);
    this->addParameter(m_colorMode);
    this->addParameter(m_metalness);
    this->addParameter(m_roughness);
    this->addParameter(m_shift);
    this->addParameter(m_texCoordOffset);
    this->addParameter(m_texCoordScale);
}

} // namespace Klein
