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

    UnlitInstancedMaterial(bool useInstanceColor,
                           ColorMode mode,
                           Qt3DCore::QNode* parent = nullptr);

    virtual ~UnlitInstancedMaterial() = default;

    bool useInstanceColor() const
    {
        return m_useInstanceColor->value().value<bool>();
    }

public slots:
    void setUseInstanceColor(bool value)
    {
        m_useInstanceColor->setValue(value);
        if (value) { m_colorMode->setValue(VCOLOR_MODE); }
    }

private:
    bool m_baseColorMapInitialized = false;
    Qt3DRender::QParameter* m_useInstanceColor;
};

} // namespace Klein

#endif
