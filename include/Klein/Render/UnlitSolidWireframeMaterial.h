#ifndef KLEIN_UNLITSOLIDWIREFRAMEMATERIAL_H
#define KLEIN_UNLITSOLIDWIREFRAMEMATERIAL_H

#include <Klein/Core/Export.h>
#include <Klein/Render/BaseUnlitMaterial.h>

namespace Klein
{

class KLEIN_API UnlitSolidWireframeMaterial : public BaseUnlitMaterial
{
    Q_OBJECT

public:
    explicit UnlitSolidWireframeMaterial(Qt3DCore::QNode* parent = nullptr);

    UnlitSolidWireframeMaterial(ColorMode mode,
                                Qt3DCore::QNode* parent = nullptr);

    virtual ~UnlitSolidWireframeMaterial() = default;

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
