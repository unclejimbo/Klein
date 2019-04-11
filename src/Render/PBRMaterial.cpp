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
    this->setEffect(
        gResourceManager().get<Qt3DRender::QEffect>(BUILTIN_EFFECT_PBR));
}

} // namespace Klein
