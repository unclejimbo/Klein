#ifndef KLEIN_PBRINSTANCEDMATERIAL_H
#define KLEIN_PBRINSTANCEDMATERIAL_H

#include <Klein/Core/Export.h>
#include <Klein/Render/BasePBRMaterial.h>

namespace Klein
{

class KLEIN_API PBRInstancedMaterial : public BasePBRMaterial
{
    Q_OBJECT

public:
    explicit PBRInstancedMaterial(Qt3DCore::QNode* parent = nullptr);

    PBRInstancedMaterial(bool useInstanceColor,
                         Qt3DCore::QNode* parent = nullptr);

    bool useInstanceColor() const
    {
        return m_useInstanceColor->value().value<bool>();
    }

public slots:
    void setUseInstanceColor(bool value)
    {
        m_useInstanceColor->setValue(value);
        if (value) {
            auto mask = m_renderMode->value().value<RenderMode>();
            mask =
                (mask & (RENDER_MODE_MATERIAL_MAP | RENDER_MODE_NORMAL_MAP)) |
                RENDER_MODE_VCOLOR;
            m_renderMode->setValue(mask);
        }
    }

public:
    static Qt3DRender::QEffect* createEffect(bool castShadow = false);

protected:
    static constexpr const char* effectName_CastShadow{
        "KLEIN_EFFECT_PBR_INSTANCED_CAST_SHADOW"
    };

    static constexpr const char* effectName_NoShadow{
        "KLEIN_EFFECT_PBR_INSTANCED_NO_SHADOW"
    };

    Qt3DRender::QEffect* getEffectVariant(bool castSahdow) override;

private:
    bool m_baseColorMapInitialized = false;
    Qt3DRender::QParameter* m_useInstanceColor = nullptr;
};

} // namespace Klein

#endif
