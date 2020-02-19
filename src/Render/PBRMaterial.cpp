#include <Klein/Render/PBRMaterial.h>

#include <Klein/Render/ResourceManager.h>
#include <Klein/Render/ShaderProgram.h>
#include <Qt3DRender/QEffect>

namespace Klein
{

PBRMaterial::PBRMaterial(Qt3DCore::QNode* parent) : BasePBRMaterial(parent)
{
    this->setEffect(getEffectVariant(isShadowCastingEnabled()));
}

Qt3DRender::QEffect* PBRMaterial::createEffect(bool castShadow)
{
    QString shaderPath("data/shader/");
    auto shader = createShader(shaderPath + QStringLiteral("Shading.vert"),
                               shaderPath + QStringLiteral("PBR.frag"));
    return createEffectImpl(shader, castShadow);
}

const QString PBRMaterial::effectName_CastShadow{
    "KLEIN_EFFECT_PBR_CAST_SHADOW"
};

const QString PBRMaterial::effectName_NoShadow{ "KLEIN_EFFECT_PBR_NO_SHADOW" };

Qt3DRender::QEffect* PBRMaterial::getEffectVariant(bool castShadow)
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
