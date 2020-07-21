#ifndef KLEIN_PBRMATERIAL_H
#define KLEIN_PBRMATERIAL_H

#include <Klein/Core/Export.h>
#include <Klein/Render/BasePBRMaterial.h>

namespace Klein
{

class KLEIN_API PBRMaterial : public BasePBRMaterial
{
    Q_OBJECT

public:
    explicit PBRMaterial(Qt3DCore::QNode* parent = nullptr);

public:
    static Qt3DRender::QEffect* createEffect(bool castShadow = false);

protected:
    static constexpr const char* effectName_CastShadow{
        "KLEIN_EFFECT_PBR_CAST_SHADOW"
    };

    static constexpr const char* effectName_NoShadow{
        "KLEIN_EFFECT_PBR_NO_SHADOW"
    };

    Qt3DRender::QEffect* getEffectVariant(bool castSahdow) override;
};

} // namespace Klein

#endif
