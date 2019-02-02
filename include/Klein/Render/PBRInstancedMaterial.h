#ifndef KLEIN_PBRINSTANCEDMATERIAL_H
#define KLEIN_PBRINSTANCEDMATERIAL_H

#include <Klein/Core/Export.h>

#include <QColor>
#include <QVariant>
#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QParameter>
#include <Qt3DRender/QTexture>

namespace Klein
{

class KLEIN_API PBRInstancedMaterial : public Qt3DRender::QMaterial
{
    Q_OBJECT

public:
    explicit PBRInstancedMaterial(Qt3DCore::QNode* parent = nullptr);

    float ambientness() const { return m_ambientness->value().value<float>(); }

    float metalness() const { return m_metalness->value().value<float>(); }

    float roughness() const { return m_roughness->value().value<float>(); }

public slots:
    void setAmbientness(float value) { m_ambientness->setValue(value); }

    void setMetalness(float value) { m_metalness->setValue(value); }

    void setRoughness(float value) { m_roughness->setValue(value); }

private:
    Qt3DRender::QParameter* m_ambientness;
    Qt3DRender::QParameter* m_metalness;
    Qt3DRender::QParameter* m_roughness;
};

} // namespace Klein

#endif
