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
    enum ColorMode : int
    {
        BASECOLOR_MODE = 0,
        TEXTURE_MODE,
        VCOLOR_MODE
    };

public:
    explicit PBRInstancedMaterial(Qt3DCore::QNode* parent = nullptr);

    PBRInstancedMaterial(bool useInstanceColor,
                         ColorMode mode,
                         Qt3DCore::QNode* parent = nullptr);

    virtual ~PBRInstancedMaterial() = default;

    float ambientness() const { return m_ambientness->value().value<float>(); }

    QColor baseColor() const { return m_baseColor->value().value<int>(); }

    Qt3DRender::QTexture2D* baseColorMap() const
    {
        return m_baseColorMap->value().value<Qt3DRender::QTexture2D*>();
    }

    ColorMode colorMode() const
    {
        return m_colorMode->value().value<ColorMode>();
    }

    float metalness() const { return m_metalness->value().value<float>(); }

    float roughness() const { return m_roughness->value().value<float>(); }

    float texCoordOffset() const
    {
        return m_texCoordOffset->value().value<float>();
    }

    float texCoordScale() const
    {
        return m_texCoordScale->value().value<float>();
    }

    bool useInstanceColor() const
    {
        return m_useInstanceColor->value().value<bool>();
    }

public slots:
    void setAmbientness(float value) { m_ambientness->setValue(value); }

    void setBaseColor(const QColor& value) { m_baseColor->setValue(value); }

    void setBaseColorMap(Qt3DRender::QTexture2D* value)
    {
        m_baseColorMap->setValue(QVariant::fromValue(value));
    }

    void setColorMode(ColorMode value)
    {
        if (!m_useInstanceColor->value().value<bool>()) {
            m_colorMode->setValue(value);
        }
    }

    void setMetalness(float value) { m_metalness->setValue(value); }

    void setRoughness(float value) { m_roughness->setValue(value); }

    void setTexCoordOffset(float value) { m_texCoordOffset->setValue(value); }

    void setTexCoordScale(float value) { m_texCoordScale->setValue(value); }

    void setUseInstanceColor(bool value)
    {
        m_useInstanceColor->setValue(value);
        if (value) { m_colorMode->setValue(VCOLOR_MODE); }
    }

private:
    Qt3DRender::QParameter* m_ambientness;
    Qt3DRender::QParameter* m_baseColor;
    Qt3DRender::QParameter* m_baseColorMap;
    Qt3DRender::QParameter* m_colorMode;
    Qt3DRender::QParameter* m_instanceColor;
    Qt3DRender::QParameter* m_instanceModel;
    Qt3DRender::QParameter* m_metalness;
    Qt3DRender::QParameter* m_roughness;
    Qt3DRender::QParameter* m_texCoordOffset;
    Qt3DRender::QParameter* m_texCoordScale;
    Qt3DRender::QParameter* m_useInstanceColor;
};

} // namespace Klein

Q_DECLARE_METATYPE(Klein::PBRInstancedMaterial::ColorMode)

#endif