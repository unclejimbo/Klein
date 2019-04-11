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
                         ColorMode mode,
                         Qt3DCore::QNode* parent = nullptr);

    virtual ~PBRInstancedMaterial() = default;

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
