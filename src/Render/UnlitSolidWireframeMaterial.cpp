#include <Klein/Render/UnlitSolidWireframeMaterial.h>

#include <Klein/Render/ResourceManager.h>
#include <Klein/Render/ShaderProgram.h>
#include <QString>
#include <Qt3DRender/QEffect>

namespace Klein
{

UnlitSolidWireframeMaterial::UnlitSolidWireframeMaterial(
    Qt3DCore::QNode* parent)
    : BaseUnlitMaterial(parent)
{
    m_lineColor = new Qt3DRender::QParameter(
        QStringLiteral("lineColor"), QColor("black"), this);
    m_lineWidth =
        new Qt3DRender::QParameter(QStringLiteral("lineWidth"), 1.0f, this);
    this->addParameter(m_lineColor);
    this->addParameter(m_lineWidth);
    auto effect = gResourceManager().get<Qt3DRender::QEffect>(effectName);
    if (effect == nullptr) {
        effect = createEffect();
        gResourceManager().put(effectName, effect);
    }
    this->setEffect(effect);
}

Qt3DRender::QEffect* UnlitSolidWireframeMaterial::createEffect()
{
    const QString shaderPath("data/shader/");
    const auto shader =
        createShader(shaderPath + QStringLiteral("ShadingSolidWireframe.vert"),
                     shaderPath + QStringLiteral("ShadingSolidWireframe.geom"),
                     shaderPath + QStringLiteral("UnlitSolidWireframe.frag"));
    return createEffectImpl(shader);
}

} // namespace Klein
