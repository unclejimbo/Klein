#include <Klein/Render/PBRInstancedMaterial.h>

#include <Klein/Render/ForwardRenderingEffect.h>
#include <Klein/Render/ResourceManager.h>
#include <QString>
#include <Qt3DRender/QEffect>

namespace Klein
{

PBRInstancedMaterial::PBRInstancedMaterial(Qt3DCore::QNode* parent)
    : Qt3DRender::QMaterial(parent)
{
    m_ambientness =
        new Qt3DRender::QParameter(QStringLiteral("ambientness"), 0.0f, this);
    m_metalness =
        new Qt3DRender::QParameter(QStringLiteral("metalness"), 0.0f, this);
    m_roughness =
        new Qt3DRender::QParameter(QStringLiteral("roughness"), 0.0f, this);
    this->addParameter(m_ambientness);
    this->addParameter(m_metalness);
    this->addParameter(m_roughness);
    this->addParameter(new Qt3DRender::QParameter(
        QStringLiteral("colorMode"),
        2,
        this)); // instancing needs to use VCOLOR_MODE
    this->setEffect(gResourceManager().get<Qt3DRender::QEffect>(
        BUILTIN_EFFECT_PBRINSTANCED));
}

} // namespace Klein
