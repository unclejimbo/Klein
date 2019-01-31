#include <Klein/Render/PBRMaterial.h>

#include <Klein/Render/ForwardRenderingEffect.h>
#include <Klein/Render/ResourceManager.h>
#include <QString>
#include <QVector3D>
#include <Qt3DRender/QEffect>
#include <Qt3DRender/QParameter>
#include <Qt3DRender/QTexture>

namespace Klein
{

QSharedPointer<Qt3DRender::QEffect> PBRMaterial::pbrEffect =
    createForwardRenderingEffect(gResourceManager().get(KLEIN_SHADER_PBR));

PBRMaterial::PBRMaterial(Qt3DCore::QNode* parent)
    : PBRMaterial(PBRMaterialParams(), parent)
{}

PBRMaterial::PBRMaterial(PBRMaterialParams params, Qt3DCore::QNode* parent)
    : Qt3DRender::QMaterial(parent)
{
    m_ambientness = params.ambientness;
    m_baseColor = params.baseColor;
    m_baseColorMap = params.baseColorMap;
    m_metalness = params.metalness;
    m_roughness = params.roughness;
    m_texCoordOffset = params.texCoordOffset;
    m_texCoordScale = params.texCoordScale;
    m_useTexture = params.useTexture;
    build();
}

void PBRMaterial::build()
{
    if (m_ambientness == nullptr) {
        m_ambientness = new Qt3DRender::QParameter(
            QStringLiteral("ambientness"), 0.0f, this);
    }
    if (m_baseColor == nullptr) {
        m_baseColor = new Qt3DRender::QParameter(
            QStringLiteral("baseColor"), QVector3D(1.0f, 1.0f, 1.0f), this);
    }
    if (m_baseColorMap == nullptr) {
        m_baseColorMap =
            new Qt3DRender::QParameter(QStringLiteral("baseColorMap"),
                                       new Qt3DRender::QTexture2D(this),
                                       this);
    }
    if (m_metalness == nullptr) {
        m_metalness =
            new Qt3DRender::QParameter(QStringLiteral("metalness"), 0.0f, this);
    }
    if (m_roughness == nullptr) {
        m_roughness =
            new Qt3DRender::QParameter(QStringLiteral("roughness"), 0.0f, this);
    }
    if (m_texCoordOffset == nullptr) {
        m_texCoordOffset = new Qt3DRender::QParameter(
            QStringLiteral("texCoordOffset"), 0.0f, this);
    }
    if (m_texCoordScale == nullptr) {
        m_texCoordScale = new Qt3DRender::QParameter(
            QStringLiteral("texCoordScale"), 0.0f, this);
    }
    if (m_useTexture == nullptr) {
        m_useTexture = new Qt3DRender::QParameter(
            QStringLiteral("useTexture"), false, this);
    }
    this->addParameter(m_ambientness);
    this->addParameter(m_baseColor);
    this->addParameter(m_baseColorMap);
    this->addParameter(m_metalness);
    this->addParameter(m_roughness);
    this->addParameter(m_texCoordOffset);
    this->addParameter(m_texCoordScale);
    this->addParameter(m_useTexture);
    this->setEffect(pbrEffect.get());
}

} // namespace Klein
