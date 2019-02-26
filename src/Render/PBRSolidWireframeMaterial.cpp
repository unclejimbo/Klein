#include <Klein/Render/PBRSolidWireframeMaterial.h>

#include <Klein/Render/ForwardRenderingEffect.h>
#include <Klein/Render/ResourceManager.h>
#include <QString>
#include <Qt3DRender/QEffect>

namespace Klein
{

PBRSolidWireframeMaterial::PBRSolidWireframeMaterial(Qt3DCore::QNode* parent)
    : PBRSolidWireframeMaterial(BASECOLOR_MODE, parent)
{}

PBRSolidWireframeMaterial::PBRSolidWireframeMaterial(ColorMode mode,
                                                     Qt3DCore::QNode* parent)
    : Qt3DRender::QMaterial(parent)
{
    m_ambientness =
        new Qt3DRender::QParameter(QStringLiteral("ambientness"), 0.0f, this);
    m_baseColor = new Qt3DRender::QParameter(
        QStringLiteral("baseColor"), QColor("white"), this);
    ;
    m_colorMode =
        new Qt3DRender::QParameter(QStringLiteral("colorMode"), mode, this);
    m_lineColor = new Qt3DRender::QParameter(
        QStringLiteral("lineColor"), QColor("black"), this);
    m_lineWidth =
        new Qt3DRender::QParameter(QStringLiteral("lineWidth"), 1.0f, this);
    m_metalness =
        new Qt3DRender::QParameter(QStringLiteral("metalness"), 0.0f, this);
    m_roughness =
        new Qt3DRender::QParameter(QStringLiteral("roughness"), 0.0f, this);
    m_shift = new Qt3DRender::QParameter(QStringLiteral("shift"), 0.0f, this);
    m_texCoordOffset = new Qt3DRender::QParameter(
        QStringLiteral("texCoordOffset"), 0.0f, this);
    m_texCoordScale =
        new Qt3DRender::QParameter(QStringLiteral("texCoordScale"), 1.0f, this);
    this->addParameter(m_ambientness);
    this->addParameter(m_baseColor);
    this->addParameter(m_colorMode);
    this->addParameter(m_lineColor);
    this->addParameter(m_lineWidth);
    this->addParameter(m_metalness);
    this->addParameter(m_roughness);
    this->addParameter(m_shift);
    this->addParameter(m_texCoordOffset);
    this->addParameter(m_texCoordScale);
    this->setEffect(gResourceManager().get<Qt3DRender::QEffect>(
        BUILTIN_EFFECT_PBRSOLIDWIREFRAME));
}

} // namespace Klein
