#include <Klein/Render/UnlitSolidWireframeMaterial.h>

#include <Klein/Render/ResourceManager.h>
#include <QString>

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
    auto effect =
        createEffect(gResourceManager().get<Qt3DRender::QShaderProgram>(
            BUILTIN_SHADER_UNLIT_SOLIDWIREFRAME));
    this->setEffect(effect);
}

} // namespace Klein
