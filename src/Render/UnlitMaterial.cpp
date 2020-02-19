#include <Klein/Render/UnlitMaterial.h>

#include <Klein/Render/ResourceManager.h>
#include <Klein/Render/ShaderProgram.h>
#include <QString>
#include <Qt3DRender/QEffect>

namespace Klein
{

UnlitMaterial::UnlitMaterial(Qt3DCore::QNode* parent)
    : BaseUnlitMaterial(parent)
{
    auto effect = gResourceManager().get<Qt3DRender::QEffect>(effectName);
    if (effect == nullptr) {
        effect = createEffect();
        gResourceManager().put(effectName, effect);
    }
    this->setEffect(effect);
}

const QString UnlitMaterial::effectName{ "KLEIN_EFFECT_UNLIT" };

Qt3DRender::QEffect* UnlitMaterial::createEffect()
{
    QString shaderPath("data/shader/");
    auto shader = createShader(shaderPath + QStringLiteral("Shading.vert"),
                               shaderPath + QStringLiteral("Unlit.frag"));
    return createEffectImpl(shader);
}

} // namespace Klein
