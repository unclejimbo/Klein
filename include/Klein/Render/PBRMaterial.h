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

    virtual ~PBRMaterial() = default;

public:
    static Qt3DRender::QEffect* createEffect(bool castShadow = false);

protected:
    static const QString effectName_CastShadow;

    static const QString effectName_NoShadow;

    Qt3DRender::QEffect* getEffectVariant(bool castSahdow) override;
};

} // namespace Klein

#endif
