#ifndef KLEIN_PBRSOLIDWIREFRAMEMATERIAL_H
#define KLEIN_PBRSOLIDWIREFRAMEMATERIAL_H

#include <Klein/Core/Export.h>
#include <Klein/Render/BasePBRMaterial.h>

namespace Klein
{

class KLEIN_API PBRSolidWireframeMaterial : public BasePBRMaterial
{
    Q_OBJECT

public:
    explicit PBRSolidWireframeMaterial(Qt3DCore::QNode* parent = nullptr);

    QColor lineColor() const { return m_lineColor->value().value<QColor>(); }

    float lineWidth() const { return m_lineColor->value().value<float>(); }

public slots:
    void setLineColor(const QColor& value) { m_lineColor->setValue(value); }

    void setLineWidth(float value) { m_lineWidth->setValue(value); }

public:
    static Qt3DRender::QEffect* createEffect(bool castShadow = false);

protected:
    static constexpr const char* effectName_CastShadow{
        "KLEIN_EFFECT_PBR_SOLID_WIREFRAME_CAST_SHADOW"
    };

    static constexpr const char* effectName_NoShadow{
        "KLEIN_EFFECT_PBR_SOLID_WIREFRAME_NO_SHADOW"
    };

    Qt3DRender::QEffect* getEffectVariant(bool castSahdow) override;

private:
    Qt3DRender::QParameter* m_lineColor;
    Qt3DRender::QParameter* m_lineWidth;
};

} // namespace Klein

#endif
