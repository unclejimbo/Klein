#include <Klein/Render/PBRInstancedMaterial.h>

#include <Klein/Render/ForwardRenderingEffect.h>
#include <Klein/Render/ResourceManager.h>
#include <QString>
#include <Qt3DRender/QEffect>

namespace Klein
{

PBRInstancedMaterial::PBRInstancedMaterial(Qt3DCore::QNode* parent)
    : PBRInstancedMaterial(false, BASECOLOR_MODE, parent)
{}

PBRInstancedMaterial::PBRInstancedMaterial(bool useInstanceColor,
                                           ColorMode mode,
                                           Qt3DCore::QNode* parent)
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
    m_texCoordOffset = new Qt3DRender::QParameter(
        QStringLiteral("texCoordOffset"), 0.0f, this);
    m_texCoordScale =
        new Qt3DRender::QParameter(QStringLiteral("texCoordScale"), 1.0f, this);
    m_useInstanceColor = new Qt3DRender::QParameter(
        QStringLiteral("useInstanceColor"), useInstanceColor, this);
    this->addParameter(m_baseColor);
    this->addParameter(m_colorMode);
    this->addParameter(m_metalness);
    this->addParameter(m_roughness);
    this->addParameter(m_texCoordOffset);
    this->addParameter(m_texCoordScale);
    this->addParameter(m_useInstanceColor);
    this->setEffect(gResourceManager().get<Qt3DRender::QEffect>(
        BUILTIN_EFFECT_PBRINSTANCED));
}

} // namespace Klein
