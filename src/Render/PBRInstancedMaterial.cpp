#include <Klein/Render/PBRInstancedMaterial.h>

#include <Klein/Render/ResourceManager.h>
#include <QString>

namespace Klein
{

PBRInstancedMaterial::PBRInstancedMaterial(Qt3DCore::QNode* parent)
    : PBRInstancedMaterial(false, BASECOLOR_MODE, parent)
{}

PBRInstancedMaterial::PBRInstancedMaterial(bool useInstanceColor,
                                           ColorMode mode,
                                           Qt3DCore::QNode* parent)
    : BasePBRMaterial(mode, parent)
{
    m_useInstanceColor = new Qt3DRender::QParameter(
        QStringLiteral("useInstanceColor"), useInstanceColor, this);
    this->addParameter(m_useInstanceColor);
    auto effect =
        createEffect(gResourceManager().get<Qt3DRender::QShaderProgram>(
            BUILTIN_SHADER_PBR_INSTANCED));
    this->setEffect(effect);
}

} // namespace Klein
