#ifndef KLEIN_WBOITMATERIAL_H
#define KLEIN_WBOITMATERIAL_H

#include <Klein/Core/Export.h>

#include <QColor>
#include <QVariant>
#include <Qt3DCore/QEntity>
#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QParameter>
#include <Qt3DRender/QRenderTarget>
#include <Qt3DRender/QRenderTargetOutput>
#include <Qt3DRender/QTexture>

namespace Qt3DRender
{
class QEffect;
class QFrameGraphNode;
} // namespace Qt3DRender

namespace Klein
{

class KLEIN_API WBOITCompositor : public Qt3DCore::QNode
{
    Q_OBJECT

public:
    explicit WBOITCompositor(Qt3DCore::QNode* parent = nullptr);

    virtual ~WBOITCompositor() = default;

    Qt3DRender::QFrameGraphNode* attachRenderTargetTo(
        Qt3DRender::QFrameGraphNode* parent);

    Qt3DRender::QFrameGraphNode* attachCompositionPassTo(
        Qt3DRender::QFrameGraphNode* parent);

    QSize size() const { return m_size; }

public slots:
    void setSize(const QSize& size)
    {
        m_size = size;
        m_accumTexture->setSize(size.width(), size.height());
        m_revealageTexture->setSize(size.width(), size.height());
        m_depthTexture->setSize(size.width(), size.height());
    }

private:
    void createTarget(Qt3DCore::QNode* parent);

    void createEntity(Qt3DCore::QNode* parent);

private:
    QSize m_size;
    Qt3DRender::QRenderTarget* m_renderTarget;
    Qt3DRender::QRenderTargetOutput* m_accumOutput;
    Qt3DRender::QRenderTargetOutput* m_revealageOutput;
    Qt3DRender::QRenderTargetOutput* m_depthOutput;
    Qt3DRender::QTexture2D* m_accumTexture;
    Qt3DRender::QTexture2D* m_revealageTexture;
    Qt3DRender::QTexture2D* m_depthTexture;
    Qt3DCore::QEntity* m_entity;
};

class KLEIN_API WBOITMaterial : public Qt3DRender::QMaterial
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
    explicit WBOITMaterial(Qt3DCore::QNode* parent = nullptr);

    WBOITMaterial(ColorMode mode, Qt3DCore::QNode* parent = nullptr);

    virtual ~WBOITMaterial() = default;

    QColor baseColor() const { return m_baseColor->value().value<int>(); }

    const Qt3DRender::QAbstractTexture* baseColorMap() const
    {
        return m_baseColorMap->value().value<Qt3DRender::QAbstractTexture*>();
    }

    ColorMode colorMode() const
    {
        return m_colorMode->value().value<ColorMode>();
    }

    float depthScale() const { return m_depthScale->value().value<float>(); }

    float texCoordOffset() const
    {
        return m_texCoordOffset->value().value<float>();
    }

    float texCoordScale() const
    {
        return m_texCoordScale->value().value<float>();
    }

public slots:
    void setBaseColor(const QColor& value) { m_baseColor->setValue(value); }

    void setBaseColorMap(Qt3DRender::QAbstractTexture* value)
    {
        if (!m_baseColorMapInitialized) {
            m_baseColorMap = new Qt3DRender::QParameter(
                QStringLiteral("baseColorMap"), value, this);
            this->addParameter(m_baseColorMap);
            m_baseColorMapInitialized = true;
        }
        else {
            m_baseColorMap->setValue(QVariant::fromValue(value));
        }
    }

    void setColorMode(ColorMode value) { m_colorMode->setValue(value); }

    void setDepthScale(float value) { m_depthScale->setValue(value); }

    void setTexCoordOffset(float value) { m_texCoordOffset->setValue(value); }

    void setTexCoordScale(float value) { m_texCoordScale->setValue(value); }

public:
    static Qt3DRender::QFrameGraphNode* attachTranparentPassTo(
        Qt3DRender::QFrameGraphNode* parent);

protected:
    bool m_baseColorMapInitialized = false;
    bool m_envLightBrdfInitialized = false;
    bool m_shadowMapInitialized = false;
    Qt3DRender::QParameter* m_baseColor;
    Qt3DRender::QParameter* m_baseColorMap;
    Qt3DRender::QParameter* m_colorMode;
    Qt3DRender::QParameter* m_depthScale;
    Qt3DRender::QParameter* m_texCoordOffset;
    Qt3DRender::QParameter* m_texCoordScale;
};

} // namespace Klein

Q_DECLARE_METATYPE(Klein::WBOITMaterial::ColorMode)

#endif