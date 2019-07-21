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

    virtual ~PBRSolidWireframeMaterial() = default;

    QColor lineColor() const { return m_lineColor->value().value<QColor>(); }

    float lineWidth() const { return m_lineColor->value().value<float>(); }

public slots:
    void setLineColor(const QColor& value) { m_lineColor->setValue(value); }

    void setLineWidth(float value) { m_lineWidth->setValue(value); }

private:
    Qt3DRender::QParameter* m_lineColor;
    Qt3DRender::QParameter* m_lineWidth;
};

} // namespace Klein

#endif
