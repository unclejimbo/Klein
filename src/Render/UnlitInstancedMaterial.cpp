#include <Klein/Render/UnlitInstancedMaterial.h>

#include <Klein/Render/ResourceManager.h>
#include <QString>

namespace Klein
{

UnlitInstancedMaterial::UnlitInstancedMaterial(Qt3DCore::QNode* parent)
    : BaseUnlitMaterial(parent)
{
    m_useInstanceColor = new Qt3DRender::QParameter(
        QStringLiteral("useInstanceColor"), false, this);
    this->addParameter(m_useInstanceColor);
    auto effect =
        createEffect(gResourceManager().get<Qt3DRender::QShaderProgram>(
            BUILTIN_SHADER_UNLIT_INSTANCED));
    this->setEffect(effect);
}

} // namespace Klein
