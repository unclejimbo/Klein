#include <Klein/Render/UnlitInstancedMaterial.h>

#include <Klein/Render/ResourceManager.h>
#include <Klein/Render/ShaderProgram.h>
#include <QString>
#include <Qt3DRender/QEffect>

namespace Klein
{

UnlitInstancedMaterial::UnlitInstancedMaterial(Qt3DCore::QNode* parent)
    : BaseUnlitMaterial(parent)
{
    m_useInstanceColor = new Qt3DRender::QParameter(
        QStringLiteral("useInstanceColor"), false, this);
    this->addParameter(m_useInstanceColor);
    auto effect = gResourceManager().get<Qt3DRender::QEffect>(effectName);
    if (effect == nullptr) {
        effect = createEffect();
        gResourceManager().put(effectName, effect);
    }
    this->setEffect(effect);
}

const QString UnlitInstancedMaterial::effectName{
    "KLEIN_EFFECT_UNLIT_INSTANCED"
};

Qt3DRender::QEffect* UnlitInstancedMaterial::createEffect()
{
    QString shaderPath("data/shader/");
    auto shader =
        createShader(shaderPath + QStringLiteral("ShadingInstanced.vert"),
                     shaderPath + QStringLiteral("Unlit.frag"));
    return createEffectImpl(shader);
}

} // namespace Klein
