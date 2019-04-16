#include <Klein/Render/PBRMaterial.h>

#include <Klein/Render/ResourceManager.h>

namespace Klein
{

PBRMaterial::PBRMaterial(Qt3DCore::QNode* parent)
    : PBRMaterial(BASECOLOR_MODE, parent)
{}

PBRMaterial::PBRMaterial(ColorMode mode, Qt3DCore::QNode* parent)
    : BasePBRMaterial(mode, parent)
{
    auto effect = createEffect(
        gResourceManager().get<Qt3DRender::QShaderProgram>(BUILTIN_SHADER_PBR));
    this->setEffect(effect);
}

} // namespace Klein
