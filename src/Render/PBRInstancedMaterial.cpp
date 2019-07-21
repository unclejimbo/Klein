#include <Klein/Render/PBRInstancedMaterial.h>

#include <Klein/Render/ResourceManager.h>
#include <QString>

namespace Klein
{

PBRInstancedMaterial::PBRInstancedMaterial(Qt3DCore::QNode* parent)
    : BasePBRMaterial(parent)
{
    m_useInstanceColor = new Qt3DRender::QParameter(
        QStringLiteral("useInstanceColor"), false, this);
    this->addParameter(m_useInstanceColor);
    auto effect =
        createEffect(gResourceManager().get<Qt3DRender::QShaderProgram>(
            BUILTIN_SHADER_PBR_INSTANCED));
    this->setEffect(effect);
}

} // namespace Klein
