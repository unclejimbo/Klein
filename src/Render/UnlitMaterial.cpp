#include <Klein/Render/UnlitMaterial.h>

#include <Klein/Render/ResourceManager.h>

namespace Klein
{

UnlitMaterial::UnlitMaterial(Qt3DCore::QNode* parent)
    : UnlitMaterial(BASECOLOR_MODE, parent)
{}

UnlitMaterial::UnlitMaterial(ColorMode mode, Qt3DCore::QNode* parent)
    : BaseUnlitMaterial(mode, parent)
{
    auto effect =
        createEffect(gResourceManager().get<Qt3DRender::QShaderProgram>(
            BUILTIN_SHADER_UNLIT));
    this->setEffect(effect);
}

} // namespace Klein
