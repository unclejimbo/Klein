#include <Klein/Render/PBRMaterial.h>

#include <Klein/Render/ResourceManager.h>

namespace Klein
{

PBRMaterial::PBRMaterial(Qt3DCore::QNode* parent) : BasePBRMaterial(parent)
{
    auto effect = createEffect(
        gResourceManager().get<Qt3DRender::QShaderProgram>(BUILTIN_SHADER_PBR));
    this->setEffect(effect);
}

} // namespace Klein
