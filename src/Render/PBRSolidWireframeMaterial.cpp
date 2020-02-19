#include <Klein/Render/PBRSolidWireframeMaterial.h>

#include <Klein/Render/ResourceManager.h>
#include <Klein/Render/ShaderProgram.h>
#include <QString>
#include <Qt3DRender/QEffect>

namespace Klein
{

PBRSolidWireframeMaterial::PBRSolidWireframeMaterial(Qt3DCore::QNode* parent)
    : BasePBRMaterial(parent)
{
    m_lineColor = new Qt3DRender::QParameter(
        QStringLiteral("lineColor"), QColor("black"), this);
    m_lineWidth =
        new Qt3DRender::QParameter(QStringLiteral("lineWidth"), 1.0f, this);
    this->addParameter(m_lineColor);
    this->addParameter(m_lineWidth);
    this->setEffect(getEffectVariant(isShadowCastingEnabled()));
}

Qt3DRender::QEffect* PBRSolidWireframeMaterial::createEffect(bool castShadow)
{
    QString shaderPath("data/shader/");
    auto shader =
        createShader(shaderPath + QStringLiteral("ShadingSolidWireframe.vert"),
                     shaderPath + QStringLiteral("ShadingSolidWireframe.geom"),
                     shaderPath + QStringLiteral("PBRSolidWireframe.frag"));
    return createEffectImpl(shader, castShadow);
}

const QString PBRSolidWireframeMaterial::effectName_CastShadow{
    "KLEIN_EFFECT_PBR_SOLID_WIREFRAME_CAST_SHADOW"
};

const QString PBRSolidWireframeMaterial::effectName_NoShadow{
    "KLEIN_EFFECT_PBR_SOLID_WIREFRAME_NO_SHADOW"
};

Qt3DRender::QEffect* PBRSolidWireframeMaterial::getEffectVariant(
    bool castShadow)
{
    Qt3DRender::QEffect* effect = nullptr;
    if (castShadow) {
        effect =
            gResourceManager().get<Qt3DRender::QEffect>(effectName_CastShadow);
        if (effect == nullptr) {
            effect = createEffect(castShadow);
            gResourceManager().put(effectName_CastShadow, effect);
        }
    }
    else {
        effect =
            gResourceManager().get<Qt3DRender::QEffect>(effectName_NoShadow);
        if (effect == nullptr) {
            effect = createEffect(castShadow);
            gResourceManager().put(effectName_NoShadow, effect);
        }
    }
    return effect;
}

} // namespace Klein
