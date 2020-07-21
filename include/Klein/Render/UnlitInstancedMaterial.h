#ifndef KLEIN_UNLITINSTANCEDMATERIAL_H
#define KLEIN_UNLITINSTANCEDMATERIAL_H

#include <Klein/Core/Export.h>
#include <Klein/Render/BaseUnlitMaterial.h>

namespace Klein
{

class KLEIN_API UnlitInstancedMaterial : public BaseUnlitMaterial
{
    Q_OBJECT

public:
    explicit UnlitInstancedMaterial(Qt3DCore::QNode* parent = nullptr);

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
            mask = RENDER_MODE_VCOLOR;
            m_renderMode->setValue(mask);
        }
    }

public:
    static constexpr const char* effectName{ "KLEIN_EFFECT_UNLIT_INSTANCED" };

    static Qt3DRender::QEffect* createEffect();

private:
    bool m_baseColorMapInitialized = false;
    Qt3DRender::QParameter* m_useInstanceColor;
};

} // namespace Klein

#endif
